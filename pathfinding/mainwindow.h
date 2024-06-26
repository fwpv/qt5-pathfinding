#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "fieldscene.h"

#include <QGraphicsView>
#include <QMainWindow>
#include <QPushButton>
#include <QSpinBox>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void GenerateClicked();

private:
    void ReadSettings();
    void WriteSettings();

    QGraphicsView* gv;
    FieldScene* scene;
    QSpinBox* width_spin;
    QSpinBox* height_spin;
    QPushButton* button;
};
#endif // MAINWINDOW_H
