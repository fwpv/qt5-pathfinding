#ifndef FIELDSCENE_H
#define FIELDSCENE_H

#include "field.h"

#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneWheelEvent>

#include <memory>
#include <optional>

class FieldScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit FieldScene(QGraphicsScene *parent = nullptr);
    void RegenerateField(QSize size);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void wheelEvent(QGraphicsSceneWheelEvent *event) override;

private:
    enum class PressState {
        WAITING_FOR_A,
        WAITING_FOR_B,
        SHOWING_PATH
    };

    void ProcessPress(QPoint cell_pos);

    void DrawField();
    void UpdateRects();

    void RestoreCellColor(QPoint cell_pos);
    void SetCellColor(QPoint cell_pos, const QColor& color);
    void DrawPath(const QColor& color);

    void FindPath();

    PressState state_;

    std::unique_ptr<pf::Field> field_;
    QPoint a_cell_pos_;
    QPoint b_cell_pos_;
    pf::Path path_;

    qreal cell_size_ = 20;
    std::vector<QGraphicsRectItem*> rects_;
};

#endif // FIELDSCENE_H
