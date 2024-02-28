#include "scene3D.h" // подключаем заголовочный файл scene3D.h
#include "class.h"
#include <QtGui> // подключаем модуль QtGui
#include <math.h> // подключаем математическую библиотеку

#define eps 1.e-6

const static float pi = 3.141593, k = pi / 180; // глобальная переменная

GLfloat VertexArray[2 * (N_ + 1) * (N_ + 1)][3]; // декларируем массив вершин
GLfloat ColorArray[2 * (N_ + 1) * (N_ + 1)]
                  [3]; // декларируем массив цветов вершин
GLuint IndexArray[2 * 2 * (N_ + 1) * (N_ + 1)][4];

GLfloat FVertexArray[(N_ + 1) * (N_ + 1)][3]; // декларируем массив вершин
GLfloat FColorArray[(N_ + 1) * (N_ + 1)]
                      [3]; // декларируем массив цветов вершин
GLuint FIndexArray[2 * (N_ + 1) * (N_ + 1)][4];

static double f_0(double x, double y)
{
    x += y;
    x -= y;
    return 1;
}

static double f_1(double x, double y)
{
    y++;
    y--;
    return x;
}

static double f_2(double x, double y)
{
    x++;
    x--;
    return y;
}

static double f_3(double x, double y)
{
    return x + y;
}

static double f_4(double x, double y)
{
    return sqrt(x * x + y * y);
}

static double f_5(double x, double y)
{
    return x * x + y * y;
}

static double f_6(double x, double y)
{
    return exp(-x * x + y * y);
}

static double f_7(double x, double y)
{
    return 1. / (25 * (x * x + y * y) + 1);
}

Scene3D::Scene3D(int n_, int m_, double a_, double b_, double c_, double d_,
                 QWidget *parent)
    : QGLWidget(parent), n(n_), m(m_), a(a_), b(b_), c(c_), d(d_), xRot(-90),
      yRot(0), zRot(0), zTra(0), nSca(1) // конструктор класса Scene3D
{
    angle_count = 0;
    change = 1;
    // chng = 1;
    change_func();
    change_mode();
}

Scene3D::Scene3D(QWidget *parent)
    : QGLWidget(parent), n(DEFAULT_N), m(DEFAULT_N), a(DEFAULT_A), b(DEFAULT_B),
      c(DEFAULT_C), d(DEFAULT_D), xRot(-90), yRot(0), zRot(0), zTra(0),
      nSca(1) // конструктор класса Scene3D
{
    angle_count = 0;
    change = 1;
    // chng = 1;
    change_func();
    change_mode();
}

/*virtual*/ void Scene3D::initializeGL() // инициализация
{
    qglClearColor(Qt::white); // цвет для очистки буфера изображения - здесь
                              // просто фон окна
    glEnable(GL_DEPTH_TEST); // устанавливает режим проверки глубины пикселей
    glShadeModel(GL_FLAT); // отключает режим сглаживания цветов
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINES);

    getVertexArray(); // определить массив вершин
    getFVertexArray();

    glEnableClientState(GL_VERTEX_ARRAY); // активизация массива вершин
    glEnableClientState(GL_COLOR_ARRAY); // активизация массива цветов вершин
}

/*virtual*/ void Scene3D::resizeGL(int nWidth, int nHeight) // окно виджета
{
    glMatrixMode(GL_PROJECTION); // устанавливает текущей проекционную матрицу
    glLoadIdentity(); // присваивает проекционной матрице единичную матрицу

    GLfloat ratio =
        (GLfloat)nHeight /
        (GLfloat)nWidth; // отношение высоты окна виджета к его ширине

    if (nWidth >= nHeight)
        glOrtho(-1.0 / ratio, 1.0 / ratio, -1.0, 1.0, -10.0,
                1.0); // параметры видимости ортогональной проекции
    else
        glOrtho(-1.0, 1.0, -1.0 * ratio, 1.0 * ratio, -10.0,
                1.0); // параметры видимости ортогональной проекции
    // поле просмотра
    glViewport(0, 0, (GLint)nWidth, (GLint)nHeight);
}

/*virtual*/ void Scene3D::paintGL() // рисование
{
    glClear(GL_COLOR_BUFFER_BIT |
            GL_DEPTH_BUFFER_BIT); // очистка буфера изображения и глубины

    glMatrixMode(GL_MODELVIEW); // устанавливает положение и ориентацию матрице
                                // моделирования
    glLoadIdentity(); // загружает единичную матрицу моделирования

    // последовательные преобразования
    glScalef(nSca, nSca, nSca); // масштабирование
    glTranslatef(0.0f, zTra, 0.0f); // трансляция
    glRotatef(xRot, 1.0f, 0.0f, 0.0f); // поворот вокруг оси X
    glRotatef(yRot, 0.0f, 1.0f, 0.0f); // поворот вокруг оси Y
    glRotatef(zRot, 0.0f, 0.0f, 1.0f); // поворот вокруг оси Z

    getVertexArray();
    getFVertexArray();
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINES);

    drawAxis(); // рисование осей координат
    drawFigure(); // нарисовать фигуру
    renderText(10, 30, f_name);
    renderText(10, 45, m_name);
    renderText(10, 60, ("max{|F_min|, |F_max|} = " + to_string(max_)).c_str());
    renderText(10, 75, ("n = " + to_string(n)).c_str());
    renderText(10, 90, ("m = " + to_string(m)).c_str());
    renderText(10, 105, ("p = " + to_string(p)).c_str());
}

/*virtual*/ void
Scene3D::mousePressEvent(QMouseEvent *pe) // нажатие клавиши мыши
{
    // при нажатии пользователем кнопки мыши переменной ptrMousePosition будет
    // присвоена координата указателя мыши
    ptrMousePosition = pe->pos();
}

/*virtual*/ void
Scene3D::mouseMoveEvent(QMouseEvent *pe) // изменение положения стрелки мыши
{
    xRot += 180 / nSca * (GLfloat)(pe->y() - ptrMousePosition.y()) /
            height(); // вычисление углов поворота
    zRot += 180 / nSca * (GLfloat)(pe->x() - ptrMousePosition.x()) / width();

    ptrMousePosition = pe->pos();

    updateGL(); // обновление изображения
}

/*virtual*/ void Scene3D::wheelEvent(QWheelEvent *pe) // вращение колёсика мыши
{
    if ((pe->delta()) > 0)
        scale_plus();
    else if ((pe->delta()) < 0)
        scale_minus();
    change = 0;
    updateGL(); // обновление изображения
}

/*virtual*/ void
Scene3D::keyPressEvent(QKeyEvent *pe) // нажатие определенной клавиши
{
    switch (pe->key()) {
    case Qt::Key_Plus:
        scale_plus(); // приблизить сцену
        break;

    case Qt::Key_Equal:
        scale_plus(); // приблизить сцену
        break;

    case Qt::Key_Minus:
        scale_minus(); // удалиться от сцены
        break;

    case Qt::Key_Up:
        rotate_up(); // повернуть сцену вверх
        break;

    case Qt::Key_Down:
        rotate_down(); // повернуть сцену вниз
        break;

    case Qt::Key_Left:
        rotate_left(); // повернуть сцену влево
        break;

    case Qt::Key_Right:
        rotate_right(); // повернуть сцену вправо
        break;

    case Qt::Key_Z:
        translate_down(); // транслировать сцену вниз
        break;

    case Qt::Key_X:
        translate_up(); // транслировать сцену вверх
        break;

    case Qt::Key_Space: // клавиша пробела
        defaultScene(); // возвращение значений по умолчанию
        break;

    case Qt::Key_Escape: // клавиша "эскейп"
        this->close(); // завершает приложение
        break;
    }
    change = 0;
    updateGL(); // обновление изображения
}

void Scene3D::scale_plus() // приблизить сцену
{
    nSca = nSca * 1.1;
    change = 0;
}

void Scene3D::scale_minus() // удалиться от сцены
{
    nSca = nSca / 1.1;
    change = 0;
}

void Scene3D::rotate_up() // повернуть сцену вверх
{
    xRot += 1.0;
    change = 0;
}

void Scene3D::rotate_down() // повернуть сцену вниз
{
    xRot -= 1.0;
    change = 0;
}

void Scene3D::rotate_left() // повернуть сцену влево
{
    zRot += 1.0;
    change = 0;
}

void Scene3D::rotate_right() // повернуть сцену вправо
{
    zRot -= 1.0;
    change = 0;
}

void Scene3D::translate_down() // транслировать сцену вниз
{
    zTra -= 0.05;
    change = 0;
}

void Scene3D::translate_up() // транслировать сцену вверх
{
    zTra += 0.05;
    change = 0;
}

void Scene3D::change_mode() // транслировать сцену вверх
{
    mode = (mode + 1) % 3;

    switch (mode) {
    case 0:
        m_name = "Method 21";
        break;
    case 1:
        m_name = "Function + method 21";
        break;
    case 2:
        m_name = "Error";
        break;
    }
    if (start == 1) {
        change = 1;
        start = 0;
    } else {
        change = 2;
    }
    updateGL();
}

void Scene3D::change_func() // транслировать сцену вверх
{
    k = (k + 1) % 8;

    switch (k) {
    case 0:
        f_name = "f(x) = 1";
        f = f_0;
        break;
    case 1:
        f_name = "f(x) = x";
        f = f_1;
        break;
    case 2:
        f_name = "f(x) = y";
        f = f_2;
        break;
    case 3:
        f_name = "f(x) = x + y";
        f = f_3;
        break;
    case 4:
        f_name = "f(x) = sqrt(x^2 + y^2)";
        f = f_4;
        break;
    case 5:
        f_name = "f(x) = x^2 + y^2";
        f = f_5;
        break;
    case 6:
        f_name = "f(x) = e^(x^2 - y^2)";
        f = f_6;
        break;
    case 7:
        f_name = "f(x) = 1 / (25 * (x^2 + y^2) + 1";
        f = f_7;
        break;
    }
    change = 1;
    updateGL();
}

void Scene3D::increase_p() // транслировать сцену вверх
{
    p += 1;
    change = 1;
    updateGL();
}

void Scene3D::decrease_p() // транслировать сцену вверх
{
    p -= 1;
    change = 1;
    updateGL();
}

void Scene3D::increase_n() // транслировать сцену вверх
{
    n *= 2;
    change = 1;
    updateGL();
}

void Scene3D::decrease_n() // транслировать сцену вверх
{
    n /= 2;
    change = 1;
    updateGL();
}

void Scene3D::increase_m() // транслировать сцену вверх
{
    m *= 2;
    change = 1;
    updateGL();
}

void Scene3D::decrease_m() // транслировать сцену вверх
{
    m /= 2;
    change = 1;
    updateGL();
}

void Scene3D::increase_angle()
{
    angle_count = (angle_count + 1) % 12;
    zRot = 30 * angle_count;
    change = 0;
    resizeGL(width(), height());
    updateGL();
}

void Scene3D::reduce_angle()
{
    angle_count--;
    if (angle_count < 0) {
        angle_count = 11;
    }
    zRot = 30 * angle_count;
    change = 0;
    resizeGL(width(), height());
    updateGL();
}

void Scene3D::defaultScene() // наблюдение сцены по умолчанию
{
    xRot = -90;
    yRot = 0;
    zRot = 0;
    zTra = 0;
    nSca = 1;
    change = 1;
}

void Scene3D::drawAxis() // построить оси координат
{
    glLineWidth(3.0f); // устанавливаю ширину линии приближённо в пикселях
    // до вызова команды ширина равна 1 пикселю по умолчанию

    glColor4f(1.00f, 0.00f, 0.00f,
              1.0f); // устанавливается цвет последующих примитивов
    // ось x красного цвета
    glBegin(GL_LINES); // построение линии
    glVertex3f(1.0f, 0.0f, 0.0f); // первая точка
    glVertex3f(-1.0f, 0.0f, 0.0f); // вторая точка
    glEnd();

    QColor halfGreen(0, 128, 0, 255);
    qglColor(halfGreen);
    glBegin(GL_LINES);
    // ось y зеленого цвета
    glVertex3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, -1.0f, 0.0f);

    glColor4f(0.00f, 0.00f, 1.00f, 1.0f);
    // ось z синего цвета
    glVertex3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, -1.0f);
    glEnd();
}

void Scene3D::getVertexArray() // определить массив вершин
{
    // std::cout << change << "\n";
    double delta_x = (b - a) / N_;
    double delta_y = (d - c) / N_;
    double x1;
    double y1;
    double func;
    double app;
    int vertex = 0;
    int ind = 0;
    if (change == 1) {
        aaa.resize(0);
        Approx approx = Approx(n, m, f, DEFAULT_A, DEFAULT_B, DEFAULT_C, DEFAULT_D);
        approx.input(p, max_);
        approx.make_G();
        approx.make_F();
        approx.make_blocks();
        for (x1 = a; x1 - b < eps; x1 += delta_x) {
            for (y1 = c; y1 - d < eps; y1 += delta_y) {
                func = f(x1, y1);
                app = approx.result(x1, y1);
                aaa.push_back(app);
                if (max_ < max(abs(func), abs(app))) {
                    max_ = max(abs(func), abs(app));
                }
            }
        }
    }
    if (change == 1 || change == 2) {
        vertex = 0;
        ind = 0;
        for (x1 = a; x1 - b < eps; x1 += delta_x) {
            for (y1 = c; y1 - d < eps; y1 += delta_y) {
                VertexArray[vertex][0] = x1;
                VertexArray[vertex][1] = y1;
                if (mode == 2) {
                    VertexArray[vertex][2] = f(x1, y1) - aaa[vertex];
                } else {
                    VertexArray[vertex][2] = aaa[vertex];
                }
                ColorArray[vertex][0] = 1.f * (255. / 255.);
                ColorArray[vertex][1] = 1.f * (0. / 255.);
                ColorArray[vertex][2] = 1.f * (255. / 255.);
                if (abs(x1 - a) > eps && abs(y1 - c) > eps) {
                    IndexArray[ind][0] = vertex;
                    IndexArray[ind + 1][0] = vertex;
                    IndexArray[ind][1] = vertex - N_ - 1;
                    IndexArray[ind + 1][1] = vertex - 1;
                    IndexArray[ind][2] = vertex - N_ - 2;
                    IndexArray[ind + 1][2] = vertex - N_ - 2;
                    IndexArray[ind][3] = vertex;
                    IndexArray[ind + 1][3] = vertex;
                    ind += 2;
                }
                vertex++;
            }
        }
    }
}

void Scene3D::getFVertexArray() // определить массив вершин
{
    if (change == 1 || change == 2) {
        // chng = 0;
        double delta_x = (b - a) / N_;
        double delta_y = (d - c) / N_;
        int vertex = 0;
        int ind = 0;
        double x1;
        double y1;
        double func;
        for (x1 = a; x1 - b < eps; x1 += delta_x) {
            for (y1 = c; y1 - d < eps; y1 += delta_y) {
                func = f(x1, y1);
                if (max_ < abs(func)) {
                    max_ = abs(func);
                }
            }
        }
        for (x1 = a; x1 - b < eps; x1 += delta_x) {
            for (y1 = c; y1 - d < eps; y1 += delta_y) {
                FVertexArray[vertex][0] = x1;
                FVertexArray[vertex][1] = y1;
                FVertexArray[vertex][2] = f(x1, y1);
                FColorArray[vertex][0] = 1.f * (0. / 255.);
                FColorArray[vertex][1] = 1.f * (255. / 255.);
                FColorArray[vertex][2] = 1.f * (0. / 255.);
                if (abs(x1 - a) > eps && abs(y1 - c) > eps) {
                    FIndexArray[ind][0] = vertex;
                    FIndexArray[ind + 1][0] = vertex;
                    FIndexArray[ind][1] = vertex - N_ - 1;
                    FIndexArray[ind + 1][1] = vertex - 1;
                    FIndexArray[ind][2] = vertex - N_ - 2;
                    FIndexArray[ind + 1][2] = vertex - N_ - 2;
                    FIndexArray[ind][3] = vertex;
                    FIndexArray[ind + 1][3] = vertex;
                    ind += 2;
                }
                vertex++;
            }
        }
    }
}

vector<vector<double>> A_inverse(double h)
{
    vector<vector<double>> A(4);
    int i;
    for (i = 0; i < 4; ++i) {
        A[i].resize(4, 0);
    }
    A[0][0] = 1;
    A[1][1] = 1;
    A[2][0] = (-3.) / (h * h * h);
    A[2][1] = (-2.) / (h * h);
    A[2][2] = (3.) / (h * h * h);
    A[2][3] = (-1.) / (h * h);
    A[3][0] = (2.) / (h * h * h);
    A[3][1] = (1.) / (h * h);
    A[3][2] = (-2.) / (h * h * h);
    A[3][3] = (1.) / (h * h);
    return A;
}

Polynom::Polynom(int i_, int j_, const vector<double> &x_,
                 const vector<double> &y_, const vector<vector<double>> &F_ij_)
    : i(i_), j(j_), x(x_), y(y_), F_ij(F_ij_)
{
    G.resize(4);
    vector<vector<double>> A_x = A_inverse(x[1] - x[0]);
    vector<vector<double>> A_y = A_inverse(y[1] - y[0]);
    vector<vector<double>> tmp(4);
    int str;
    int clmn;
    int el;
    for (str = 0; str < 4; str++) {
        tmp[str].resize(4);
        for (clmn = 0; clmn < 4; clmn++) {
            tmp[str][clmn] = 0;
            for (el = 0; el < 4; el++) {
                tmp[str][clmn] += F_ij[str][el] * A_y[clmn][el];
            }
        }
    }
    for (str = 0; str < 4; str++) {
        G[str].resize(4);
        for (clmn = 0; clmn < 4; clmn++) {
            G[str][clmn] = 0;
            for (el = 0; el < 4; el++) {
                G[str][clmn] += A_x[str][el] * tmp[el][clmn];
            }
        }
    }
}

double Polynom::value(double px, double py)
{
    int i;
    double res = 0;
    int str;
    int clmn;
    double tmp_x;
    double tmp_y;
    for (str = 0; str < 4; str++) {
        for (clmn = 0; clmn < 4; clmn++) {
            tmp_x = 1;
            for (i = 0; i < str; i++) {
                tmp_x *= (px - x[0]);
            }
            tmp_y = 1;
            for (i = 0; i < clmn; i++) {
                tmp_y *= (py - y[0]);
            }
            res += G[str][clmn] * tmp_x * tmp_y;
        }
    }
    return res;
}

Approx::Approx(int n_, int m_, double f_(double, double), double a_, double b_,
               double c_, double d_)
    : n(n_), m(m_), f(f_), a(a_), b(b_), c(c_), d(d_)
{
    int i;
    int j;
    F.resize(n);
    Fy.resize(n);
    Fx.resize(n);
    Fxy.resize(n);
    for (int j = 0; j < n; j++) {
        F[j].resize(m);
        Fx[j].resize(m);
        Fy[j].resize(m);
        Fxy[j].resize(m);
    }
    Gx.resize(n);
    Gy.resize(m);
    for (i = 0; i < n; i++) {
        Gx[i].resize(n);
    }
    for (j = 0; j < m; j++) {
        Gy[j].resize(m);
    }
    Fij.resize(4);
    for (i = 0; i < 4; i++) {
        Fij[i].resize(4);
    }
}

void Approx::input(double p, double max_)
{
    double delta_x = (b - a) / (n - 1);
    double delta_y = (d - c) / (m - 1);
    int i;
    int j;
    double tmp_x;
    double tmp_y;
    double tmp_val;
    for (i = 0; i < n; i++) {
        tmp_x = a + i * delta_x;
        x.push_back(tmp_x);
    }
    for (j = 0; j < m; j++) {
        tmp_y = c + j * delta_y;
        y.push_back(tmp_y);
    }
    for (i = 0; i < n; i++) {
        tmp_x = a + i * delta_x;
        for (j = 0; j < m; j++) {
            tmp_y = c + j * delta_y;
            tmp_val = f(tmp_x, tmp_y);
            if (i == n / 2 && j == m / 2) {
                tmp_val += p * 0.1 * max_;
            }
            F[i][j] = tmp_val;
        }
    }
}

void Approx::make_G()
{
    double k;
    double l;
    int str;
    int clmn;
    double tmp_1;
    double tmp_2;
    for (str = 1; str < n - 1; str++) {
        k = (x[str + 1] - x[str]) / (x[str] - x[str - 1]);
        l = (x[str + 1] - x[str - 1]);
        for (clmn = 0; clmn < n; clmn++) {
            if (clmn == str - 1) {
                Gx[str][clmn] = -k / l;
            } else if (clmn == str) {
                Gx[str][clmn] = (k - 1. / k) / l;
            } else if (clmn == str + 1) {
                Gx[str][clmn] = 1. / (k * l);
            } else {
                Gx[str][clmn] = 0.;
            }
        }
    }
    tmp_1 = (x[1] - x[0]);
    tmp_2 = x[n - 1] - x[n - 2];
    for (clmn = 0; clmn < n; clmn++) {
        Gx[0][clmn] = 0.;
        Gx[n - 1][clmn] = 0.;
    }
    Gx[0][0] = -1. / tmp_1;
    Gx[0][1] = 1. / tmp_1;
    Gx[n - 1][n - 2] = -1. / tmp_2;
    Gx[n - 1][n - 1] = 1. / tmp_2;
    for (str = 1; str < m - 1; str++) {
        k = (y[str + 1] - y[str]) / (y[str] - y[str - 1]);
        l = (y[str + 1] - y[str - 1]);
        for (clmn = 0; clmn < m; clmn++) {
            if (clmn == str - 1) {
                Gy[str][clmn] = -k / l;
            } else if (clmn == str) {
                Gy[str][clmn] = (k - 1. / k) / l;
            } else if (clmn == str + 1) {
                Gy[str][clmn] = 1. / (k * l);
            } else {
                Gy[str][clmn] = 0.;
            }
        }
    }
    tmp_1 = y[1] - y[0];
    tmp_2 = y[m - 1] - y[m - 2];
    for (clmn = 0; clmn < m; clmn++) {
        Gy[0][clmn] = 0.;
        Gy[m - 1][clmn] = 0.;
    }
    Gy[0][0] = -1. / tmp_1;
    Gy[0][1] = 1. / tmp_1;
    Gy[m - 1][m - 2] = -1. / tmp_2;
    Gy[m - 1][m - 1] = 1. / tmp_2;
}

void Approx::make_F()
{
    int str;
    int clmn;
    int el;
    for (str = 0; str < n; str++) {
        for (clmn = 0; clmn < m; clmn++) {
            Fx[str][clmn] = 0;
            for (el = 0; el < n; el++) {
                Fx[str][clmn] += Gx[str][el] * F[el][clmn];
            }
        }
    }
    for (str = 0; str < n; str++) {
        for (clmn = 0; clmn < m; clmn++) {
            Fy[str][clmn] = 0;
            for (el = 0; el < m; el++) {
                Fy[str][clmn] += F[str][el] * Gy[clmn][el];
            }
        }
    }
    for (str = 0; str < n; str++) {
        for (clmn = 0; clmn < m; clmn++) {
            Fxy[str][clmn] = 0;
            for (el = 0; el < n; el++) {
                Fxy[str][clmn] += Gx[str][el] * Fy[el][clmn];
            }
        }
    }
}

void Approx::make_Fij(int i, int j)
{
    Fij[0][0] = F[i][j];
    Fij[0][1] = Fy[i][j];
    Fij[0][2] = F[i][j + 1];
    Fij[0][3] = Fy[i][j + 1];
    Fij[1][0] = Fx[i][j];
    Fij[1][1] = Fxy[i][j];
    Fij[1][2] = Fx[i][j + 1];
    Fij[1][3] = Fxy[i][j + 1];
    Fij[2][0] = F[i + 1][j];
    Fij[2][1] = Fy[i + 1][j];
    Fij[2][2] = F[i + 1][j + 1];
    Fij[2][3] = Fy[i + 1][j + 1];
    Fij[3][0] = Fx[i + 1][j];
    Fij[3][1] = Fxy[i + 1][j];
    Fij[3][2] = Fx[i + 1][j + 1];
    Fij[3][3] = Fxy[i + 1][j + 1];
}

void Approx::make_blocks()
{
    blocks.resize(n - 1);
    int str;
    int clmn;
    for (str = 0; str < n - 1; str++) {
        for (clmn = 0; clmn < m - 1; clmn++) {
            make_Fij(str, clmn);
            blocks[str].push_back(
                Polynom(str, clmn, vector<double>{x[str], x[str + 1]},
                        vector<double>{y[clmn], y[clmn + 1]}, Fij));
        }
    }
}

double Approx::result(double px, double py)
{
    int cur_n = 0;
    int cur_m = 0;
    int i;
    int j;
    if (px - x[0] < -eps || px - x[n - 1] > eps || py - y[0] < -eps || py - y[m - 1] > eps) {
        return 0;
    }
    for (i = 0; i < n - 1; i++) {
        if (px < x[i + 1] + eps) {
            cur_n = i;
            break;
        }
    }
    for (j = 0; j < m - 1; j++) {
        if (py < y[j + 1] + eps) {
            cur_m = j;
            break;
        }
    }
    return blocks[cur_n][cur_m].value(px, py);
}

void Scene3D::drawFigure() // построить фигуру
{

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINES);
    glVertexPointer(
        3, GL_FLOAT, 0,
        VertexArray); // указываем, откуда нужно извлечь данные о массиве вершин
    glColorPointer(3, GL_FLOAT, 0, ColorArray);
    glDrawElements(GL_LINES, 8 * (N_ + 1) * (N_ + 1), GL_UNSIGNED_INT,
                   IndexArray); // строим поверхности

    if (mode == 1) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINES);
        glVertexPointer(3, GL_FLOAT, 0,
                        FVertexArray); // указываем, откуда нужно извлечь
                                          // данные о массиве вершин
        glColorPointer(3, GL_FLOAT, 0, FColorArray);
        glDrawElements(GL_LINES, 8 * (N_ + 1) * (N_ + 1), GL_UNSIGNED_INT,
                       FIndexArray);
    }
}
