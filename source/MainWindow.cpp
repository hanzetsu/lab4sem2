#include "MainWindow.hpp"
#include "StatisticsWidget.hpp"
#include "SubstringSearchWidget.hpp"
#include <QTabWidget>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    QTabWidget* tabs = new QTabWidget(this);
    tabs->addTab(new StatisticsWidget, "Статистика потока");
    tabs->addTab(new SubstringSearchWidget, "Подсчёт подстрок");
    setCentralWidget(tabs);
    resize(800, 600);
}