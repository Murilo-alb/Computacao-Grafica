#include "tela.h"
#include <QPainter>
#include <QDebug>
// #include "clipping.h" // REMOVIDO

extern Matriz4x4 criarMatrizTranslacao(float dx, float dy, float dz);
extern Matriz4x4 criarMatrizRotacaoX(float angulo);
extern Matriz4x4 criarMatrizRotacaoY(float angulo);
extern Matriz4x4 criarMatrizRotacaoZ(float angulo);
extern Matriz4x4 criarMatrizEscala(float sx, float sy, float sz);

Matriz4x4 criarMatrizOrtogonal(float l, float r, float b, float t, float n, float f) {
    Matriz4x4 M;
    M.m[0][0] = 2.0f / (r - l);
    M.m[1][1] = 2.0f / (t - b);
    M.m[2][2] = -2.0f / (f - n);
    M.m[3][0] = -(r + l) / (r - l);
    M.m[3][1] = -(t + b) / (t - b);
    M.m[3][2] = -(f + n) / (f - n);
    M.m[3][3] = 1.0f;
    return M;
}

const int VIEWPORT_MARGIN = 20;

Tela::Tela(QWidget *parent) : QFrame(parent){
    setMouseTracking(true);
    setFrameStyle(QFrame::Box | QFrame::Sunken);
}

void Tela::DisplayFile(QList<ObjetoGeometrico*> *file){
    this->displayFile = file;
}

void Tela::setObjetoAtual(ObjetoGeometrico *obj){
    this->objetoAtual = obj;
}

void Tela::setVisualizacao(const Ponto4D& centro, double zoom, double rotX, double rotY, double rotZ){
    m_wc = centro;
    m_zoom = zoom;
    m_rotX = rotX;
    m_rotY = rotY;
    m_rotZ = rotZ;
    update();
}

void Tela::paintEvent(QPaintEvent *event){
    QFrame::paintEvent(event);
    QPainter pintar(this);
    pintar.setRenderHint(QPainter::Antialiasing);

    if (!displayFile) {
        return;
    }

    double viewport_x_min = VIEWPORT_MARGIN;
    double viewport_y_min = VIEWPORT_MARGIN;
    double viewport_x_max = this->width() - VIEWPORT_MARGIN;
    double viewport_y_max = this->height() - VIEWPORT_MARGIN;

    // A Borda do Clipper 2D
    QRectF limitesViewport(viewport_x_min, viewport_y_min,
                           viewport_x_max - viewport_x_min,
                           viewport_y_max - viewport_y_min);

    pintar.setPen(QPen(Qt::gray, 1));
    pintar.drawRect(limitesViewport);

    if (viewport_x_max <= viewport_x_min || viewport_y_max <= viewport_y_min) {
        return;
    }

    double aspect = (viewport_x_max - viewport_x_min) / (viewport_y_max - viewport_y_min);
    double window_altura = m_zoom;
    double window_largura = m_zoom * aspect;

    if (window_largura == 0 || window_altura == 0)
        return;

    Matriz4x4 matrizView;
    matrizView.multiplicar(criarMatrizEscala(1.0, 1.0, -1.0));
    matrizView.multiplicar(criarMatrizRotacaoZ(-m_rotZ));
    matrizView.multiplicar(criarMatrizRotacaoY(-m_rotY));
    matrizView.multiplicar(criarMatrizRotacaoX(-m_rotX));
    matrizView.multiplicar(criarMatrizTranslacao(-m_wc.x, -m_wc.y, -m_wc.z));

    float zNear = -1000.0;
    float zFar = 1000.0;
    Matriz4x4 matrizProj = criarMatrizOrtogonal(
        -window_largura / 2.0, window_largura / 2.0,
        -window_altura / 2.0, window_altura / 2.0,
        zNear, zFar
        );
    matrizProj.multiplicar(criarMatrizEscala(1.0, -1.0, 1.0));

    Matriz4x4 matrizViewport;
    matrizViewport.multiplicar(criarMatrizEscala(
        (viewport_x_max - viewport_x_min) / 2.0, (viewport_y_max - viewport_y_min) / 2.0,1.0));
    matrizViewport.multiplicar(criarMatrizTranslacao(
        viewport_x_min + (viewport_x_max - viewport_x_min) / 2.0, viewport_y_min + (viewport_y_max - viewport_y_min) / 2.0, 0.0));

    // Criamos UMA matriz final que faz (Mundo -> Viewport)
    Matriz4x4 matrizFinal = matrizView;
    matrizFinal.multiplicar(matrizProj);
    matrizFinal.multiplicar(matrizViewport);

    m_matrizInversaTotal = matrizFinal.inverter();

    const QList<ObjetoGeometrico*>& lista = *displayFile;
    for (const auto& objeto : lista){
        // Passamos a matriz final e os limites da viewport
        objeto->desenhar(pintar, matrizFinal, limitesViewport);
    }
}

void Tela::mousePressEvent(QMouseEvent *event)
{
    if (objetoAtual){
        // Criamos o ponto no "meio" (z=0) do volume
        Ponto4D pontoTela(event->pos().x(), event->pos().y(), 0.0f, 1);
        Ponto4D coordenadaDoMundo = m_matrizInversaTotal.transformarPonto(pontoTela);

        objetoAtual->addPonto(coordenadaDoMundo.normalizado());

        emit objetoModificado();
        update();
    }
}
