#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Objeto.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Slots originais
    void on_btn_novoPoligono_clicked();
    void on_btn_reset_clicked();
    void on_btn_desenhar_clicked();
    void on_listWidget_opcoes_currentRowChanged(int currentRow);
    void atualizarListaObjetos();

    // Slots para os botões de transformação de objeto
    void on_btn_aplicar_translacao_clicked();
    void on_btn_aplicar_escala_clicked();
    void on_btn_aplicar_rotacao_clicked();

    // Slot para o novo botão de atualização da visualização (câmera)
    void on_btn_atualizar_visualizacao_clicked();

    // Slot central para atualização da UI em tempo real
    void atualizarUIAposModificacao();

private:
    Ui::MainWindow *ui;
    QList<ObjetoGeometrico*> displayFile;
    int contadorPoligonos = 0;
    Poligono* poligonoEmConstrucao = nullptr;

    // Funções auxiliares privadas
    void atualizarPainelTransformacao(ObjetoGeometrico* objeto);
    void atualizarListaDeCoordenadas(ObjetoGeometrico* objeto);
    void resetarPaineisDeVisualizacao();
};

#endif // MAINWINDOW_H
