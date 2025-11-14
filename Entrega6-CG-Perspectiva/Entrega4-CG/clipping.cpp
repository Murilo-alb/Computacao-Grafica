#include "clipping.h"
#include <cmath>

Clipping::Clipping(const QRectF& limites) : m_limites(limites) {
}

int Clipping::getCodigoRegiao(const QPointF& p) const {
    int codigo = DENTRO_2D;

    if (p.x() < m_limites.left())
        codigo |= ESQUERDA_2D;
    else if (p.x() > m_limites.right())
        codigo |= DIREITA_2D;

    if (p.y() < m_limites.top()) // Y cresce para baixo no Qt
        codigo |= CIMA_2D;
    else if (p.y() > m_limites.bottom())
        codigo |= BAIXO_2D;

    return codigo;
}

bool Clipping::clipLinha(QPointF& p1, QPointF& p2) const {
    int codigo1 = getCodigoRegiao(p1);
    int codigo2 = getCodigoRegiao(p2);
    bool aceito = false;

    while (true) {
        if ((codigo1 | codigo2) == 0) {
            aceito = true;
            break;
        } else if ((codigo1 & codigo2) != 0) {
            break;
        } else {
            int codigoFora = (codigo1 != 0) ? codigo1 : codigo2;
            double x, y;
            const float EPSILON = 1e-6; // Evitar divisão por zero

            if (codigoFora & CIMA_2D) {
                if (fabs(p2.y() - p1.y()) > EPSILON)
                    x = p1.x() + (p2.x() - p1.x()) * (m_limites.top() - p1.y()) / (p2.y() - p1.y());
                else
                    x = p1.x();
                y = m_limites.top();
            } else if (codigoFora & BAIXO_2D) {
                if (fabs(p2.y() - p1.y()) > EPSILON)
                    x = p1.x() + (p2.x() - p1.x()) * (m_limites.bottom() - p1.y()) / (p2.y() - p1.y());
                else
                    x = p1.x();
                y = m_limites.bottom();
            } else if (codigoFora & DIREITA_2D) {
                if (fabs(p2.x() - p1.x()) > EPSILON)
                    y = p1.y() + (p2.y() - p1.y()) * (m_limites.right() - p1.x()) / (p2.x() - p1.x());
                else
                    y = p1.y(); // Linha vertical
                x = m_limites.right();
            } else if (codigoFora & ESQUERDA_2D) {
                if (fabs(p2.x() - p1.x()) > EPSILON)
                    y = p1.y() + (p2.y() - p1.y()) * (m_limites.left() - p1.x()) / (p2.x() - p1.x());
                else
                    y = p1.y();
                x = m_limites.left();
            }

            if (codigoFora == codigo1) {
                p1.setX(x);
                p1.setY(y);
                codigo1 = getCodigoRegiao(p1);
            } else {
                p2.setX(x);
                p2.setY(y);
                codigo2 = getCodigoRegiao(p2);
            }
        }
    }
    return aceito;
}
