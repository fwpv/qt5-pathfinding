#include "fieldscene.h"

#include <QMessageBox>

FieldScene::FieldScene(QGraphicsScene *parent)
    : QGraphicsScene(parent) {
}

void FieldScene::RegenerateField(QSize size){
    field_ = std::make_unique<pf::Field>(static_cast<size_t>(size.width()),
        static_cast<size_t>(size.height()));

    state_ = PressState::WAITING_FOR_A;
    DrawField();
    path_.clear();
}

void FieldScene::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    QPointF pos = event->scenePos();

    if (!sceneRect().contains(pos)) {
        return; // Клик вне поля это не ошибка
    }

    QPoint cell_pos{static_cast<int>(pos.x() / cell_size_),
        static_cast<int>(pos.y() / cell_size_)};

    if (field_->GetCellType(pf::Point{cell_pos.x(), cell_pos.y()}) == pf::CellType::WALL) {
        QMessageBox messageBox;
        messageBox.warning(0, "Предупреждение!",
            "В качестве начала или конца пути нельзя выбрать стену!");
        return;
    }

    ProcessPress(cell_pos);
}

void FieldScene::wheelEvent(QGraphicsSceneWheelEvent *event) {
    const qreal factor = 1.12;
    if (event->delta() > 0) {
        cell_size_ *= factor;
    } else {
        cell_size_ /= factor;
    }
    UpdateRects();
}

void FieldScene::ProcessPress(QPoint cell_pos) {
    // Переключение состояния нажатия
    if (state_ == PressState::WAITING_FOR_A) {
        a_cell_pos_ = cell_pos;
        state_ = PressState::WAITING_FOR_B;
    } else if (state_ == PressState::WAITING_FOR_B) {
        if (cell_pos == a_cell_pos_) {
            state_ = PressState::WAITING_FOR_A;
        } else {
            b_cell_pos_ = cell_pos;
            FindPath();
            state_ = PressState::SHOWING_PATH;
        }
    } else if (state_ == PressState::SHOWING_PATH) {
        if (cell_pos == b_cell_pos_) {
            state_ = PressState::WAITING_FOR_B;
        } else {
            state_ = PressState::WAITING_FOR_A;
        }
    }

    // Нарисовать точки в зависимости от состояния нажатия
    switch (state_) {
        case PressState::WAITING_FOR_A:
            RestoreCellColor(a_cell_pos_);
            RestoreCellColor(b_cell_pos_);
            DrawPath(Qt::white);
            break;
        case PressState::WAITING_FOR_B:
            RestoreCellColor(b_cell_pos_);
            SetCellColor(a_cell_pos_, Qt::green);
            DrawPath(Qt::white);
            break;
        case PressState::SHOWING_PATH:
            SetCellColor(a_cell_pos_, Qt::green);
            SetCellColor(b_cell_pos_, Qt::red);
            DrawPath(Qt::blue);
            break;
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
                cell_size_, cell_size_);
            if (cell_type == pf::CellType::WALL) {
                rect->setBrush(Qt::black);
            } else {
                rect->setBrush(Qt::white);
            }
            rects_.emplace_back(rect);
        }
    }

    QRectF sceneBounds = itemsBoundingRect();
    setSceneRect(sceneBounds);
}

void FieldScene::UpdateRects() {
    int w = static_cast<int>(field_->GetWidth());
    int h = static_cast<int>(field_->GetHeight());

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
        int index = y * w + x;
            QGraphicsRectItem* rect = rects_[index];
            if (rect) {
                rect->setRect(x * cell_size_, y * cell_size_,
                              cell_size_, cell_size_);
            }
        }
    }

    QRectF sceneBounds = itemsBoundingRect();
    setSceneRect(sceneBounds);
}

void FieldScene::RestoreCellColor(QPoint cell_pos) {
    size_t index = cell_pos.y() * field_->GetWidth() + cell_pos.x();
    if (index < rects_.size()) {
        pf::CellType cell_type = field_->GetCellType({cell_pos.x(), cell_pos.y()});
        if (cell_type == pf::CellType::WALL) {
            rects_[index]->setBrush(Qt::black);
        } else {
            rects_[index]->setBrush(Qt::white);
        }
    }
}

void FieldScene::SetCellColor(QPoint cell_pos, const QColor &color) {
    size_t index = cell_pos.y() * field_->GetWidth() + cell_pos.x();
    if (index < rects_.size()) {
        rects_[index]->setBrush(color);
    }
}

void FieldScene::DrawPath(const QColor &color) {
    if (path_.empty()) {
        return;
    }
    for (size_t i = 1; i < path_.size() - 1; ++i) {
        const auto& p = path_[i];
        SetCellColor(QPoint{p.x, p.y}, color);
    }
}

void FieldScene::FindPath(){
    pf::Point a = {a_cell_pos_.x(), a_cell_pos_.y()};
    pf::Point b = {b_cell_pos_.x(), b_cell_pos_.y()};
    auto result = field_->FindPath(a, b);
    if (result) {
        path_ = *result;
    }
}
