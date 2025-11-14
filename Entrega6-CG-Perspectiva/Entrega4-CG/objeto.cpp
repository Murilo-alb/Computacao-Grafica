#include "Objeto.h"
#include "clipping.h"
#include <cmath>

Matriz4x4 criarMatrizTranslacao(float dx, float dy, float dz) {
    Matriz4x4 T; T.m[3][0] = dx; T.m[3][1] = dy; T.m[3][2] = dz;
    return T;
}
Matriz4x4 criarMatrizEscala(float sx, float sy, float sz) {
    Matriz4x4 S; S.m[0][0] = sx; S.m[1][1] = sy; S.m[2][2] = sz;
    return S;
}
Matriz4x4 criarMatrizRotacaoX(float angulo) {
    Matriz4x4 R; float r = angulo * M_PI / 180.0, c = cos(r), s = sin(r);
    R.m[1][1] = c; R.m[1][2] = s; R.m[2][1] = -s; R.m[2][2] = c;
    return R;
}
Matriz4x4 criarMatrizRotacaoY(float angulo) {
    Matriz4x4 R; float r = angulo * M_PI / 180.0, c = cos(r), s = sin(r);
    R.m[0][0] = c; R.m[0][2] = -s; R.m[2][0] = s; R.m[2][2] = c;
    return R;
}
Matriz4x4 criarMatrizRotacaoZ(float angulo) {
    Matriz4x4 R; float r = angulo * M_PI / 180.0, c = cos(r), s = sin(r);
    R.m[0][0] = c; R.m[0][1] = s; R.m[1][0] = -s; R.m[1][1] = c;
    return R;
}

ObjetoGeometrico::ObjetoGeometrico(const QString& nome, TipoObjeto tipo) : nome(nome), tipo(tipo) {

}
void ObjetoGeometrico::addPonto(const Ponto4D& p) {
    pontos.append(p);
}
QString ObjetoGeometrico::getNome() const {
    return nome;
}
TipoObjeto ObjetoGeometrico::getTipo() const {
    return tipo;
}
const QList<Ponto4D>& ObjetoGeometrico::getPontos() const {
    return pontos;
}
void ObjetoGeometrico::selecionar(){
    this->selecionado = true;
}
void ObjetoGeometrico::deselecionar(){
    this->selecionado = false;
}
void ObjetoGeometrico::transladar(double dx, double dy, double dz){
    Matriz4x4 mT = criarMatrizTranslacao(dx, dy, dz);
    m_transform.multiplicar(mT);
    m_total_translacao = m_total_translacao + Ponto4D(dx, dy, dz, 0);
}
void ObjetoGeometrico::escalar(double sx, double sy, double sz){
    Ponto4D c = getCentroGeometrico();
    Matriz4x4 T1 = criarMatrizTranslacao(-c.x, -c.y, -c.z);
    Matriz4x4 S = criarMatrizEscala(sx, sy, sz);
    Matriz4x4 T2 = criarMatrizTranslacao(c.x, c.y, c.z);
    T1.multiplicar(S); T1.multiplicar(T2);
    m_transform.multiplicar(T1);
    m_total_escala.x *= sx; m_total_escala.y *= sy; m_total_escala.z *= sz;
}
void ObjetoGeometrico::rotacionarX(double angulo){
    Ponto4D c = getCentroGeometrico();
    Matriz4x4 T1 = criarMatrizTranslacao(-c.x, -c.y, -c.z);
    Matriz4x4 R = criarMatrizRotacaoX(angulo);
    Matriz4x4 T2 = criarMatrizTranslacao(c.x, c.y, c.z);
    T1.multiplicar(R); T1.multiplicar(T2);
    m_transform.multiplicar(T1);
    m_total_rotacao.x += angulo;
}
void ObjetoGeometrico::rotacionarY(double angulo){
    Ponto4D c = getCentroGeometrico();
    Matriz4x4 T1 = criarMatrizTranslacao(-c.x, -c.y, -c.z);
    Matriz4x4 R = criarMatrizRotacaoY(angulo);
    Matriz4x4 T2 = criarMatrizTranslacao(c.x, c.y, c.z);
    T1.multiplicar(R); T1.multiplicar(T2);
    m_transform.multiplicar(T1);
    m_total_rotacao.y += angulo;
}
void ObjetoGeometrico::rotacionarZ(double angulo){
    Ponto4D c = getCentroGeometrico();
    Matriz4x4 T1 = criarMatrizTranslacao(-c.x, -c.y, -c.z);
    Matriz4x4 R = criarMatrizRotacaoZ(angulo);
    Matriz4x4 T2 = criarMatrizTranslacao(c.x, c.y, c.z);
    T1.multiplicar(R); T1.multiplicar(T2);
    m_transform.multiplicar(T1);
    m_total_rotacao.z += angulo;
}

Ponto::Ponto(const QString& nome, const Ponto4D& p) : ObjetoGeometrico(nome, PONTO) {
    pontos.append(p);
}
void Ponto::desenhar(QPainter& pintar, const Matriz4x4& matrizFinal, const QRectF& limites) {
    if (pontos.isEmpty())
        return;

    Matriz4x4 mFinal = m_transform;
    mFinal.multiplicar(matrizFinal);
    Ponto4D pTela4D = mFinal.transformarPonto(pontos.first());

    QPointF pTela2D = pTela4D.paraQPointF();

    if (!limites.contains(pTela2D)) {
        return;
    }

    if(this->isSelecionado()){
        pintar.setPen(QPen(Qt::yellow, 7));
    }else {
        pintar.setPen(QPen(m_cor, 5));
    }
    pintar.drawEllipse(pTela2D, 5, 5);
}
Ponto4D Ponto::getCentroGeometrico() const {
    if (pontos.isEmpty()) return Ponto4D(0, 0, 0);
    return pontos.first();
}
Reta::Reta(const QString& nome, const Ponto4D& p1, const Ponto4D& p2) : ObjetoGeometrico(nome, RETA) {
    pontos.append(p1);
    pontos.append(p2);
}
void Reta::desenhar(QPainter& pintar, const Matriz4x4& matrizFinal, const QRectF& limites) {
    if (pontos.size() < 2)
        return;

    Matriz4x4 mFinal = m_transform;
    mFinal.multiplicar(matrizFinal);

    Ponto4D p1Tela4D = mFinal.transformarPonto(pontos[0]);
    Ponto4D p2Tela4D = mFinal.transformarPonto(pontos[1]);

    QPointF p1Tela2D = p1Tela4D.paraQPointF();
    QPointF p2Tela2D = p2Tela4D.paraQPointF();

    Clipping clipper(limites);
    if (clipper.clipLinha(p1Tela2D, p2Tela2D)) {
        if(this->isSelecionado()){
            pintar.setPen(QPen(Qt::yellow, 2));
        } else { pintar.setPen(QPen(m_cor, 2)); }
        pintar.drawLine(p1Tela2D, p2Tela2D);
    }
}
Ponto4D Reta::getCentroGeometrico() const {
    if (pontos.size() < 2) return Ponto4D(0, 0, 0);
    Ponto4D c = (pontos[0] + pontos[1]);
    c.x /= 2.0;
    c.y /= 2.0;
    c.z /= 2.0;
    c.w = 1.0;
    return c;
}
Poligono::Poligono(const QString& nome) : ObjetoGeometrico(nome, POLIGONO) {
    finalizado = false;
}
void Poligono::finalizar() {
    this->finalizado = true;
}
void Poligono::desenhar(QPainter& pintar, const Matriz4x4& matrizFinal, const QRectF& limites) {
    if (pontos.isEmpty())
        return;

    Matriz4x4 mFinal = m_transform;
    mFinal.multiplicar(matrizFinal);

    Clipping clipper(limites);

    if(this->isSelecionado()){
        pintar.setPen(QPen(Qt::yellow, 2));
    }else {
        pintar.setPen(QPen(m_cor, 2));
    }

    for (int i = 0; i < pontos.size(); ++i) {
        if (!finalizado && i == pontos.size() - 1)
            break;

        QPointF p1Tela2D = mFinal.transformarPonto(pontos[i]).paraQPointF();
        QPointF p2Tela2D = mFinal.transformarPonto(pontos[(i + 1) % pontos.size()]).paraQPointF();
        if (clipper.clipLinha(p1Tela2D, p2Tela2D)) {
            pintar.drawLine(p1Tela2D, p2Tela2D);
        }
    }

    if(this->isSelecionado()){
        pintar.setPen(QPen(Qt::yellow, 7));
        for(const Ponto4D& p_original : pontos){
            QPointF p_tela = mFinal.transformarPonto(p_original).paraQPointF();
            if(limites.contains(p_tela)) {
                pintar.drawPoint(p_tela);
            }
        }
    }
}
Ponto4D Poligono::getCentroGeometrico() const {
    if (pontos.isEmpty())
        return Ponto4D(0, 0, 0);
    Ponto4D s(0, 0, 0, 0);

    for (const Ponto4D& p : pontos){
    s = s + p;
    s.x /= pontos.size();
    s.y /= pontos.size();
    s.z /= pontos.size();
    s.w = 1.0;
    }
    return s;

}
CurvaBezier::CurvaBezier(const QString& nome) : ObjetoGeometrico(nome, CURVA_BEZIER) {}
Ponto4D CurvaBezier::getCentroGeometrico() const {
    if (pontos.isEmpty())
        return Ponto4D(0, 0, 0);
    Ponto4D s(0, 0, 0, 0);
    for (const Ponto4D& p : pontos){
    s = s + p;
    s.x /= pontos.size();
    s.y /= pontos.size();
    s.z /= pontos.size();
    s.w = 1.0;
    }
    return s;
}
Ponto4D CurvaBezier::calcularPontoDeCasteljau(const QList<Ponto4D>& pts, double t) const {
    if (pts.size() == 1)
        return pts.first();
    QList<Ponto4D> nPts;
    for (int i = 0; i < pts.size() - 1; ++i) nPts.append((1.0 - t) * pts[i] + t * pts[i+1]);
    return calcularPontoDeCasteljau(nPts, t);
}
void CurvaBezier::desenhar(QPainter& pintar, const Matriz4x4& matrizFinal, const QRectF& limites) {
    Matriz4x4 mFinal = m_transform;
    mFinal.multiplicar(matrizFinal);
    Clipping clipper(limites);

    if(this->isSelecionado()) {
        pintar.setPen(QPen(Qt::darkGray, 1, Qt::DashLine));
        for (int i = 0; i < pontos.size() - 1; ++i) {
            QPointF p1Tela2D = mFinal.transformarPonto(pontos[i]).paraQPointF();
            QPointF p2Tela2D = mFinal.transformarPonto(pontos[i+1]).paraQPointF();
            // CLIPPING REATIVADO
            if(clipper.clipLinha(p1Tela2D, p2Tela2D)) {
                pintar.drawLine(p1Tela2D, p2Tela2D);
            }
        }
        pintar.setPen(QPen(Qt::red, 5));
        for(const Ponto4D& p_original : pontos){
            QPointF p_tela = mFinal.transformarPonto(p_original).paraQPointF();
            if(limites.contains(p_tela)) {
                pintar.drawPoint(p_tela);
            }
        }
    }
    if (pontos.size() < 2)
        return;

    if(this->isSelecionado()){
        pintar.setPen(QPen(Qt::yellow, 2));
    }else {
        pintar.setPen(QPen(m_cor, 2));
    }

    QList<Ponto4D> ptsCurva; double passo = 1.0 / numSegmentos;
    for (int i = 0; i <= numSegmentos; ++i) ptsCurva.append(calcularPontoDeCasteljau(pontos, i * passo));

    for (int i = 0; i < ptsCurva.size() - 1; ++i) {
        QPointF p1Tela2D = mFinal.transformarPonto(ptsCurva[i]).paraQPointF();
        QPointF p2Tela2D = mFinal.transformarPonto(ptsCurva[i+1]).paraQPointF();
        if(clipper.clipLinha(p1Tela2D, p2Tela2D)) {
            pintar.drawLine(p1Tela2D, p2Tela2D);
        }
    }
}

Malha3D::Malha3D(const QString& nome) : ObjetoGeometrico(nome, MALHA_3D){

}
void Malha3D::addFace(int v1, int v2, int v3){
    m_faces.append(Face{v1, v2, v3});
}
Ponto4D Malha3D::getCentroGeometrico() const {
    if (pontos.isEmpty())
        return Ponto4D(0, 0, 0);
    Ponto4D s(0, 0, 0, 0);
    for (const Ponto4D& p : pontos){
    s = s + p;
    s.x /= pontos.size();
    s.y /= pontos.size();
    s.z /= pontos.size();
    s.w = 1.0;
    }
    return s;
}

void Malha3D::desenhar(QPainter& pintar, const Matriz4x4& matrizFinal, const QRectF& limites) {
    if (m_faces.isEmpty())
        return;

    Matriz4x4 mFinal = m_transform;
    mFinal.multiplicar(matrizFinal);
    Clipping clipper(limites);


    if(this->isSelecionado()){
        pintar.setPen(QPen(Qt::yellow, 1));
    } else {
        pintar.setPen(QPen(m_cor, 1));
    }

    for (const Face& face : m_faces) {
        Ponto4D p1_original = pontos[face.v1];
        Ponto4D p2_original = pontos[face.v2];
        Ponto4D p3_original = pontos[face.v3];

        Ponto4D p1_tela_bfc = mFinal.transformarPonto(p1_original);
        Ponto4D p2_tela_bfc = mFinal.transformarPonto(p2_original);
        Ponto4D p3_tela_bfc = mFinal.transformarPonto(p3_original);
        QPointF p1_qpf = p1_tela_bfc.paraQPointF();
        QPointF p2_qpf = p2_tela_bfc.paraQPointF();
        QPointF p3_qpf = p3_tela_bfc.paraQPointF();

        float v1x = p2_qpf.x() - p1_qpf.x();
        float v1y = p2_qpf.y() - p1_qpf.y();
        float v2x = p3_qpf.x() - p1_qpf.x();
        float v2y = p3_qpf.y() - p1_qpf.y();
        float z_component = (v1x * v2y) - (v1y * v2x);

        if (z_component < 0) {
            continue;
        }

        QPointF p1Tela2D = p1_qpf;
        QPointF p2Tela2D = p2_qpf;
        QPointF p3Tela2D = p3_qpf;

        QPointF p1c = p1Tela2D, p2c = p2Tela2D;
        QPointF p2c_2 = p2Tela2D, p3c = p3Tela2D;
        QPointF p3c_2 = p3Tela2D, p1c_2 = p1Tela2D;

        if (clipper.clipLinha(p1c, p2c)) {
            pintar.drawLine(p1c, p2c);
        }
        if (clipper.clipLinha(p2c_2, p3c)) {
            pintar.drawLine(p2c_2, p3c);
        }
        if (clipper.clipLinha(p3c_2, p1c_2)) {
            pintar.drawLine(p3c_2, p1c_2);
        }
    }

    if(this->isSelecionado()){
        pintar.setPen(QPen(Qt::red, 2));
        for(const Ponto4D& p_original : pontos){
            QPointF pTela = mFinal.transformarPonto(p_original).paraQPointF();
            if(limites.contains(pTela)) {
                pintar.drawPoint(pTela);
            }
        }
    }
}
