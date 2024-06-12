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

    // Создаёт новое поле и отображает на экране
    void RegenerateField(QSize size);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void wheelEvent(QGraphicsSceneWheelEvent *event) override;

private:
    void FindPath();
    void DrawPath(const QColor& color);
    void DrawField();
    void SetCellColor(const QPoint& point, const QColor& color);
    void ClearAPoint();
    void ClearBPoint();
    void ClearPoints();

    std::unique_ptr<pf::Field> field_;

    std::optional<QPoint> a_point_;
    std::optional<QPoint> b_point_;

    std::vector<QGraphicsRectItem*> rects_;
    qreal cell_size_ = 20;

    std::optional<pf::Path> path_;
};

#endif // FIELDSCENE_H
