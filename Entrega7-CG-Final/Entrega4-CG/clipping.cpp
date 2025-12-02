#include "clipping.h"
#include <cmath>
#include <algorithm> //Não sei pq o alerta.

Clipping::Clipping(const QRectF& limites) : m_limites(limites) {

}

//2D
int Clipping::getCodigoRegiao(const QPointF& p) const {
    int codigo = DENTRO_2D;
    const float EPS = 1e-5f;
    if (p.x() < m_limites.left() - EPS) codigo |= ESQUERDA_2D;
    else if (p.x() > m_limites.right() + EPS) codigo |= DIREITA_2D;
    if (p.y() < m_limites.top() - EPS) codigo |= CIMA_2D;
    else if (p.y() > m_limites.bottom() + EPS) codigo |= BAIXO_2D;
    return codigo;
}

bool Clipping::clipLinha(QPointF& p1, QPointF& p2) const {
    int code1 = getCodigoRegiao(p1);
    int code2 = getCodigoRegiao(p2);
    bool aceito = false;
    int iter = 0;

    while (true) {
        //Trava de segurança para evitar loops infinitos por erro de precisão
        if (iter > 10) {
            break;
        }
        iter++;

        if ((code1 | code2) == 0) {
            aceito = true;
            break;
        }
        else if (code1 & code2) {
            break;
        }
        else {
            int codeOut;
            //Escolhe um dos pontos que está fora da janela
            if (code1 != 0) {
                codeOut = code1;
            } else {
                codeOut = code2;
            }

            double x = 0;
            double y = 0;

            //Define os limites da janela
            double xMin = m_limites.left();
            double xMax = m_limites.right();
            double yMin = m_limites.top();
            double yMax = m_limites.bottom();

            double dx = p2.x() - p1.x();
            double dy = p2.y() - p1.y();

            //Calcula o ponto de interseção usando a fórmula da reta
            if (codeOut & CIMA_2D) {
                if (std::abs(dy) < 1e-9) return false; //Evita divisão por zero
                x = p1.x() + dx * (yMin - p1.y()) / dy;
                y = yMin;
            }
            else if (codeOut & BAIXO_2D) {
                if (std::abs(dy) < 1e-9) return false;
                x = p1.x() + dx * (yMax - p1.y()) / dy;
                y = yMax;
            }
            else if (codeOut & DIREITA_2D) {
                if (std::abs(dx) < 1e-9) return false;
                y = p1.y() + dy * (xMax - p1.x()) / dx;
                x = xMax;
            }
            else if (codeOut & ESQUERDA_2D) {
                if (std::abs(dx) < 1e-9) return false;
                y = p1.y() + dy * (xMin - p1.x()) / dx;
                x = xMin;
            }
            //Substitui o ponto que estava fora pelo novo ponto de interseção
            if (codeOut == code1) {
                p1.setX(x);
                p1.setY(y);
                code1 = getCodigoRegiao(p1); //Recalcula o código da nova posição
            } else {
                p2.setX(x);
                p2.setY(y);
                code2 = getCodigoRegiao(p2); //Recalcula o código da nova posição
            }
        }
    }
    return aceito;
}

//Resolvi dividir o 2D e o 3D, achei mais pratico...
int Clipping::getCodigoRegiao3D(const Ponto4D& p) {
    int code = INSIDE_3D;
    const float EPS = 1e-5f;
    if (p.x < -p.w - EPS) code |= LEFT_3D;
    else if (p.x > p.w + EPS) code |= RIGHT_3D;
    if (p.y < -p.w - EPS) code |= BOTTOM_3D;
    else if (p.y > p.w + EPS) code |= TOP_3D;
    if (p.z < -p.w - EPS) code |= NEAR_3D;
    else if (p.z > p.w + EPS) code |= FAR_3D;
    return code;
}

bool Clipping::isPontoVisivel(const Ponto4D& p) {
    return getCodigoRegiao3D(p) == INSIDE_3D;
}

bool Clipping::clipLinha3D(Ponto4D& p1, Ponto4D& p2) {
    int code1 = getCodigoRegiao3D(p1);
    int code2 = getCodigoRegiao3D(p2);
    bool accept = false;
    int iter = 0;

    while (true) {
        //Trava de segurança para evitar loops infinitos
        if (iter > 10) {
            return false;
        }
        iter++;
        if ((code1 | code2) == 0) {
            accept = true;
            break;
        }
        else if ((code1 & code2) != 0) {
            break;
        }
        else {
            int codeOut;
            //Escolhe qual ponto está fora para ser cortado
            if (code1 != 0) {
                codeOut = code1;
            } else {
                codeOut = code2;
            }

            Ponto4D p;
            float t = 0;
            float num = 0;
            float den = 0;

            //Diferenças (Deltas)
            float dx = p2.x - p1.x;
            float dy = p2.y - p1.y;
            float dz = p2.z - p1.z;
            float dw = p2.w - p1.w;

            //Seleciona a fórmula de interseção baseada no plano que foi violado
            //Equações dos planos canônicos: x=w, x=-w, y=w, y=-w, z=w, z=-w
            //ISSO É IMPORTANTE!

            if (codeOut & TOP_3D) {//y > w  ->  y - w = 0
                num = p1.w - p1.y;
                den = dy - dw;
            }
            else if (codeOut & BOTTOM_3D) {//y < -w ->  y + w = 0
                num = -(p1.w + p1.y);
                den = dy + dw;
            }
            else if (codeOut & RIGHT_3D) {//x > w  ->  x - w = 0
                num = p1.w - p1.x;
                den = dx - dw;
            }
            else if (codeOut & LEFT_3D) {//x < -w ->  x + w = 0
                num = -(p1.w + p1.x);
                den = dx + dw;
            }
            else if (codeOut & FAR_3D) {//z > w  ->  z - w = 0
                num = p1.w - p1.z;
                den = dz - dw;
            }
            else if (codeOut & NEAR_3D) {//z < -w ->  z + w = 0
                num = -(p1.w + p1.z);
                den = dz + dw;
            }

            if (std::abs(den) < 1e-9) {
                return false;
            }

            //Calcula o parâmetro t da interseção
            t = num / den;

            //Garante que t fique no intervalo [0, 1]
            if (t < 0.0f) {
                t = 0.0f;
            }
            if (t > 1.0f) {
                t = 1.0f;
            }

            // Calcula as novas coordenadas do ponto de interseção
            p.x = p1.x + t * dx;
            p.y = p1.y + t * dy;
            p.z = p1.z + t * dz;
            p.w = p1.w + t * dw;

            //Substitui o ponto antigo pelo novo e recalcula o código
            if (codeOut == code1) {
                p1 = p;
                code1 = getCodigoRegiao3D(p1);
            } else {
                p2 = p;
                code2 = getCodigoRegiao3D(p2);
            }
        }
    }
    return accept;
}
