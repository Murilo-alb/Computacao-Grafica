#ifndef OBJETO_H
#define OBJETO_H

#include <QString>
#include <QList>
// #include <QPoint> // REMOVIDO: Não usarei QPoint para armazenar as coordenadas
#include <QPainter>
#include <QPen>
#include <QPointF>
#include <cmath> // Para M_PI e funções trigonométricas
#include <QDebug> // Para usar qDebug() em caso de matriz não inversível

enum TipoObjeto {
    PONTO,
    RETA,
    POLIGONO
};

struct Matriz3x3 {
    float m[3][3];

    Matriz3x3() {
        setIdentidade();
    }

    void setIdentidade() {
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                m[i][j] = (i == j) ? 1.0f : 0.0f;
            }
        }
    }

    void multiplicar(const Matriz3x3& B) {
        Matriz3x3 resultado; // Crie uma matriz temporária para armazenar o resultado
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                resultado.m[i][j] = m[i][0] * B.m[0][j] + m[i][1] * B.m[1][j] + m[i][2] * B.m[2][j];
            }
        }
        *this = resultado; // Atribua o resultado à matriz atual
    }

    QPointF transformarPonto(const QPointF& p) const {
        float x = p.x();
        float y = p.y();
        // Aplicação da matriz M a um ponto P.
        float novoX = x * m[0][0] + y * m[1][0] + m[2][0];
        float novoY = x * m[0][1] + y * m[1][1] + m[2][1];
        return QPointF(novoX, novoY);
    }

    // Método para calcular a matriz inversa
    Matriz3x3 inverter() const {
        Matriz3x3 inversa; // Para armazenar o resultado

        // Calcula o determinante da matriz 3x3
        float det = m[0][0] * (m[1][1] * m[2][2] - m[2][1] * m[1][2]) - m[0][1] * (m[1][0] * m[2][2] - m[2][0] * m[1][2]) + m[0][2] * (m[1][0] * m[2][1] - m[2][0] * m[1][1]);

        // Se o determinante for zero, a matriz não tem inversa
        if (fabs(det) < 1e-6) { // Usamos um pequeno epsilon para lidar com números de ponto flutuante
            qDebug() << "Determinante zero ou muito pequeno, matriz não inversível! Retornando identidade.";
            inversa.setIdentidade(); // Retorna identidade como fallback
            return inversa;
        }

        float invDet = 1.0f / det;

        // Calcula a matriz adjunta (cofatores transpostos)
        // e multiplica pelo inverso do determinante para obter a inversa
        inversa.m[0][0] = (m[1][1] * m[2][2] - m[2][1] * m[1][2]) * invDet;
        inversa.m[0][1] = (m[0][1] * m[2][2] - m[2][1] * m[0][2]) * -invDet; // Note o sinal negativo
        inversa.m[0][2] = (m[0][1] * m[1][2] - m[1][1] * m[0][2]) * invDet;

        inversa.m[1][0] = (m[1][0] * m[2][2] - m[2][0] * m[1][2]) * -invDet; // Note o sinal negativo
        inversa.m[1][1] = (m[0][0] * m[2][2] - m[2][0] * m[0][2]) * invDet;
        inversa.m[1][2] = (m[0][0] * m[1][2] - m[1][0] * m[0][2]) * -invDet; // Note o sinal negativo

        inversa.m[2][0] = (m[1][0] * m[2][1] - m[2][0] * m[1][1]) * invDet;
        inversa.m[2][1] = (m[0][0] * m[2][1] - m[2][0] * m[0][1]) * -invDet; // Note o sinal negativo
        inversa.m[2][2] = (m[0][0] * m[1][1] - m[1][0] * m[0][1]) * invDet;

        return inversa;
    }
};

class ObjetoGeometrico
{
public:
    ObjetoGeometrico(const QString& nome, TipoObjeto tipo);
    virtual ~ObjetoGeometrico() {}

    virtual void desenhar(QPainter& pintar, const Matriz3x3& matrizVisualizacao) = 0;
    virtual QPointF getCentroGeometrico() const = 0;

    // CORREÇÃO: addPonto agora aceita QPointF
    void addPonto(const QPointF& p);
    QString getNome() const;
    TipoObjeto getTipo() const;
    // CORREÇÃO: Retorna uma lista de QPointF
    const QList<QPointF>& getPontos() const;

    void selecionar();
    void deselecionar();
    bool isSelecionado() const { return selecionado; }

    QPointF getTotalTranslacao() const;
    QPointF getTotalEscala() const;
    double getTotalRotacao() const;

    void transladar(double dx, double dy);
    void escalar(double sx, double sy);
    void rotacionar(double angulo, const QPointF& pivo);

protected:
    QString nome;
    TipoObjeto tipo;
    QList<QPointF> pontos;
    Matriz3x3 m_transform;
    QPointF m_total_translacao = QPointF(0, 0);
    QPointF m_total_escala = QPointF(1.0, 1.0);
    double m_total_rotacao = 0.0;
    bool selecionado = false;
};


class Ponto : public ObjetoGeometrico {
public:
    Ponto(const QString& nome, const QPointF& p);
    void desenhar(QPainter& pintar, const Matriz3x3& matrizVisualizacao) override;
    QPointF getCentroGeometrico() const override;
};

class Reta : public ObjetoGeometrico {
public:

    Reta(const QString& nome, const QPointF& p1, const QPointF& p2);
    void desenhar(QPainter& pintar, const Matriz3x3& matrizVisualizacao) override;
    QPointF getCentroGeometrico() const override;
};

class Poligono : public ObjetoGeometrico {
public:
    Poligono(const QString& nome);
    void finalizar();
    bool isFinalizado() const { return finalizado; }
    void desenhar(QPainter& pintar, const Matriz3x3& matrizVisualizacao) override;
    QPointF getCentroGeometrico() const override;

private:
    bool finalizado;
};

#endif // OBJETO_H
