#ifndef CLIPPING_H
#define CLIPPING_H

#include <QPointF>
#include <QRectF>
#include "Objeto.h"

//Códigos de região 2D
const int DENTRO_2D = 0;
const int ESQUERDA_2D = 1;
const int DIREITA_2D = 2;
const int BAIXO_2D = 4;
const int CIMA_2D = 8;

//Códigos de região 3D
const int INSIDE_3D = 0;
const int LEFT_3D   = 1;
const int RIGHT_3D  = 2;
const int BOTTOM_3D = 4;
const int TOP_3D    = 8;
const int NEAR_3D   = 16;
const int FAR_3D    = 32;

class Clipping
{
public:
    Clipping(const QRectF& limites);

    bool clipLinha(QPointF& p1, QPointF& p2) const;
    static bool clipLinha3D(Ponto4D& p1, Ponto4D& p2);

    //Função para testar apenas um ponto ---
    static bool isPontoVisivel(const Ponto4D& p);

private:
    int getCodigoRegiao(const QPointF& p) const;
    static int getCodigoRegiao3D(const Ponto4D& p);

    QRectF m_limites;
};

#endif // CLIPPING_H
