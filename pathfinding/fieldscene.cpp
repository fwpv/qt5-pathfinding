#include "fieldscene.h"

#include <QMessageBox>

FieldScene::FieldScene(QGraphicsScene *parent)
: QGraphicsScene(parent)
, field_(nullptr) {
}

void FieldScene::RegenerateField(QSize size) {
    field_ = std::make_unique<pf::Field>(static_cast<size_t>(size.width()),
        static_cast<size_t>(size.height()));

    a_point_.reset();
    b_point_.reset();
    path_.reset();
    DrawField();
}

void FieldScene::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    QPointF pos = event->scenePos();

    if (!sceneRect().contains(pos)) {
        return; // Клик вне поля это не ошибка
    }

    QPoint point{static_cast<int>(pos.x() / cell_size_),
        static_cast<int>(pos.y() / cell_size_)};

    if (field_->GetCellType(pf::Point{point.x(), point.y()}) == pf::CellType::WALL) {
        QMessageBox messageBox;
        messageBox.warning(0, "Предупреждение!",
            "В качестве начала или конца пути нельзя выбрать стену!");
        return;
    }

    if (a_point_ && point == a_point_) {
        ClearAPoint();
        DrawPath(Qt::white);
    } else if (b_point_ && point == b_point_) {
        ClearBPoint();
        DrawPath(Qt::white);
    } else if (!a_point_) {
        a_point_ = point;
        SetCellColor(point, Qt::green);
    } else if (!b_point_) {
        b_point_ = point;
        SetCellColor(point, Qt::red);
    } else {
        ClearPoints();
        DrawPath(Qt::white);
        path_.reset();
    }

    if (a_point_ && b_point_) {
        FindPath();
        DrawPath(Qt::blue);
    }
}

void FieldScene::wheelEvent(QGraphicsSceneWheelEvent *event) {
    const qreal factor = 1.12;
    if (event->delta() > 0) {
        cell_size_ *= factor;
    } else {
        cell_size_ /= factor;
    }
    DrawField();
    DrawPath(Qt::blue);

    if (a_point_) {
        SetCellColor(*a_point_, Qt::green);
    }
    if (b_point_) {
        SetCellColor(*b_point_, Qt::red);
    }
}

void FieldScene::FindPath() {
    pf::Point a = {a_point_->x(), a_point_->y()};
    pf::Point b = {b_point_->x(), b_point_->y()};
    path_ = field_->FindPath(a, b);
}

void FieldScene::DrawPath(const QColor& color) {
   if (path_) {
        pf::Path path_vec = *path_;
        for (size_t i = 1; i < path_vec.size() - 1; ++i) {
            const auto& p = path_vec[i];
            SetCellColor(QPoint{p.x, p.y}, color);
        }
    }
}

void FieldScene::DrawField() {
    clear();
    rects_.clear();
    int w = static_cast<int>(field_->GetWidth());
    int h = static_cast<int>(field_->GetHeight());

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            pf::CellType cell_type = field_->GetCellType({x, y});
            QGraphicsRectItem* rect = addRect(x * cell_size_, y * cell_size_,
                cell_size_, cell_size_, QPen(Qt::black));
            if (cell_type == pf::CellType::WALL) {
                rect->setBrush(Qt::black);
            } else {
                rect->setBrush(Qt::white);
            }
            rects_.emplace_back(rect);
        }
    }

    // Обновить границы поля
    QRectF sceneBounds = itemsBoundingRect();
    setSceneRect(sceneBounds);
}

void FieldScene::SetCellColor(const QPoint& point, const QColor& color) {
    size_t index = point.y() * field_->GetWidth() + point.x();
    if (index < rects_.size()) {
        rects_[index]->setBrush(color);
    }
}

void FieldScene::ClearAPoint() {
    if (a_point_) {
        SetCellColor(*a_point_, Qt::white);
    }
    a_point_.reset();
}

void FieldScene::ClearBPoint() {
    if (b_point_) {
        SetCellColor(*b_point_, Qt::white);
    }
    b_point_.reset();
}
void FieldScene::ClearPoints() {
   ClearAPoint();
   ClearBPoint();
}
