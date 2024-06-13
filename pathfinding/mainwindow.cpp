#include "mainwindow.h"

#include <QGraphicsView>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollBar>
#include <QSettings>
#include <QWheelEvent>

// Производный класс от QGraphicsView,
// запрещающий скроллинг поля колесом мыши
class GraphicsView : public QGraphicsView {
    using  QGraphicsView::QGraphicsView;
    void wheelEvent(QWheelEvent *event) override {
        int horizontalValue = horizontalScrollBar()->value();
        int verticalValue = verticalScrollBar()->value();
        QGraphicsView::wheelEvent(event);
        horizontalScrollBar()->setValue(horizontalValue);
        verticalScrollBar()->setValue(verticalValue);
    }
};

MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent) {
    // Создать сцену
    gv = new GraphicsView(this);
    gv->setFocusPolicy(Qt::NoFocus);
    scene = new FieldScene;
    gv->setScene(scene);
    scene->RegenerateField(QSize(15, 15));

    // Создать поля ввода ширины и высоты
    QLabel* width_label = new QLabel("Ширина: ", this);
    width_label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    width_spin = new QSpinBox(this);
    width_spin->setRange(1, 1000);
    width_spin->setValue(15);

    QLabel* height_label = new QLabel("Высота: ", this);
    height_label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    height_spin = new QSpinBox(this);
    height_spin->setRange(1, 1000);
    height_spin->setValue(15);

    // Создать кнопку
    button = new QPushButton("Генерировать", this);
    connect(button, &QPushButton::clicked, this, &MainWindow::GenerateClicked);

    // Создать разметку
    QHBoxLayout* input_layout = new QHBoxLayout;
    input_layout->addWidget(width_label);
    input_layout->addWidget(width_spin);
    input_layout->addWidget(height_label);
    input_layout->addWidget(height_spin);
    input_layout->addWidget(button);
    input_layout->addStretch();

    QVBoxLayout* main_layout = new QVBoxLayout;
    main_layout->addWidget(gv);
    main_layout->addLayout(input_layout);

    // Создать центральный виджет
    QWidget *central_widget = new QWidget(this);
    central_widget->setLayout(main_layout);
    setCentralWidget(central_widget);

    ReadSettings();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    WriteSettings();
    QMainWindow::closeEvent(event);
}

void MainWindow::GenerateClicked() {
    int width = width_spin->value();
    int height = height_spin->value();
    scene->RegenerateField(QSize(width, height));
}

void MainWindow::ReadSettings() {
    QSettings settings("settings.ini", QSettings::IniFormat);
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    move(pos);
}

void MainWindow::WriteSettings() {
    QSettings settings("settings.ini", QSettings::IniFormat);
    settings.setValue("pos", pos());
}
