#ifndef FIELD_H
#define FIELD_H

#include <optional>
#include <unordered_map>
#include <vector>

namespace pf {

enum class CellType {
    EMPTY = 0,
    WALL = 1
};

struct Point {
    int x = 0;
    int y = 0;

    bool operator==(const Point& other) const;
    bool operator!=(const Point& other) const;

    struct Hasher {
        std::size_t operator()(const Point& p) const;
    };
};

using Path = std::vector<Point>;

class CellGraph {
public:
    struct Node {
        Point point;
        std::vector<Node*> neighbor_nodes;
    };

    void Init(const std::vector<std::vector<CellType>>& data);
    std::optional<Path> FindPath(Point a, Point b) const;

private:
    std::unordered_map<Point, Node, Point::Hasher> nodes_;
};

class Field {
public:
    Field(size_t w, size_t h, int probability_of_wall = 20);

    // Найти все точки пути, включая начальную и конечную
    std::optional<Path> FindPath(Point a, Point b) const;
    CellType GetCellType(Point pos) const;

    size_t GetWidth() const;
    size_t GetHeight() const;

private:
    void PointIsOnFieldAssert(Point pos) const;

    std::vector<std::vector<CellType>> data_;
    size_t w_;
    size_t h_;
    CellGraph cell_graph_;
};

}

#endif // FIELD_H
