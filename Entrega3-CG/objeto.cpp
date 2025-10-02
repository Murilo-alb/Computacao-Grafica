#include "Objeto.h"
#include <cmath> // Já incluído em Objeto.h, mas por clareza

// --- Funções Auxiliares de Matriz --- (sem alterações aqui, já que a lógica da inversa foi para o struct)

Matriz3x3 criarMatrizTranslacao(float dx, float dy) {
    Matriz3x3 T;
    T.m[2][0] = dx;
    T.m[2][1] = dy;
    return T;
}

Matriz3x3 criarMatrizEscala(float sx, float sy) {
    Matriz3x3 S;
    S.m[0][0] = sx;
    S.m[1][1] = sy;
    return S;
}

Matriz3x3 criarMatrizRotacao(float angulo) {
    Matriz3x3 R;
    float radianos = angulo * M_PI / 180.0;
    float cosA = cos(radianos);
    float sinA = sin(radianos);
    R.m[0][0] = cosA;
    R.m[0][1] = sinA;
    R.m[1][0] = -sinA;
    R.m[1][1] = cosA;
    return R;
}

// --- Implementações da classe ObjetoGeometrico ---

ObjetoGeometrico::ObjetoGeometrico(const QString& nome, TipoObjeto tipo) : nome(nome), tipo(tipo) {}

//addPonto agora aceita QPointF
void ObjetoGeometrico::addPonto(const QPointF& p) {
    pontos.append(p);
}

QString ObjetoGeometrico::getNome() const { return nome; }
TipoObjeto ObjetoGeometrico::getTipo() const { return tipo; }
//Retorna uma lista de QPointF
const QList<QPointF>& ObjetoGeometrico::getPontos() const { return pontos; }

void ObjetoGeometrico::selecionar(){ this->selecionado = true; }
void ObjetoGeometrico::deselecionar(){ this->selecionado = false; }

QPointF ObjetoGeometrico::getTotalTranslacao() const { return m_total_translacao; }
QPointF ObjetoGeometrico::getTotalEscala() const { return m_total_escala; }
double ObjetoGeometrico::getTotalRotacao() const { return m_total_rotacao; }

void ObjetoGeometrico::transladar(double dx, double dy)
{
    Matriz3x3 matrizTranslacao = criarMatrizTranslacao(dx, dy);
    m_transform.multiplicar(matrizTranslacao);
    m_total_translacao += QPointF(dx, dy);
}

void ObjetoGeometrico::escalar(double sx, double sy)
{
    QPointF centro = getCentroGeometrico();
    Matriz3x3 T_para_origem = criarMatrizTranslacao(-centro.x(), -centro.y());
    Matriz3x3 S = criarMatrizEscala(sx, sy);
    Matriz3x3 T_de_volta = criarMatrizTranslacao(centro.x(), centro.y());

    // Compõe a matriz de escala em torno do centro
    T_para_origem.multiplicar(S);
    T_para_origem.multiplicar(T_de_volta);

    m_transform.multiplicar(T_para_origem);
    m_total_escala.setX(m_total_escala.x() * sx);
    m_total_escala.setY(m_total_escala.y() * sy);
}

void ObjetoGeometrico::rotacionar(double angulo, const QPointF& pivo)
{
    Matriz3x3 T_para_origem = criarMatrizTranslacao(-pivo.x(), -pivo.y());
    Matriz3x3 R = criarMatrizRotacao(angulo);
    Matriz3x3 T_de_volta = criarMatrizTranslacao(pivo.x(), pivo.y());

    // Compõe a matriz de rotação em torno do pivô
    T_para_origem.multiplicar(R);
    T_para_origem.multiplicar(T_de_volta);

    m_transform.multiplicar(T_para_origem);
    m_total_rotacao += angulo;
}

// --- Implementações das classes Ponto, Reta e Poligono ---

//Construtor de Ponto agora aceita QPointF
Ponto::Ponto(const QString& nome, const QPointF& p) : ObjetoGeometrico(nome, PONTO) {
    pontos.append(p);
}

void Ponto::desenhar(QPainter& pintar, const Matriz3x3& matrizVisualizacao) {
    if (pontos.isEmpty()) return;

    Matriz3x3 matrizFinal = m_transform;
    matrizFinal.multiplicar(matrizVisualizacao);

    QPointF p_transformado = matrizFinal.transformarPonto(pontos.first());

    pintar.setPen(QPen(Qt::red, 5));
    pintar.drawPoint(p_transformado);
}

QPointF Ponto::getCentroGeometrico() const {
    if (pontos.isEmpty()) return QPointF(0, 0);
    return pontos.first(); // pontos.first() já é QPointF
}

//Construtor de Reta agora aceita QPointF
Reta::Reta(const QString& nome, const QPointF& p1, const QPointF& p2) : ObjetoGeometrico(nome, RETA) {
    pontos.append(p1);
    pontos.append(p2);
}

void Reta::desenhar(QPainter& pintar, const Matriz3x3& matrizVisualizacao) {
    if (pontos.size() < 2) return;

    Matriz3x3 matrizFinal = m_transform;
    matrizFinal.multiplicar(matrizVisualizacao);
    // pontos[0] e pontos[1] já são QPointF
    QPointF p1_transformado = matrizFinal.transformarPonto(pontos[0]);
    QPointF p2_transformado = matrizFinal.transformarPonto(pontos[1]);

    pintar.setPen(QPen(Qt::green, 2));
    pintar.drawLine(p1_transformado, p2_transformado);
}

QPointF Reta::getCentroGeometrico() const {
    if (pontos.size() < 2) return QPointF(0, 0);
    // pontos[0] e pontos[1] já são QPointF
    return (pontos[0] + pontos[1]) / 2.0;
}

Poligono::Poligono(const QString& nome) : ObjetoGeometrico(nome, POLIGONO) {
    finalizado = false;
}

void Poligono::finalizar() {
    this->finalizado = true;
}

void Poligono::desenhar(QPainter& pintar, const Matriz3x3& matrizVisualizacao) {
    if (pontos.isEmpty()) return;

    Matriz3x3 matrizFinal = m_transform;
    matrizFinal.multiplicar(matrizVisualizacao);

    QPolygonF poligonoTransformado;
    // pontos é QList<QPointF>
    // existe um warning que nao sei resolver
    for(const QPointF& ponto : pontos) {
        poligonoTransformado.append(matrizFinal.transformarPonto(ponto));
    }

    if (finalizado && pontos.size() > 1) {
        pintar.setPen(QPen(Qt::blue, 2));
        pintar.drawPolygon(poligonoTransformado);
    }

    if(this->selecionado){
        pintar.setPen(QPen(Qt::yellow, 7));
    } else {
        pintar.setPen(QPen(Qt::red, 5));
    }

    for(const QPointF& ponto : poligonoTransformado) {
        pintar.drawPoint(ponto);
    }
}

QPointF Poligono::getCentroGeometrico() const {
    if (pontos.isEmpty()) return QPointF(0, 0);
    double somaX = 0, somaY = 0;
    // pontos é QList<QPointF>
    for (const QPointF& p : pontos) {
        somaX += p.x();
        somaY += p.y();
    }
    return QPointF(somaX / pontos.size(), somaY / pontos.size());
}
