#ifndef CLIPPING_H
#define CLIPPING_H

#include <QPointF>
#include <QRectF>

// Códigos de região 2D (Esquerda, Direita, Cima, Baixo)
const int DENTRO_2D = 0; // 0000
const int ESQUERDA_2D = 1; // 0001
const int DIREITA_2D = 2; // 0010
const int BAIXO_2D = 4; // 0100
const int CIMA_2D = 8; // 1000

class Clipping
{
public:
    Clipping(const QRectF& limites);

    // Algoritmo Cohen-Sutherland 2D
    bool clipLinha(QPointF& p1, QPointF& p2) const;

private:
    int getCodigoRegiao(const QPointF& p) const;
    QRectF m_limites;
};

#endif // CLIPPING_H
