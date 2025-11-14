#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QTextStream>
#include <QColor>
#include <QKeyEvent>
#include "Objeto.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void on_btn_novoPoligono_clicked();
    void on_btn_reset_clicked();
    void on_btn_desenhar_clicked();
    void on_listWidget_opcoes_currentRowChanged(int currentRow);
    void atualizarListaObjetos();

    //Slots 3D
    void on_btn_aplicar_translacao_clicked();
    void on_btn_aplicar_escala_clicked();
    void on_btn_aplicar_rotacao_clicked();
    void on_btn_atualizar_visualizacao_clicked();

    void atualizarUIAposModificacao();
    void on_btn_novaCurva_clicked();

    void on_btn_carregarObj_clicked();

private:
    Ui::MainWindow *ui;
    QList<ObjetoGeometrico*> displayFile;
    int contadorPoligonos = 0;
    int contadorCurvas = 0;
    int contadorMalhas = 0;

    ObjetoGeometrico* objetoEmConstrucao = nullptr;

    QList<QColor> listaDeCores;
    int indiceCorAtual = 0;
    QColor getProximaCor();

    void finalizarObjetoAtual();
    void atualizarPainelTransformacao(ObjetoGeometrico* objeto);
    void atualizarListaDeCoordenadas(ObjetoGeometrico* objeto);
    void resetarPaineisDeVisualizacao();

    //Parser 3D
    bool carregarArquivoObj(const QString& caminho, Malha3D* malha);
};

#endif // MAINWINDOW_H
