#include "scene3D.h"
#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>
#include <QPainter>

int main(int argc, char **argv)
{
    QApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
    QApplication app(argc, argv);
    QMainWindow *window = new QMainWindow;
    QMenuBar *tool_bar = new QMenuBar(window);
    Scene3D *scene1 = new Scene3D(window);
    QAction *action;
    action =
        tool_bar->addAction("&Change function", scene1, SLOT(change_func()));
    action->setShortcut(QString("0"));
    action = tool_bar->addAction("&Change method", scene1, SLOT(change_mode()));
    action->setShortcut(QString("1"));
    action = tool_bar->addAction("&Increase m", scene1, SLOT(increase_m()));
    action->setShortcut(QString("2"));
    action = tool_bar->addAction("&Decrease m", scene1, SLOT(decrease_m()));
    action->setShortcut(QString("3"));
    action = tool_bar->addAction("&Increase n", scene1, SLOT(increase_n()));
    action->setShortcut(QString("4"));
    action = tool_bar->addAction("&Decrease n", scene1, SLOT(decrease_n()));
    action->setShortcut(QString("5"));
    action = tool_bar->addAction("&Increase p", scene1, SLOT(increase_p()));
    action->setShortcut(QString("6"));
    action = tool_bar->addAction("&Decrease p", scene1, SLOT(decrease_p()));
    action->setShortcut(QString("7"));
    action =
        tool_bar->addAction("&Increase angle", scene1, SLOT(increase_angle()));
    action->setShortcut(QString("8"));
    action =
        tool_bar->addAction("&Decrease angle", scene1, SLOT(reduce_angle()));
    action->setShortcut(QString("9"));
    tool_bar->setMaximumHeight(30);
    window->setMenuBar(tool_bar);
    window->setCentralWidget(scene1);
    window->resize(500, 500);
    window->setWindowTitle("Graph");
    window->show();
    app.exec();
    delete scene1;
    delete window;
    return 0;

    return app.exec();
}
