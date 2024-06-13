#include "field.h"

#include <algorithm>
#include <cassert>
#include <chrono>
#include <queue>
#include <random>
#include <unordered_set>

namespace pf {

bool Point::operator==(const Point &other) const {
    return x == other.x && y == other.y;
}

bool Point::operator!=(const Point &other) const {
    return !(*this == other);
}

std::size_t Point::Hasher::operator()(const Point &p) const {
    return std::hash<int>()(p.x) ^ (std::hash<int>()(p.y) * 37);
}

void CellGraph::Init(const std::vector<std::vector<CellType>> &data) {
    nodes_.clear();
    int h = data.size();
    assert(h > 0);
    int w = data[0].size();
    assert(w > 0);

    std::unordered_map<Point, std::vector<Point>, Point::Hasher> points_map;

    // Создать ноды и временный map для соседей
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            if (data[y][x] == CellType::WALL) continue;

            Point p = {x, y};
            std::vector<Point> neighbor_points;

            if (x > 0 && data[y][x - 1] == CellType::EMPTY) {
                neighbor_points.emplace_back(Point{x - 1, y});
            }

            if (x < w - 1 && data[y][x + 1] == CellType::EMPTY) {
                neighbor_points.emplace_back(Point{x + 1, y});
            }

            if (y > 0 && data[y - 1][x] == CellType::EMPTY) {
                neighbor_points.emplace_back(Point{x, y - 1});
            }

            if (y < h - 1 && data[y + 1][x] == CellType::EMPTY) {
                neighbor_points.emplace_back(Point{x, y + 1});
            }

            points_map[p] = std::move(neighbor_points);
            nodes_.emplace(p, Node{p, {}});
        }
    }

    // Создать связи между нодами
    for (const auto& pair : points_map) {
        const Point& p = pair.first;
        const std::vector<Point>& neighbor_points = pair.second;

        Node& node = nodes_.at(p);
        for (const Point& neighbor_point : neighbor_points) {
            node.neighbor_nodes.push_back(&nodes_.at(neighbor_point));
        }
    }
}

std::optional<Path> CellGraph::FindPath(Point a, Point b) const {
    // BFS гарантированно находит кратчайший путь в невзвешенном графе,
    // поскольку он обходит вершины уровнями, начиная от начальной вершины.

    // Алгоритм останавливается, как только достигает целевой вершины,
    // что позволяет избежать необходимости хранения всех возможных путей

    std::queue<std::vector<const Node*>> paths_queue; // очередь путей
    std::unordered_set<const Node*> visited; // посещённые узлы

    // Создать путь, содержащий только начальную точку
    const Node* a_node = &nodes_.at(a);
    paths_queue.emplace(std::vector<const Node*>{a_node});
    visited.insert(a_node);

    while (!paths_queue.empty()) {
        // Получить текущий путь
        std::vector<const Node*> current_path = std::move(paths_queue.front());
        paths_queue.pop();

        // Получить последнюю точку в текущем пути
        const Node* current_node = current_path.back();

        if (current_node->point == b) { // если достигли конца
            Path result;
            result.reserve(current_path.size());
            for (const Node* node : current_path) {
                result.push_back(node->point);
            }
            return result;
        }

        // Для всех соседей создать новые пути
        for (const Node* neighbor : current_node->neighbor_nodes) {
            if (visited.count(neighbor) == 0) {
                visited.insert(neighbor); // отметить как посещённый
                std::vector<const Node*> new_path = current_path;
                new_path.push_back(neighbor);
                paths_queue.push(std::move(new_path)); // положить в очередь путей
            }
        }
    }

    return std::nullopt; // путь не найден
}

Field::Field(size_t w, size_t h, int probability_of_wall)
: w_(w)
, h_(h) {
    assert(probability_of_wall >= 0 && probability_of_wall <= 100);

    // Использовать время в качестве seed, так как std::random_device не всегда доступен
    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine e(seed);
    std::uniform_int_distribution<int> dist(0, 99);

    data_.resize(h);
    for (auto& row: data_) {
        row.resize(w);
        for (size_t col = 0; col < w; ++col) {
            row[col] = (dist(e) < probability_of_wall) ? CellType::WALL : CellType::EMPTY;
        }
    }

    cell_graph_.Init(data_);
}

std::optional<Path> Field::FindPath(Point a, Point b) const {
    PointIsOnFieldAssert(a);
    PointIsOnFieldAssert(b);
    assert(a != b);
    assert(data_[a.y][a.x] == CellType::EMPTY);
    assert(data_[b.y][b.x] == CellType::EMPTY);

    return cell_graph_.FindPath(a, b);
}

CellType Field::GetCellType(Point pos) const {
    PointIsOnFieldAssert(pos);
    return data_[pos.y][pos.x];
}

size_t Field::GetWidth() const {
    return w_;
}

size_t Field::GetHeight() const {
    return h_;
}

void Field::PointIsOnFieldAssert(Point pos) const {
    size_t x = static_cast<size_t>(pos.x);
    size_t y = static_cast<size_t>(pos.y);
    assert(x >= 0 && y >= 0 && x < w_ && y < h_);
}

}
