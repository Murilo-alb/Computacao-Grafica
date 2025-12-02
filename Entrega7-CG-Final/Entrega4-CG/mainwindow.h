#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QTimer>
#include "Objeto.h"
#include "tela.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}

QT_END_NAMESPACE

//Estrutura para salvar o estado da câmera
struct CameraConfig {
    double x, y, z;
    double rotX, rotY, rotZ;
    double zoom;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void on_btn_carregarObj_clicked();
    void on_btn_novoPoligono_clicked();
    void on_btn_novaCurva_clicked();
    void on_btn_desenhar_clicked();
    void on_btn_reset_clicked();
    void on_listWidget_opcoes_currentRowChanged(int currentRow);
    void on_btn_atualizar_visualizacao_clicked();

    //Transformações
    void on_btn_aplicar_translacao_clicked();
    void on_btn_aplicar_escala_clicked();
    void on_btn_aplicar_rotacao_clicked();

    void atualizarUIAposModificacao();

    //Slot para a troca de projeção
    void on_combo_projecao_currentIndexChanged(int index);

private:
    Ui::MainWindow *ui;

    QList<ObjetoGeometrico*> displayFile;
    ObjetoGeometrico* objetoEmConstrucao = nullptr;

    //Variáveis para cores e contadores
    QList<QColor> listaDeCores;
    int indiceCorAtual;
    int contadorPoligonos = 0;
    int contadorCurvas = 0;
    int contadorMalhas = 0;

    //GERENCIAMENTO DE CÂMERA
    CameraConfig configOrtogonal;
    CameraConfig configPerspectiva;
    int indiceProjecaoAtual = 0; // 0 = Ortogonal, 1 = Perspectiva

    //Funções auxiliares
    void atualizarListaObjetos();
    void atualizarListaDeCoordenadas(ObjetoGeometrico* objeto);
    void atualizarPainelTransformacao(ObjetoGeometrico* objeto);
    void finalizarObjetoAtual();
    bool carregarArquivoObj(const QString& caminho, Malha3D* malha);
    QColor getProximaCor();
    void resetarPaineisDeVisualizacao();

    //Funções para salvar/carregar estado da câmera
    void salvarConfiguracaoAtual(int indice);
    void carregarConfiguracao(int indice);
};
#endif // MAINWINDOW_H
