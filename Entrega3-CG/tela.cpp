#include "tela.h"
#include <QPainter>
#include <QDebug> // Para usar qDebug() em caso de matriz não inversível

// Declaração antecipada das funções globais que estão em objeto.cpp
Matriz3x3 criarMatrizTranslacao(float dx, float dy);
Matriz3x3 criarMatrizRotacao(float angulo);
Matriz3x3 criarMatrizEscala(float sx, float sy);


Tela::Tela(QWidget *parent) : QFrame(parent)
{
    setMouseTracking(true);
    setFrameStyle(QFrame::Box | QFrame::Sunken);
}

void Tela::DisplayFile(QList<ObjetoGeometrico*> *file)
{
    this->displayFile = file;
}

void Tela::PoligonoAtual(Poligono *poligono)
{
    this->poligonoAtual = poligono;
}

void Tela::setVisualizacao(const QPointF& centro, double zoom, double angulo)
{
    m_wc = centro;
    m_zoom = zoom;
    m_angulo = angulo;
    update(); // Força um redesenho com os novos parâmetros
}

void Tela::paintEvent(QPaintEvent *event)
{
    QFrame::paintEvent(event);
    QPainter pintar(this);
    pintar.setRenderHint(QPainter::Antialiasing);

    if (!displayFile) {
        return;
    }

    // Define o Viewport (usando o tamanho do widget)
    double xv_min = 0, yv_min = 0;
    double xv_max = this->width(), yv_max = this->height();

    if (yv_max == 0) return;

    double window_height = m_zoom;
    double window_width = m_zoom * (xv_max / yv_max);

    if (window_width == 0 || window_height == 0) return;

    // Monta a matriz de transformação de visualização (composta)
    Matriz3x3 matrizVisualizacao; // Começa como identidade

    // A ordem das operações para que os controles X/Y da câmera se movam horizontal/verticalmente na TELA
    // 1. Rotação do mundo ao redor da origem (inverso do ângulo da câmera)
    matrizVisualizacao.multiplicar(criarMatrizRotacao(-m_angulo));

    // 2. Transladar o mundo para que o centro da window (m_wc) vá para a origem
    matrizVisualizacao.multiplicar(criarMatrizTranslacao(-m_wc.x(), -m_wc.y()));

    // 3. Escalar para mapear a window para o viewport (e inverter o Y)
    double sx = (xv_max - xv_min) / window_width;
    double sy = (yv_min - yv_max) / window_height; // Y invertido!
    matrizVisualizacao.multiplicar(criarMatrizEscala(sx, sy));

    // 4. Transladar para a posição final do viewport na tela
    double tx = (xv_max + xv_min) / 2.0;
    double ty = (yv_max + yv_min) / 2.0;
    matrizVisualizacao.multiplicar(criarMatrizTranslacao(tx, ty));

    m_matrizInversa = matrizVisualizacao.inverter();

    // Loop de desenho: cada objeto recebe a matriz e se desenha
    const QList<ObjetoGeometrico*>& lista = *displayFile;
    for (const auto& objeto : lista)
    {
        objeto->desenhar(pintar, matrizVisualizacao);
    }
}

void Tela::mousePressEvent(QMouseEvent *event)
{
    if (poligonoAtual)
    {
        QPoint coordenadaDoMouse_int = event->pos();
        QPointF coordenadaDoMouse_float(coordenadaDoMouse_int.x(), coordenadaDoMouse_int.y());

        //Usa o transformarPonto da nossa Matriz3x3 inversa
        QPointF coordenadaDoMundo = m_matrizInversa.transformarPonto(coordenadaDoMouse_float);
        poligonoAtual->addPonto(coordenadaDoMundo);
        emit objetoModificado();
        update();
    }
}
