#ifndef TELA_H
#define TELA_H

#include <QFrame>
#include <QMouseEvent>
#include "Objeto.h" // Objeto.h já inclui nossa Matriz3x3

class Tela : public QFrame
{
    Q_OBJECT

public:
    explicit Tela(QWidget *parent = nullptr);

    void DisplayFile(QList<ObjetoGeometrico*> *file);
    void PoligonoAtual(Poligono *poligono);
    void setVisualizacao(const QPointF& centro, double zoom, double angulo);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    QList<ObjetoGeometrico*> *displayFile = nullptr;
    Poligono *poligonoAtual = nullptr;

    // Parâmetros da Câmera/Visualização
    QPointF m_wc {0, 0};
    double m_zoom {500.0};
    double m_angulo {0.0};

    Matriz3x3 m_matrizInversa;

signals:
    void objetoModificado();

};

#endif // TELA_H
