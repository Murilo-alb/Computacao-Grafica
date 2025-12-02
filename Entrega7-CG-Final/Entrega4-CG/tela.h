#ifndef TELA_H
#define TELA_H

#include <QFrame>
#include <QMouseEvent>
#include "Objeto.h"

enum ProjecaoTipo {
    ORTOGONAL,
    PERSPECTIVA
};

class Tela : public QFrame
{
    Q_OBJECT

public:
    explicit Tela(QWidget *parent = nullptr);
    ~Tela() { delete m_chao; }

    void DisplayFile(QList<ObjetoGeometrico*> *file);
    void setObjetoAtual(ObjetoGeometrico *obj);

    void setVisualizacao(const Ponto4D& centro, double zoom, double rotX, double rotY, double rotZ, ProjecaoTipo tipo, double zNear);

protected:
    void paintEvent(QPaintEvent *event) override;

    // Eventos do Mouse
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QList<ObjetoGeometrico*> *displayFile = nullptr;
    ObjetoGeometrico *objetoAtual = nullptr;

    //Objeto Chão
    Chao* m_chao = nullptr;

    Ponto4D m_wc {0, 0, 0};
    double m_zoom {500.0};
    double m_rotX {0.0};
    double m_rotY {0.0};
    double m_rotZ {0.0};

    ProjecaoTipo m_projecao {ORTOGONAL};
    double m_distanciaProjecao {100.0};

    Matriz4x4 m_matrizInversaTotal;

    //Variáveis de Controle do Mouse
    QPoint m_posMouseAnterior;
    bool m_rotacionando = false;

signals:
    void objetoModificado();
    void cameraRotacionada(double x, double y, double z);
};

#endif // TELA_H
