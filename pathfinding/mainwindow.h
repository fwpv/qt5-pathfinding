#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "fieldscene.h"

#include <QMainWindow>
#include <QGraphicsView>
#include <QSpinBox>
#include <QPushButton>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void GenerateClicked();

private:
    QGraphicsView* gv;
    FieldScene* scene;
    QSpinBox* width_spin;
    QSpinBox* height_spin;
    QPushButton* button;
};
#endif // MAINWINDOW_H
