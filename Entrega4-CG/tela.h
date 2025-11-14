#ifndef TELA_H
#define TELA_H

#include <QFrame>
#include <QMouseEvent>
#include "Objeto.h"
// #include "clipping.h" // REMOVIDO

class Tela : public QFrame
{
    Q_OBJECT

public:
    explicit Tela(QWidget *parent = nullptr);

    void DisplayFile(QList<ObjetoGeometrico*> *file);
    void setObjetoAtual(ObjetoGeometrico *obj);

    void setVisualizacao(const Ponto4D& centro, double zoom, double rotX, double rotY, double rotZ);

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

    Matriz4x4 m_matrizInversaTotal;

signals:
    void objetoModificado();

};

#endif // TELA_H
