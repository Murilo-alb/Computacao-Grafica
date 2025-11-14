#ifndef OBJETO_H
#define OBJETO_H

#include <QString>
#include <QList>
#include <QPainter>
#include <QPen>
#include <QPointF>
#include <QColor>
#include <cmath>
#include <QDebug>
#include <QRectF> // Adicionado para os limites

enum TipoObjeto {
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
        if (w == 0 || w == 1) {
            return *this;
        }
        return Ponto4D(x / w, y / w, z / w, 1.0);
    }

    QPointF paraQPointF() const {
        Ponto4D p = normalizado();
        return QPointF(p.x, p.y);
    }

    Ponto4D operator*(float t) const {
        return Ponto4D(x * t, y * t, z * t, w * t);
    }

    Ponto4D operator+(const Ponto4D& p) const {
        return Ponto4D(x + p.x, y + p.y, z + p.z, w + p.w);
    }
};

inline Ponto4D operator*(float t, const Ponto4D& p) {
    return p * t;
}


//Estrutura Matriz4x4
struct Matriz4x4 {
    float m[4][4];

    Matriz4x4() {
        setIdentidade();
    }

    void setIdentidade() {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                if (i == j) {
                    m[i][j] = 1.0f;
                } else {
                    m[i][j] = 0.0f;
                }
            }
        }
    }

    void multiplicar(const Matriz4x4& B) {
        Matriz4x4 resultado;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                resultado.m[i][j] = m[i][0] * B.m[0][j] + m[i][1] * B.m[1][j] + m[i][2] * B.m[2][j] + m[i][3] * B.m[3][j];
            }
        }
        *this = resultado;
    }

    Ponto4D transformarPonto(const Ponto4D& p) const {
        Ponto4D resultado;
        resultado.x = p.x * m[0][0] + p.y * m[1][0] + p.z * m[2][0] + p.w * m[3][0];
        resultado.y = p.x * m[0][1] + p.y * m[1][1] + p.z * m[2][1] + p.w * m[3][1];
        resultado.z = p.x * m[0][2] + p.y * m[1][2] + p.z * m[2][2] + p.w * m[3][2];
        resultado.w = p.x * m[0][3] + p.y * m[1][3] + p.z * m[2][3] + p.w * m[3][3];
        return resultado;
    }

    //Isso aqui é brincadeira fazer na mão...
    Matriz4x4 inverter() const {
        Matriz4x4 inversa;
        float det;
        inversa.m[0][0] = m[1][1] * m[2][2] * m[3][3] - m[1][1] * m[2][3] * m[3][2] - m[2][1] * m[1][2] * m[3][3] + m[2][1] * m[1][3] * m[3][2] + m[3][1] * m[1][2] * m[2][3] - m[3][1] * m[1][3] * m[2][2];
        inversa.m[1][0] = -m[1][0] * m[2][2] * m[3][3] + m[1][0] * m[2][3] * m[3][2] + m[2][0] * m[1][2] * m[3][3] - m[2][0] * m[1][3] * m[3][2] - m[3][0] * m[1][2] * m[2][3] + m[3][0] * m[1][3] * m[2][2];
        inversa.m[2][0] = m[1][0] * m[2][1] * m[3][3] - m[1][0] * m[2][3] * m[3][1] - m[2][0] * m[1][1] * m[3][3] + m[2][0] * m[1][3] * m[3][1] + m[3][0] * m[1][1] * m[2][3] - m[3][0] * m[1][3] * m[2][1];
        inversa.m[3][0] = -m[1][0] * m[2][1] * m[3][2] + m[1][0] * m[2][2] * m[3][1] + m[2][0] * m[1][1] * m[3][2] - m[2][0] * m[1][2] * m[3][1] - m[3][0] * m[1][1] * m[2][2] + m[3][0] * m[1][2] * m[2][1];
        inversa.m[0][1] = -m[0][1] * m[2][2] * m[3][3] + m[0][1] * m[2][3] * m[3][2] + m[2][1] * m[0][2] * m[3][3] - m[2][1] * m[0][3] * m[3][2] - m[3][1] * m[0][2] * m[2][3] + m[3][1] * m[0][3] * m[2][2];
        inversa.m[1][1] = m[0][0] * m[2][2] * m[3][3] - m[0][0] * m[2][3] * m[3][2] - m[2][0] * m[0][2] * m[3][3] + m[2][0] * m[0][3] * m[3][2] + m[3][0] * m[0][2] * m[2][3] - m[3][0] * m[0][3] * m[2][2];
        inversa.m[2][1] = -m[0][0] * m[2][1] * m[3][3] + m[0][0] * m[2][3] * m[3][1] + m[2][0] * m[0][1] * m[3][3] - m[2][0] * m[0][3] * m[3][1] - m[3][0] * m[0][1] * m[2][3] + m[3][0] * m[0][3] * m[2][1];
        inversa.m[3][1] = m[0][0] * m[2][1] * m[3][2] - m[0][0] * m[2][2] * m[3][1] - m[2][0] * m[0][1] * m[3][2] + m[2][0] * m[0][2] * m[3][1] + m[3][0] * m[0][1] * m[2][2] - m[3][0] * m[0][2] * m[2][1];
        inversa.m[0][2] = m[0][1] * m[1][2] * m[3][3] - m[0][1] * m[1][3] * m[3][2] - m[1][1] * m[0][2] * m[3][3] + m[1][1] * m[0][3] * m[3][2] + m[3][1] * m[0][2] * m[1][3] - m[3][1] * m[0][3] * m[1][2];
        inversa.m[1][2] = -m[0][0] * m[1][2] * m[3][3] + m[0][0] * m[1][3] * m[3][2] + m[1][0] * m[0][2] * m[3][3] - m[1][0] * m[0][3] * m[3][2] - m[3][0] * m[0][2] * m[1][3] + m[3][0] * m[0][3] * m[1][2];
        inversa.m[2][2] = m[0][0] * m[1][1] * m[3][3] - m[0][0] * m[1][3] * m[3][1] - m[1][0] * m[0][1] * m[3][3] + m[1][0] * m[0][3] * m[3][1] + m[3][0] * m[0][1] * m[1][3] - m[3][0] * m[0][3] * m[1][1];
        inversa.m[3][2] = -m[0][0] * m[1][1] * m[3][2] + m[0][0] * m[1][2] * m[3][1] + m[1][0] * m[0][1] * m[3][2] - m[1][0] * m[0][2] * m[3][1] - m[3][0] * m[0][1] * m[1][2] + m[3][0] * m[0][2] * m[1][1];
        inversa.m[0][3] = -m[0][1] * m[1][2] * m[2][3] + m[0][1] * m[1][3] * m[2][2] + m[1][1] * m[0][2] * m[2][3] - m[1][1] * m[0][3] * m[2][2] - m[2][1] * m[0][2] * m[1][3] + m[2][1] * m[0][3] * m[1][2];
        inversa.m[1][3] = m[0][0] * m[1][2] * m[2][3] - m[0][0] * m[1][3] * m[2][2] - m[1][0] * m[0][2] * m[2][3] + m[1][0] * m[0][3] * m[2][2] + m[2][0] * m[0][2] * m[1][3] - m[2][0] * m[0][3] * m[1][2];
        inversa.m[2][3] = -m[0][0] * m[1][1] * m[2][3] + m[0][0] * m[1][3] * m[2][1] + m[1][0] * m[0][1] * m[2][3] - m[1][0] * m[0][3] * m[2][1] - m[2][0] * m[0][1] * m[1][3] + m[2][0] * m[0][3] * m[1][1];
        inversa.m[3][3] = m[0][0] * m[1][1] * m[2][2] - m[0][0] * m[1][2] * m[2][1] - m[1][0] * m[0][1] * m[2][2] + m[1][0] * m[0][2] * m[2][1] + m[2][0] * m[0][1] * m[1][2] - m[2][0] * m[0][2] * m[1][1];

        det = m[0][0] * inversa.m[0][0] + m[0][1] * inversa.m[1][0] + m[0][2] * inversa.m[2][0] + m[0][3] * inversa.m[3][0];

        if (fabs(det) < 1e-6) {
            qDebug() << "Determinante zero, matriz não inversível! Retornando identidade.";
            inversa.setIdentidade();
            return inversa;
        }

        float invDet = 1.0f / det;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                inversa.m[i][j] *= invDet;
            }
        }
        return inversa;
    }
};

class ObjetoGeometrico{
public:
    ObjetoGeometrico(const QString& nome, TipoObjeto tipo);
    virtual ~ObjetoGeometrico() {

    }

    // Assinatura alterada para aceitar a Matriz Final e os Limites 2D
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
    Ponto4D getTotalEscala() const {
        return m_total_escala;
    }
    Ponto4D getTotalRotacao() const {
        return m_total_rotacao;
    }

    void setCor(const QColor& cor) {
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

    QColor m_cor {Qt::gray};
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

private:
    QList<Face> m_faces;
};


#endif // OBJETO_H
