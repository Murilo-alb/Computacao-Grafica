#include "Objeto.h"
#include "clipping.h"
#include <cmath>
#include <algorithm>

Ponto4D Malha3D::s_posCamera(0, 0, 500);
FonteLuz Malha3D::s_luz;

Matriz4x4 criarMatrizTranslacao(float dx, float dy, float dz) {
    Matriz4x4 T;
    T.m[3][0]=dx;
    T.m[3][1]=dy;
    T.m[3][2]=dz;
    return T;
}
Matriz4x4 criarMatrizEscala(float sx, float sy, float sz) {
    Matriz4x4 S;
    S.m[0][0]=sx;
    S.m[1][1]=sy;
    S.m[2][2]=sz;
    return S;
}
Matriz4x4 criarMatrizRotacaoX(float a) {
    Matriz4x4 R;
    float r=a*M_PI/180, c=cos(r), s=sin(r);
    R.m[1][1]=c; R.m[1][2]=s;
    R.m[2][1]=-s; R.m[2][2]=c;
    return R;
}
Matriz4x4 criarMatrizRotacaoY(float a) {
    Matriz4x4 R;
    float r=a*M_PI/180, c=cos(r), s=sin(r);
    R.m[0][0]=c; R.m[0][2]=-s;
    R.m[2][0]=s; R.m[2][2]=c;
    return R;
}
Matriz4x4 criarMatrizRotacaoZ(float a) {
    Matriz4x4 R;
    float r=a*M_PI/180, c=cos(r), s=sin(r);
    R.m[0][0]=c; R.m[0][1]=s;
    R.m[1][0]=-s;
    R.m[1][1]=c;
    return R;
}
Matriz4x4 criarMatrizSombra(const Ponto4D& l) {
    Matriz4x4 M;
    float ly=(fabs(l.y)<0.1f)?0.1f:l.y;
    M.m[0][0]=1;
    M.m[1][0]=-l.x/ly;
    M.m[1][1]=0;
    M.m[1][2]=-l.z/ly;
    M.m[2][2]=1;
    M.m[3][3]=1;
    return M;
}

//Viewport Transform
//Deve ser o inverso exato da lógica do Mouse NDC no Tela.cpp
QPointF clipToScreen(const Ponto4D& p, const QRectF& lim) {
    if (fabs(p.w) < 1e-9) return QPointF(0,0);

    //NDC (-1 a 1)
    float nx = p.x/p.w;
    float ny = p.y/p.w;

    float w = lim.width();
    float h = lim.height();

    //Y cresce para baixo. NDC Y cresce para cima.
    //ScreenX = Xmin + (NdcX + 1)/2 * W
    //ScreenY = Ymin + (1 - NdcY)/2 * H  <-- Inversão aqui
    return QPointF(lim.x() + (nx + 1.0f) * 0.5f * w, lim.y() + (1.0f - ny) * 0.5f * h);
}

ObjetoGeometrico::ObjetoGeometrico(const QString& n, TipoObjeto t) : nome(n), tipo(t) {
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
    selecionado=true;
}
void ObjetoGeometrico::deselecionar(){
    selecionado=false;
}
void ObjetoGeometrico::transladar(double dx, double dy, double dz){
    m_transform.multiplicar(criarMatrizTranslacao(dx,dy,dz));
    m_total_translacao=m_total_translacao+Ponto4D(dx,dy,dz,0);
}
void ObjetoGeometrico::escalar(double sx, double sy, double sz){
    Ponto4D c=getCentroGeometrico();
    m_transform.multiplicar(criarMatrizTranslacao(-c.x,-c.y,-c.z));
    m_transform.multiplicar(criarMatrizEscala(sx,sy,sz));
    m_transform.multiplicar(criarMatrizTranslacao(c.x,c.y,c.z));
    m_total_escala.x*=sx; m_total_escala.y*=sy; m_total_escala.z*=sz;
}
void ObjetoGeometrico::rotacionarX(double a){
    Ponto4D c=getCentroGeometrico(); m_transform.multiplicar(criarMatrizTranslacao(-c.x,-c.y,-c.z));
    m_transform.multiplicar(criarMatrizRotacaoX(a));
    m_transform.multiplicar(criarMatrizTranslacao(c.x,c.y,c.z));
    m_total_rotacao.x+=a;
}
void ObjetoGeometrico::rotacionarY(double a){
    Ponto4D c=getCentroGeometrico(); m_transform.multiplicar(criarMatrizTranslacao(-c.x,-c.y,-c.z));
    m_transform.multiplicar(criarMatrizRotacaoY(a));
    m_transform.multiplicar(criarMatrizTranslacao(c.x,c.y,c.z));
    m_total_rotacao.y+=a;
}
void ObjetoGeometrico::rotacionarZ(double a){
    Ponto4D c=getCentroGeometrico(); m_transform.multiplicar(criarMatrizTranslacao(-c.x,-c.y,-c.z));
    m_transform.multiplicar(criarMatrizRotacaoZ(a));
    m_transform.multiplicar(criarMatrizTranslacao(c.x,c.y,c.z));
    m_total_rotacao.z+=a;
}

//PONTO
Ponto::Ponto(const QString& n, const Ponto4D& p) : ObjetoGeometrico(n, PONTO) {
    pontos.append(p);
}
Ponto4D Ponto::getCentroGeometrico() const {
    return pontos.isEmpty()?Ponto4D():pontos.first();
}
void Ponto::desenhar(
    QPainter& p, const Matriz4x4& m, const QRectF& lim){
    if(pontos.isEmpty())
        return;
    Matriz4x4 mFinal = m_transform; mFinal.multiplicar(m);
    Ponto4D pClip = mFinal.transformarPonto(pontos.first());

    //Desenha sem clipping rigoroso para garantir visibilidade do ponto único
    QPointF pt = clipToScreen(pClip, lim);
    if(lim.contains(pt)) {
        p.setPen(QPen(selecionado?Qt::yellow:m_cor, selecionado?7:5));
        p.drawEllipse(pt, 5, 5);
    }
}

//RETA
Reta::Reta(const QString& n, const Ponto4D& p1, const Ponto4D& p2) : ObjetoGeometrico(n, RETA) {
    pontos<<p1<<p2;
}
Ponto4D Reta::getCentroGeometrico() const {
    return pontos.size()<2?Ponto4D():(pontos[0]+pontos[1])*0.5;
}
void Reta::desenhar(QPainter& p, const Matriz4x4& m, const QRectF& lim) {
    if(pontos.size()<2)
        return;
    Matriz4x4 mFinal = m_transform; mFinal.multiplicar(m);
    Ponto4D p1=mFinal.transformarPonto(pontos[0]), p2=mFinal.transformarPonto(pontos[1]);
    if(!Clipping::clipLinha3D(p1, p2))
        return;
    QPointF s1=clipToScreen(p1, lim), s2=clipToScreen(p2, lim);
    Clipping c2d(lim); if(c2d.clipLinha(s1, s2)){
        p.setPen(QPen(selecionado?Qt::yellow:m_cor, 2)); p.drawLine(s1, s2);
    }
}

//POLIGONO
Poligono::Poligono(const QString& n) : ObjetoGeometrico(n, POLIGONO) {
    finalizado=false;
}
void Poligono::finalizar() {
    finalizado=true;
}
Ponto4D Poligono::getCentroGeometrico() const {
    Ponto4D s; for(auto pt:pontos) s=s+pt; return pontos.isEmpty()?s:s*(1.0/pontos.size());
}

void Poligono::desenhar(QPainter& p, const Matriz4x4& m, const QRectF& lim) {
    if(pontos.isEmpty()) return;
    Matriz4x4 mFinal = m_transform; mFinal.multiplicar(m);
    Clipping c2d(lim); p.setPen(QPen(selecionado?Qt::yellow:m_cor, 2));
    QList<Ponto4D> pts; for(auto pt:pontos) pts.append(mFinal.transformarPonto(pt));

    //Linhas
    for(int i=0; i<pts.size(); ++i){
        if(!finalizado && i==pts.size()-1) break;
        Ponto4D p1=pts[i], p2=pts[(i+1)%pts.size()];
        if(Clipping::clipLinha3D(p1, p2)){
            QPointF s1=clipToScreen(p1, lim), s2=clipToScreen(p2, lim);
            if(c2d.clipLinha(s1, s2)) p.drawLine(s1, s2);
        }
    }
    //Vértices (Sempre desenha se não finalizado, para você ver onde clicou)
    if(selecionado || !finalizado){
        p.setPen(QPen(Qt::yellow, 5));
        for(auto pt:pts) {
            //Desenha direto para garantir feedback visual
            QPointF sc = clipToScreen(pt, lim);
            if(lim.contains(sc)) p.drawPoint(sc);
        }
    }
}

//CURVA BEZIER
CurvaBezier::CurvaBezier(const QString& n) : ObjetoGeometrico(n, CURVA_BEZIER) {

}
Ponto4D CurvaBezier::getCentroGeometrico() const {
    Ponto4D s;
    for(auto pt:pontos) s=s+pt;
    return pontos.isEmpty()?s:s*(1.0/pontos.size());
}
Ponto4D CurvaBezier::calcularPontoDeCasteljau(const QList<Ponto4D>& pts, double t) const {
    if(pts.size()==1) return pts.first();
    QList<Ponto4D> n;
    for(int i=0;i<pts.size()-1;++i) n.append((1-t)*pts[i]+t*pts[i+1]);
    return calcularPontoDeCasteljau(n, t);
}
void CurvaBezier::desenhar(QPainter& p, const Matriz4x4& m, const QRectF& lim) {
    Matriz4x4 mFinal = m_transform;
    mFinal.multiplicar(m); Clipping c2d(lim);

    //Controle
    if(selecionado || pontos.size() > 0){
        p.setPen(QPen(Qt::darkGray, 1, Qt::DashLine)); QList<Ponto4D> c;
        for(auto pt:pontos) c.append(mFinal.transformarPonto(pt)); //Durante o codigo tem alguns alertas! Nao sei se é pela versão, mas tem...
        for(int i=0; i<c.size()-1; ++i){
            Ponto4D p1=c[i], p2=c[i+1];
            if(Clipping::clipLinha3D(p1, p2)){
                QPointF s1=clipToScreen(p1, lim), s2=clipToScreen(p2, lim);
                if(c2d.clipLinha(s1, s2)) p.drawLine(s1, s2); }
        }

        p.setPen(QPen(Qt::red, 5));
        for(auto pt:c) {
            QPointF sc = clipToScreen(pt, lim);
            if(lim.contains(sc)) p.drawPoint(sc);
        }
    }
    if(pontos.size()<2)
        return;
    p.setPen(QPen(selecionado?Qt::yellow:m_cor, 2));
    QList<Ponto4D> curve;
    double step=1.0/numSegmentos;
    for(int i=0;i<=numSegmentos;++i) curve.append(calcularPontoDeCasteljau(pontos, i*step));
    for(int i=0; i<curve.size()-1; ++i){
        Ponto4D p1=mFinal.transformarPonto(curve[i]), p2=mFinal.transformarPonto(curve[i+1]);
        if(Clipping::clipLinha3D(p1, p2)){
            QPointF s1=clipToScreen(p1, lim), s2=clipToScreen(p2, lim);
            if(c2d.clipLinha(s1, s2)) p.drawLine(s1, s2);
        }
    }
}

//MALHA 3D
Malha3D::Malha3D(const QString& n) : ObjetoGeometrico(n, MALHA_3D){

}
void Malha3D::addFace(int v1, int v2, int v3){
    m_faces.append(Face{v1, v2, v3});
}
Ponto4D Malha3D::getCentroGeometrico() const {
    Ponto4D s; for(auto pt:pontos) s=s+pt;
    return pontos.isEmpty()?s:s*(1.0/pontos.size());
}

QColor Malha3D::calcularCorPhong(const Ponto4D& n, const Ponto4D& c, const QColor& base) {
    Ponto4D N=n.normalizado(), L=(s_luz.posicao-c).normalizado(), V=(s_posCamera-c).normalizado();
    float NdotL = std::max(0.0f, N.dot(L));
    float r = s_luz.corAmbiente.redF()*ka, g=s_luz.corAmbiente.greenF()*ka, b=s_luz.corAmbiente.blueF()*ka;
    if(NdotL>0){
        r+=s_luz.corDifusa.redF()*kd*NdotL*base.redF();
        g+=s_luz.corDifusa.greenF()*kd*NdotL*base.greenF();
        b+=s_luz.corDifusa.blueF()*kd*NdotL*base.blueF();
        Ponto4D R=((N*2.0f*NdotL)-L).normalizado();
        float RdotV=std::max(0.0f, R.dot(V));
        if(RdotV>0){
            float s=pow(RdotV, shininess);
            r+=s_luz.corEspecular.redF()*ks*s;
            g+=s_luz.corEspecular.greenF()*ks*s;
            b+=s_luz.corEspecular.blueF()*ks*s;
        }
    }
    return QColor::fromRgbF(std::min(1.0f, r), std::min(1.0f, g), std::min(1.0f, b));
}

void Malha3D::desenhar(QPainter& p, const Matriz4x4& m, const QRectF& lim) {
    if(m_faces.isEmpty()) return;
    Matriz4x4 mT = m_transform; mT.multiplicar(m);
    Matriz4x4 mS = m_transform; mS.multiplicar(criarMatrizSombra(s_luz.posicao));
    mS.multiplicar(criarMatrizTranslacao(0,0.1f,0)); mS.multiplicar(m);

    struct FaceRender {
        Face f;
        float z;
        Ponto4D n, c;
    };
    QList<FaceRender> fr;
    fr.reserve(m_faces.size());
    Matriz4x4 mModel = m_transform;
    for(const auto& face : m_faces){
        Ponto4D p1=mModel.transformarPonto(pontos[face.v1]), p2=mModel.transformarPonto(pontos[face.v2]), p3=mModel.transformarPonto(pontos[face.v3]);
        Ponto4D centro=(p1+p2+p3)*(1.0/3.0), norm=(p2-p1).cross(p3-p1).normalizado();
        float dx=centro.x-s_posCamera.x, dy=centro.y-s_posCamera.y, dz=centro.z-s_posCamera.z;
        fr.append({face, dx*dx+dy*dy+dz*dz, norm, centro});
    }
    std::sort(fr.begin(), fr.end(), [](const FaceRender& a, const FaceRender& b){
        return a.z > b.z;
    }
    );

    p.setPen(QColor(0,0,0,80));
    p.setBrush(QColor(0,0,0,80));
    for(const auto& f : fr){
        Ponto4D p1=mS.transformarPonto(pontos[f.f.v1]), p2=mS.transformarPonto(pontos[f.f.v2]), p3=mS.transformarPonto(pontos[f.f.v3]);
        Ponto4D t1=p1, t2=p2, t3=p3;
        if(Clipping::isPontoVisivel(t1) && Clipping::isPontoVisivel(t2) && Clipping::isPontoVisivel(t3)) {
            QPolygonF poly;
            poly<<clipToScreen(p1, lim)<<clipToScreen(p2, lim)<<clipToScreen(p3, lim);
            p.drawPolygon(poly);
        } else {
            t1=p1; t2=p2; if(Clipping::clipLinha3D(t1, t2)) p.drawLine(clipToScreen(t1, lim), clipToScreen(t2, lim));
            t1=p2; t2=p3; if(Clipping::clipLinha3D(t1, t2)) p.drawLine(clipToScreen(t1, lim), clipToScreen(t2, lim));
            t1=p3; t2=p1; if(Clipping::clipLinha3D(t1, t2)) p.drawLine(clipToScreen(t1, lim), clipToScreen(t2, lim));
        }
    }
    p.setPen(selecionado?Qt::yellow:Qt::NoPen);
    for(const auto& f : fr){
        Ponto4D p1=mT.transformarPonto(pontos[f.f.v1]), p2=mT.transformarPonto(pontos[f.f.v2]), p3=mT.transformarPonto(pontos[f.f.v3]);
        Ponto4D t1=p1, t2=p2, t3=p3;
        QColor cor = calcularCorPhong(f.n, f.c, m_cor);
        p.setBrush(cor);
        p.setPen(cor);
        if(Clipping::isPontoVisivel(t1) && Clipping::isPontoVisivel(t2) && Clipping::isPontoVisivel(t3)) {
            QPolygonF poly;
            poly<<clipToScreen(p1, lim)<<clipToScreen(p2, lim)<<clipToScreen(p3, lim);
            p.drawPolygon(poly);
        } else {
            p.setBrush(Qt::NoBrush);
            t1=p1; t2=p2; if(Clipping::clipLinha3D(t1, t2)) p.drawLine(clipToScreen(t1, lim), clipToScreen(t2, lim));
            t1=p2; t2=p3; if(Clipping::clipLinha3D(t1, t2)) p.drawLine(clipToScreen(t1, lim), clipToScreen(t2, lim));
            t1=p3; t2=p1; if(Clipping::clipLinha3D(t1, t2)) p.drawLine(clipToScreen(t1, lim), clipToScreen(t2, lim));
        }
    }
}

// --- Chao ---
Chao::Chao(float t, float y) : ObjetoGeometrico("Chao", POLIGONO), m_tamanho(t), m_y(y) {
    m_cor=QColor(200,200,200);
}
void Chao::desenhar(QPainter& p, const Matriz4x4& m, const QRectF& lim) {
    Matriz4x4 mT = m;
    p.setBrush(Qt::NoBrush);
    float st=100.0f;
    p.setPen(QPen(m_cor, 1));
    for(float i=-m_tamanho; i<=m_tamanho; i+=st){
        Ponto4D p1=mT.transformarPonto(Ponto4D(i, m_y, -m_tamanho)), p2=mT.transformarPonto(Ponto4D(i, m_y, m_tamanho));
        if(Clipping::clipLinha3D(p1, p2)) p.drawLine(clipToScreen(p1, lim), clipToScreen(p2, lim));
        Ponto4D p3=mT.transformarPonto(Ponto4D(-m_tamanho, m_y, i)), p4=mT.transformarPonto(Ponto4D(m_tamanho, m_y, i));
        if(Clipping::clipLinha3D(p3, p4)) p.drawLine(clipToScreen(p3, lim), clipToScreen(p4, lim));
    }
    p.setPen(QPen(Qt::red, 3)); Ponto4D xi=mT.transformarPonto(Ponto4D(0,m_y,0)), xf=mT.transformarPonto(Ponto4D(300,m_y,0));
    if(Clipping::clipLinha3D(xi, xf)) p.drawLine(clipToScreen(xi, lim), clipToScreen(xf, lim));
    p.setPen(QPen(Qt::blue, 3)); Ponto4D zi=mT.transformarPonto(Ponto4D(0,m_y,0)), zf=mT.transformarPonto(Ponto4D(0,m_y,300));
    if(Clipping::clipLinha3D(zi, zf)) p.drawLine(clipToScreen(zi, lim), clipToScreen(zf, lim));
}
