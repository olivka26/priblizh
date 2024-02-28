#include "class.h"
#include <fstream>
#include <iostream>
#include <math.h>
#include <sstream>
#include <vector>

#ifndef SCENE3D_H
#define SCENE3D_H

#define DEFAULT_A -1
#define DEFAULT_B 1
#define DEFAULT_C -1
#define DEFAULT_D 1
#define DEFAULT_N 4

#define N_ 50

using namespace std;

#include <QGLWidget> // подключаем класс QGLWidget

class Scene3D
    : public QGLWidget // класс Scene3D наследует встроенный класс QGLWidget
{
    Q_OBJECT

  private:
    int n = DEFAULT_N;
    int m = DEFAULT_N;
    int p = 0;
    int mode = 0;
    int angle_count;
    QString m_name;
    double a;
    double b;
    double c;
    double d;
    double max_ = 0;
    int change = 1;
    int start = 1;
    // int chng = 1;

    double (*f)(double, double);
    int k = 0;
    QString f_name;

    GLfloat xRot; // переменная хранит угол поворота вокруг оси X
    GLfloat yRot; // переменная хранит угол поворота вокруг оси Y
    GLfloat zRot; // переменная хранит угол поворота вокруг оси Z
    GLfloat zTra; // переменная хранит величину трансляции оси Z
    GLfloat nSca; // переменная отвечает за масштабирование объекта

    QPoint ptrMousePosition; // переменная хранит координату указателя мыши в
                             // момент нажатия
    void scale_plus(); // приблизить сцену
    void scale_minus(); // удалиться от сцены
    void rotate_up(); // повернуть сцену вверх
    void rotate_down(); // повернуть сцену вниз
    void rotate_left(); // повернуть сцену влево
    void rotate_right(); // повернуть сцену вправо
    void translate_down(); // транслировать сцену вниз
    void translate_up(); // транслировать сцену вверх
    void defaultScene(); // наблюдение сцены по умолчанию

    void drawAxis(); // построить оси координат

    void getFVertexArray();
    void getVertexArray(); // определить массив вершин
    void drawFigure(); // построить фигуру

  protected:
    /*virtual*/ void
    initializeGL(); // метод для проведения инициализаций, связанных с OpenGL
    /*virtual*/ void resizeGL(
        int nWidth,
        int nHeight); // метод вызывается при изменении размеров окна виджета
    /*virtual*/ void
    paintGL(); // метод, чтобы заново перерисовать содержимое виджета

    /*virtual*/ void mousePressEvent(
        QMouseEvent
            *pe); // методы обработки события мыши при нажатии клавиши мыши
    /*virtual*/ void mouseMoveEvent(
        QMouseEvent *pe); // методы обработки события мыши при перемещении мыши
    // /*virtual*/ void mouseReleaseEvent(
    //     QMouseEvent
    /*virtual*/ void
    wheelEvent(QWheelEvent *pe); // метод обработки событий колесика мыши
    /*virtual*/ void keyPressEvent(
        QKeyEvent
            *pe); // методы обработки события при нажатии определенной клавиши

  public:
    Scene3D(QWidget *parent = 0); // конструктор класса
    Scene3D(int n_, int m_, double a, double b, double c, double d,
            QWidget *parent = 0);
    vector <double> aaa;
  public slots:
    void change_func();
    void increase_m();
    void decrease_m();
    void increase_angle();
    void reduce_angle();
    void change_mode();
    void increase_n();
    void decrease_n();
    void increase_p();
    void decrease_p();
};

class Approx
{

  public:
    Approx(int n_, int m_, double f(double, double), double a_, double b_,
           double c_, double d_);
    int n;
    int m;
    double (*f)(double, double);
    double a;
    double b;
    double c;
    double d;
    vector<double> x;
    vector<double> y;
    vector<vector<double>> F;
    vector<vector<double>> Fxy;
    vector<vector<double>> Fx;
    vector<vector<double>> Fy;
    vector<vector<double>> Fij;
    vector<vector<double>> Gx;
    vector<vector<double>> Gy;
    vector<vector<Polynom>> blocks;
    void input(double p, double max_);
    void make_G();
    void make_Fij(int i, int j);
    void make_F();
    void make_blocks();
    double result(double px, double py);
};

#endif
