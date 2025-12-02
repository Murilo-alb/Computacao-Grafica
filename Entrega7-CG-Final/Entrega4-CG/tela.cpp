#include "tela.h"
#include <QPainter>
#include <QDebug>
#include <algorithm>

// Auxiliares
extern Matriz4x4 criarMatrizTranslacao(float, float, float);
extern Matriz4x4 criarMatrizRotacaoX(float);
extern Matriz4x4 criarMatrizRotacaoY(float);
extern Matriz4x4 criarMatrizRotacaoZ(float);
extern Matriz4x4 criarMatrizEscala(float, float, float);

Matriz4x4 criarMatrizOrtogonal(float l, float r, float b, float t, float n, float f) {
    Matriz4x4 M;
    M.m[0][0]=2.0f/(r-l); M.m[1][1]=2.0f/(t-b);
    M.m[2][2]=-2.0f/(f-n);
    M.m[3][0]=-(r+l)/(r-l);
    M.m[3][1]=-(t+b)/(t-b);
    M.m[3][2]=-(f+n)/(f-n);
    M.m[3][3]=1.0f;
    return M;
}

Matriz4x4 criarMatrizPerspectiva(float l, float r, float b, float t, float n, float f) {
    Matriz4x4 M;
    M.m[0][0]=(2.0f*n)/(r-l);
    M.m[1][1]=(2.0f*n)/(t-b);
    M.m[2][2]=-(f+n)/(f-n); M.m[2][3]=-1.0f;
    M.m[3][2]=-(2.0f*f*n)/(f-n);
    M.m[3][3]=0.0f;
    return M;
}

const int VIEWPORT_MARGIN = 20;

Tela::Tela(QWidget *parent) : QFrame(parent){
    setMouseTracking(true);
    setFrameStyle(QFrame::Box | QFrame::Sunken);
    m_chao = new Chao(2000.0f, 0.0f);
}

void Tela::DisplayFile(QList<ObjetoGeometrico*> *file){
    this->displayFile = file;
}
void Tela::setObjetoAtual(ObjetoGeometrico *obj){
    this->objetoAtual = obj;
}

void Tela::setVisualizacao(const Ponto4D& c, double z, double rx, double ry, double rz, ProjecaoTipo t, double zn){
    m_wc=c; m_zoom=z;
    m_rotX=rx;
    m_rotY=ry;
    m_rotZ=rz;
    m_projecao=t;
    m_distanciaProjecao=zn;
    update();
}

void Tela::paintEvent(QPaintEvent *event){
    QFrame::paintEvent(event);
    QPainter pintar(this);
    pintar.setRenderHint(QPainter::Antialiasing);

    double vx_min = VIEWPORT_MARGIN;
    double vy_min = VIEWPORT_MARGIN;
    double vx_max = width() - VIEWPORT_MARGIN;
    double vy_max = height() - VIEWPORT_MARGIN;
    QRectF limitesViewport(vx_min, vy_min, vx_max-vx_min, vy_max-vy_min);

    pintar.setPen(QPen(Qt::gray, 1));
    pintar.drawRect(limitesViewport);
    pintar.setClipRect(limitesViewport);

    if (vx_max <= vx_min || vy_max <= vy_min)
        return;

    double aspect = (vx_max - vx_min) / (vy_max - vy_min);
    double w = m_zoom * aspect;
    double h = m_zoom;
    if (w == 0 || h == 0)
    return;

    //Matriz View
    Matriz4x4 mView;
    mView.multiplicar(criarMatrizRotacaoZ(-m_rotZ));
    mView.multiplicar(criarMatrizRotacaoY(-m_rotY));
    mView.multiplicar(criarMatrizRotacaoX(-m_rotX));
    mView.multiplicar(criarMatrizTranslacao(-m_wc.x, -m_wc.y, -m_wc.z));

    //Matriz Projection
    Matriz4x4 mProj;
    if (m_projecao == PERSPECTIVA) mProj = criarMatrizPerspectiva(-w/2.0, w/2.0, -h/2.0, h/2.0, m_distanciaProjecao, m_distanciaProjecao+5000);
    else mProj = criarMatrizOrtogonal(-w/2.0, w/2.0, -h/2.0, h/2.0, -5000, 5000);

    //Matriz Final (MVP) para desenho
    Matriz4x4 mFinal = mView;
    mFinal.multiplicar(mProj);

    //Salva a Inversa da MVP (Sem Viewport) para o Mouse
    m_matrizInversaTotal = mFinal.inverter();

    //RENDERIZAÇÃO
    Malha3D::s_posCamera = m_wc;
    Malha3D::s_luz.posicao = Ponto4D(300, 800, 300);

    //Chão
    if(m_chao) m_chao->desenhar(pintar, mFinal, limitesViewport);

    //Objetos Existentes (Ordenados)
    if (displayFile && !displayFile->isEmpty()) {
        QList<ObjetoGeometrico*> objetosOrdenados = *displayFile;
        std::sort(objetosOrdenados.begin(), objetosOrdenados.end(), [&](ObjetoGeometrico* a, ObjetoGeometrico* b){
            Ponto4D cA = mView.transformarPonto(a->getTransform().transformarPonto(a->getCentroGeometrico()));
            Ponto4D cB = mView.transformarPonto(b->getTransform().transformarPonto(b->getCentroGeometrico()));
            float distA = cA.x*cA.x + cA.y*cA.y + cA.z*cA.z;
            float distB = cB.x*cB.x + cB.y*cB.y + cB.z*cB.z;
            return distA > distB;
        });
        for (auto obj : objetosOrdenados) obj->desenhar(pintar, mFinal, limitesViewport);
    }

    //Objeto em Construção
    if (objetoAtual) objetoAtual->desenhar(pintar, mFinal, limitesViewport);
}

void Tela::mousePressEvent(QMouseEvent *event)
{
    //GIRA CÂMERA
    if (event->button() == Qt::RightButton) {
        m_rotacionando = true;
        m_posMouseAnterior = event->pos();
        setCursor(Qt::ClosedHandCursor);
        return;
    }

    //DESENHA NO CHÃO
    if (event->button() == Qt::LeftButton && objetoAtual){
        double vx_min = VIEWPORT_MARGIN;
        double vx_max = width() - VIEWPORT_MARGIN;
        double vy_min = VIEWPORT_MARGIN;
        double vy_max = height() - VIEWPORT_MARGIN;

        double mx = event->pos().x();
        double my = event->pos().y();

        //Converter Pixel do Mouse para NDC (Normalized Device Coordinates: -1 a 1)
        //Isso deve ser EXATAMENTE o inverso do que clipToScreen faz no Objeto.cpp
        double ndc_x = (2.0 * (mx - vx_min) / (vx_max - vx_min)) - 1.0;
        double ndc_y = 1.0 - (2.0 * (my - vy_min) / (vy_max - vy_min)); // Inverte Y pois tela cresce pra baixo

        //Criar o Raio no Espaço de Clipe
        //Ponto no plano Near (z=-1) e Far (z=1)
        Ponto4D rayStart_NDC(ndc_x, ndc_y, -1.0, 1.0);
        Ponto4D rayEnd_NDC(ndc_x, ndc_y, 1.0, 1.0);

        //Transformar para Espaço do Mundo usando a Inversa da (Proj * View)
        Ponto4D rayStart_World = m_matrizInversaTotal.transformarPonto(rayStart_NDC);
        Ponto4D rayEnd_World = m_matrizInversaTotal.transformarPonto(rayEnd_NDC);

        //Divisão Perspectiva (Des-homogenização)
        if(fabs(rayStart_World.w) > 1e-6) {
            rayStart_World.x /= rayStart_World.w;
            rayStart_World.y /= rayStart_World.w;
            rayStart_World.z /= rayStart_World.w;
        }
        if(fabs(rayEnd_World.w) > 1e-6) {
            rayEnd_World.x /= rayEnd_World.w;
            rayEnd_World.y /= rayEnd_World.w;
            rayEnd_World.z /= rayEnd_World.w;
        }

        //Interseção do Raio com o Plano Y=0 (Chão)
        //P = Start + t * (End - Start)
        Ponto4D dir = rayEnd_World - rayStart_World;

        if (fabs(dir.y) > 1e-6) { //Evita divisão por zero se raio for paralelo ao chão
            float t = -rayStart_World.y / dir.y;

            //Ponto de contato no chão
            Ponto4D hitPoint = rayStart_World + dir * t;

            //Converte para o espaço local do objeto (para suportar se ele já tiver translação)
            Matriz4x4 mObjInv = objetoAtual->getTransform().inverter();
            Ponto4D pLocal = mObjInv.transformarPonto(hitPoint);

            objetoAtual->addPonto(pLocal);
            emit objetoModificado();
            update();
        }
    }
}

void Tela::mouseMoveEvent(QMouseEvent *event) {
    if (m_rotacionando) {
        int dx = event->x() - m_posMouseAnterior.x(); //Dnv esses alertas... Nao sei pq!
        int dy = event->y() - m_posMouseAnterior.y();
        double speed = 0.5;
        m_rotY += dx * speed;
        m_rotX += dy * speed;
        m_posMouseAnterior = event->pos();
        emit cameraRotacionada(m_rotX, m_rotY, m_rotZ);
        update();
    }
}

void Tela::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        m_rotacionando = false;
        setCursor(Qt::ArrowCursor);
    }
}
