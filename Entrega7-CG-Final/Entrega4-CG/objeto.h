#ifndef OBJETO_H
#define OBJETO_H

#include <QString>
#include <QList>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QPointF>
#include <QColor>
#include <cmath>
#include <QDebug>
#include <QRectF>
#include <algorithm>

enum TipoObjeto{
    PONTO,
    RETA,
    POLIGONO,
    CURVA_BEZIER,
    MALHA_3D
};

struct Ponto4D {
    float x, y, z, w;

    Ponto4D(float x = 0, float y = 0, float z = 0, float w = 1.0) : x(x), y(y), z(z), w(w) {

    }

    Ponto4D normalizado() const {
        float mag = sqrt(x*x + y*y + z*z);
        if (mag < 1e-6f) return *this;
        //Vetor normalizado geralmente é vetor de direção (w=0), mas mantemos estrutura
        return Ponto4D(x / mag, y / mag, z / mag, w);
    }

    QPointF paraQPointF() const{
        if (fabs(w) < 1e-6f || w == 1.0f)
        return QPointF(x, y);

        return QPointF(x / w, y / w);
    }

    // Operadores Vetoriais para Iluminação
    Ponto4D operator-(const Ponto4D& p) const {
        return Ponto4D(x - p.x, y - p.y, z - p.z, 0.0f); //Vetor resultante
    }
    Ponto4D operator+(const Ponto4D& p) const {
        return Ponto4D(x + p.x, y + p.y, z + p.z, w + p.w); //Soma ponto+vetor ou vetor+vetor
    }
    Ponto4D operator*(float t) const {
        return Ponto4D(x * t, y * t, z * t, w * t);
    }

    //Produto Escalar
    float dot(const Ponto4D& p) const {
        return x * p.x + y * p.y + z * p.z;
    }

    //Produto Vetorial
    Ponto4D cross(const Ponto4D& p) const {
        return Ponto4D(y * p.z - z * p.y, z * p.x - x * p.z, x * p.y - y * p.x, 0.0f);
    }
};

inline Ponto4D operator*(float t, const Ponto4D& p) {
    return p * t;
}

//Estrutura para Fonte de Luz
struct FonteLuz {
    Ponto4D posicao;
    QColor corAmbiente;
    QColor corDifusa;
    QColor corEspecular;

    FonteLuz(bool intensa = true) {

        if (intensa) {
            //Configuração Original (Luz Forte)
            posicao = Ponto4D(0, 800, 200);
            corAmbiente = QColor(60, 60, 60);
            corDifusa = QColor(255, 255, 255);
            corEspecular = QColor(255, 255, 255);
        }
        else {
            //Configuração Alternativa (Exemplo: Luz Fraca/Ambiente)
            posicao = Ponto4D(0, 100, 0);
            corAmbiente = QColor(20, 20, 20);
            corDifusa = QColor(100, 100, 100);
            corEspecular = QColor(50, 50, 50);
        }
    }
};

struct Matriz4x4 {
    float m[4][4];

    Matriz4x4() {
        setIdentidade();
    }

    void setIdentidade() {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                m[i][j] = (i == j) ? 1.0f : 0.0f;
            }
        }
    }

    void multiplicar(const Matriz4x4& B) {
        Matriz4x4 resultado;
        for(int i=0; i<4; i++)
        for(int j=0; j<4; j++)
            resultado.m[i][j] = 0;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                for(int k=0; k<4; ++k)
                    resultado.m[i][j] += m[i][k] * B.m[k][j];
            }
        }
        *this = resultado;
    }

    Ponto4D transformarPonto(const Ponto4D& p) const {
        Ponto4D res;
        res.x = p.x * m[0][0] + p.y * m[1][0] + p.z * m[2][0] + p.w * m[3][0];
        res.y = p.x * m[0][1] + p.y * m[1][1] + p.z * m[2][1] + p.w * m[3][1];
        res.z = p.x * m[0][2] + p.y * m[1][2] + p.z * m[2][2] + p.w * m[3][2];
        res.w = p.x * m[0][3] + p.y * m[1][3] + p.z * m[2][3] + p.w * m[3][3];
        return res;
    }
    //Achei esse algoritomo de inversão de matriz: Algoritmo de Gauss! Resolvi testar (Deu certo).
    //Pq o outro era muito grande! Quem for ler percebera a diferença.
    Matriz4x4 inverter() const {
        Matriz4x4 inv;
        float temp[4][4];

        //Copia a matriz atual para temp e inicializa inv como identidade
        for(int i=0;i<4;i++) {
            for(int j=0;j<4;j++) {
                temp[i][j] = m[i][j];
                inv.m[i][j] = (i==j)?1.0f:0.0f;
            }
        }
        for (int i = 0; i < 4; i++){
            float pivot = temp[i][i];
            if (fabs(pivot) < 1e-6) return Matriz4x4(); //Matriz singular (erro)

            for (int j = 0; j < 4; j++) {
                temp[i][j] /= pivot;
                inv.m[i][j] /= pivot;
            }
            for (int k = 0; k < 4; k++) {
                if (k != i) {
                    float factor = temp[k][i];
                    for (int j = 0; j < 4; j++) {
                        temp[k][j] -= factor * temp[i][j];
                        inv.m[k][j] -= factor * inv.m[i][j];
                    }
                }
            }
        }
        return inv;
    }
};
class ObjetoGeometrico{
public:
    ObjetoGeometrico(const QString& nome, TipoObjeto tipo);
    virtual ~ObjetoGeometrico(){

    }

    virtual void desenhar(QPainter& pintar, const Matriz4x4& matrizFinal, const QRectF& limites) = 0;

    virtual Ponto4D getCentroGeometrico() const = 0;

    void addPonto(const Ponto4D& p);
    QString getNome() const;
    TipoObjeto getTipo() const;
    const QList<Ponto4D>& getPontos() const;

    void selecionar();
    void deselecionar();
    bool isSelecionado() const {
        return selecionado;
    }

    void transladar(double dx, double dy, double dz);
    void escalar(double sx, double sy, double sz);
    void rotacionarX(double angulo);
    void rotacionarY(double angulo);
    void rotacionarZ(double angulo);

    const Matriz4x4& getTransform() const {
        return m_transform;
    }

    Ponto4D getTotalTranslacao() const {
        return m_total_translacao;
    }
    Ponto4D getTotalEscala() const{
        return m_total_escala;
    }
    Ponto4D getTotalRotacao() const{
        return m_total_rotacao;
    }

    void setCor(const QColor& cor){
        m_cor = cor;
    }
    QColor getCor() const {
        return m_cor;
    }

protected:
    QString nome;
    TipoObjeto tipo;
    QList<Ponto4D> pontos;
    Matriz4x4 m_transform;

    Ponto4D m_total_translacao {0, 0, 0, 0};
    Ponto4D m_total_escala {1, 1, 1, 0};
    Ponto4D m_total_rotacao {0, 0, 0, 0};

    bool selecionado = false;
    QColor m_cor {
        Qt::gray
    };
};

class Ponto : public ObjetoGeometrico {
public:
    Ponto(const QString& nome, const Ponto4D& p);
    void desenhar(QPainter& pintar, const Matriz4x4& matrizFinal, const QRectF& limites) override;
    Ponto4D getCentroGeometrico() const override;
};

class Reta : public ObjetoGeometrico {
public:
    Reta(const QString& nome, const Ponto4D& p1, const Ponto4D& p2);
    void desenhar(QPainter& pintar, const Matriz4x4& matrizFinal, const QRectF& limites) override;
    Ponto4D getCentroGeometrico() const override;
};

class Poligono : public ObjetoGeometrico {
public:
    Poligono(const QString& nome);
    void finalizar();
    bool isFinalizado() const {
        return finalizado;
    }
    void desenhar(QPainter& pintar, const Matriz4x4& matrizFinal, const QRectF& limites) override;
    Ponto4D getCentroGeometrico() const override;

private:
    bool finalizado;
};

class CurvaBezier : public ObjetoGeometrico {
public:
    CurvaBezier(const QString& nome);
    void desenhar(QPainter& pintar,  const Matriz4x4& matrizFinal, const QRectF& limites) override;
    Ponto4D getCentroGeometrico() const override;

private:
    Ponto4D calcularPontoDeCasteljau(const QList<Ponto4D>& pontosDeControlo, double t) const;
    int numSegmentos = 30;
};

struct Face {
    int v1, v2, v3;
};

class Malha3D : public ObjetoGeometrico {
public:
    Malha3D(const QString& nome);

    void desenhar(QPainter& pintar, const Matriz4x4& matrizFinal, const QRectF& limites) override;
    Ponto4D getCentroGeometrico() const override;
    void addFace(int v1, int v2, int v3);

    //Propriedades do Material para Phong
    float ka = 0.2f; //Ambiente
    float kd = 0.6f; //Difuso
    float ks = 0.5f; //Especular
    float shininess = 30.0f;

    //Variáveis estáticas para contexto de renderização
    static Ponto4D s_posCamera;
    static FonteLuz s_luz;

private:
    QList<Face> m_faces;
    QColor calcularCorPhong(const Ponto4D& normal, const Ponto4D& centroFace, const QColor& corBase);
};

//Resolvi criar um chao para projetar a sombra: Classe para o CHÃO
class Chao : public ObjetoGeometrico {
public:
    Chao(float tamanho, float yAltura);
    void desenhar(QPainter& pintar, const Matriz4x4& matrizFinal, const QRectF& limites) override;
    Ponto4D getCentroGeometrico() const override {
        return Ponto4D(0,0,0);
    }
private:
    float m_tamanho;
    float m_y;
};

#endif // OBJETO_H
