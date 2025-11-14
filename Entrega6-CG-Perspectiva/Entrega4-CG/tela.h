#ifndef TELA_H
#define TELA_H

#include <QFrame>
#include <QMouseEvent>
#include "Objeto.h"

// Enum para controlar o tipo de projeção
enum ProjecaoTipo {
    ORTOGONAL,
    PERSPECTIVA
};

class Tela : public QFrame
{
    Q_OBJECT

public:
    explicit Tela(QWidget *parent = nullptr);

    void DisplayFile(QList<ObjetoGeometrico*> *file);
    void setObjetoAtual(ObjetoGeometrico *obj);

    // Câmera 3D
    void setVisualizacao(const Ponto4D& centro, double zoom, double rotX, double rotY, double rotZ, ProjecaoTipo tipo, double zNear);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    QList<ObjetoGeometrico*> *displayFile = nullptr;
    ObjetoGeometrico *objetoAtual = nullptr;

    Ponto4D m_wc {0, 0, 0};
    double m_zoom {500.0};
    double m_rotX {0.0};
    double m_rotY {0.0};
    double m_rotZ {0.0};

    //Variáveis para guardar o estado da projeção
    ProjecaoTipo m_projecao {ORTOGONAL};
    double m_distanciaProjecao {100.0}; //Este é o 'zNear' ou 'd'

    Matriz4x4 m_matrizInversaTotal;

signals:
    void objetoModificado();

};

#endif // TELA_H
