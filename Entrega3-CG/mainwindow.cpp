//Murilo Albuquerque
//Julia Yukari
//Marcus Vinicius

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tela.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Passa o ponteiro da lista de objetos ('displayFile') para a tela de desenho ('Frame').
    ui->Frame->DisplayFile(&displayFile);

    // Conexão para atualização em tempo real a cada clique na tela
    connect(ui->Frame, &Tela::objetoModificado, this, &MainWindow::atualizarUIAposModificacao);

    // Inicia os painéis da câmera com valores padrão
    resetarPaineisDeVisualizacao();
}

MainWindow::~MainWindow()
{
    qDeleteAll(displayFile);
    displayFile.clear();
    delete ui;
}

void MainWindow::atualizarUIAposModificacao()
{
    if (poligonoEmConstrucao) {
        atualizarListaObjetos();
        atualizarListaDeCoordenadas(poligonoEmConstrucao);
    }
}

void MainWindow::on_btn_novoPoligono_clicked()
{
    poligonoEmConstrucao = nullptr;
    ui->Frame->PoligonoAtual(nullptr);

    contadorPoligonos++;
    QString nomePoligono = QString("Objeto_%1").arg(contadorPoligonos);

    ui->listWidget_opcoes->addItem(nomePoligono);

    Poligono *novoPoligono = new Poligono(nomePoligono);
    displayFile.append(novoPoligono);

    ui->listWidget_opcoes->setCurrentRow(displayFile.size() - 1);

    this->poligonoEmConstrucao = novoPoligono;
    ui->Frame->PoligonoAtual(this->poligonoEmConstrucao);

    ui->Frame->update();
    atualizarListaObjetos();
}

void MainWindow::on_btn_desenhar_clicked()
{
    if (poligonoEmConstrucao) {
        poligonoEmConstrucao->finalizar();
    }
    poligonoEmConstrucao = nullptr;
    ui->Frame->PoligonoAtual(nullptr);
    ui->Frame->update();
}

void MainWindow::on_btn_reset_clicked()
{
    poligonoEmConstrucao = nullptr;
    ui->Frame->PoligonoAtual(nullptr);

    qDeleteAll(displayFile);
    displayFile.clear();
    contadorPoligonos = 0;
    ui->listWidget_opcoes->clear();
    ui->listWidget_coordenadas->clear();
    resetarPaineisDeVisualizacao();
    on_btn_atualizar_visualizacao_clicked();
    ui->Frame->update();
}

void MainWindow::on_listWidget_opcoes_currentRowChanged(int currentRow)
{
    for (int i = 0; i < displayFile.size(); ++i) {
        displayFile.at(i)->deselecionar();
    }

    ObjetoGeometrico *objetoSelecionado = nullptr;

    if(currentRow >= 0 && currentRow < displayFile.size()){
        objetoSelecionado = displayFile.at(currentRow);
        objetoSelecionado->selecionar();

        if(objetoSelecionado->getTipo() == POLIGONO){
            poligonoEmConstrucao = static_cast<Poligono*>(objetoSelecionado);
            ui->Frame->PoligonoAtual(poligonoEmConstrucao);
        } else {
            poligonoEmConstrucao = nullptr;
            ui->Frame->PoligonoAtual(nullptr);
        }
        atualizarPainelTransformacao(objetoSelecionado);

    } else {
        poligonoEmConstrucao = nullptr;
        ui->Frame->PoligonoAtual(nullptr);
        atualizarPainelTransformacao(nullptr);
    }

    atualizarListaDeCoordenadas(objetoSelecionado);
    ui->Frame->update();
}

void MainWindow::on_btn_atualizar_visualizacao_clicked()
{
    double wc_x = ui->caixa_wc_x->value();
    double wc_y = ui->caixa_wc_y->value();
    double zoom = ui->caixa_zoom->value();
    double angulo = ui->caixa_angulo->value();

    ui->Frame->setVisualizacao(QPointF(wc_x, wc_y), zoom, angulo);
}

void MainWindow::resetarPaineisDeVisualizacao()
{
    ui->caixa_wc_x->setValue(0.0);
    ui->caixa_wc_y->setValue(0.0);
    ui->caixa_zoom->setValue(500.0);
    ui->caixa_angulo->setValue(0.0);
}

void MainWindow::atualizarListaObjetos()
{
    if (!poligonoEmConstrucao) return;
    int indice = displayFile.indexOf(poligonoEmConstrucao);
    if (indice != -1) {
        QListWidgetItem *item = ui->listWidget_opcoes->item(indice);
        ObjetoGeometrico *objeto = displayFile.at(indice);
        QPointF centro = objeto->getCentroGeometrico();
        QString nome = objeto->getNome();
        // Alterado para mostrar 2 casas decimais no centro
        QString textoItem = QString("%1: (%2, %3)").arg(nome).arg(centro.x(), 0, 'f', 2).arg(centro.y(), 0, 'f', 2);
        item->setText(textoItem);
    }
}

void MainWindow::atualizarListaDeCoordenadas(ObjetoGeometrico* objeto)
{
    ui->listWidget_coordenadas->clear();
    if (!objeto) return;
    ui->listWidget_coordenadas->addItem(objeto->getNome() + ":");
    //Pegar QPointF da lista de pontos do objeto
    const QList<QPointF>& pontos = objeto->getPontos();
    for (int i = 0; i < pontos.size(); ++i) {
        const QPointF& p = pontos.at(i);
        //Formatar para exibir precisão de ponto flutuante (2 casas decimais)
        QString textoPonto = QString("  Ponto %1: (%2, %3)").arg(i + 1).arg(p.x(), 0, 'f', 2).arg(p.y(), 0, 'f', 2);
        ui->listWidget_coordenadas->addItem(textoPonto);
    }
}

void MainWindow::on_btn_aplicar_translacao_clicked()
{
    int indice = ui->listWidget_opcoes->currentRow();
    if (indice >= 0 && indice < displayFile.size()) {
        ObjetoGeometrico* objeto = displayFile.at(indice);
        double target_dx = ui->caixa_trans_dx->value();
        double target_dy = ui->caixa_trans_dy->value();
        QPointF current_translacao = objeto->getTotalTranslacao();
        double delta_dx = target_dx - current_translacao.x();
        double delta_dy = target_dy - current_translacao.y();
        objeto->transladar(delta_dx, delta_dy);
        atualizarPainelTransformacao(objeto);
        ui->Frame->update();
    }
}

void MainWindow::on_btn_aplicar_escala_clicked()
{
    int indice = ui->listWidget_opcoes->currentRow();
    if (indice >= 0 && indice < displayFile.size()) {
        ObjetoGeometrico* objeto = displayFile.at(indice);
        double target_sx = ui->caixaDupla_escala_sx->value();
        double target_sy = ui->caixaDupla_escala_sy->value();
        QPointF current_escala = objeto->getTotalEscala();
        if (current_escala.x() == 0 || current_escala.y() == 0)
            return;

        double delta_sx = target_sx / current_escala.x();
        double delta_sy = target_sy / current_escala.y();
        objeto->escalar(delta_sx, delta_sy);
        atualizarPainelTransformacao(objeto);
        ui->Frame->update();
    }
}

void MainWindow::on_btn_aplicar_rotacao_clicked()
{
    int indice = ui->listWidget_opcoes->currentRow();
    if (indice >= 0 && indice < displayFile.size()) {
        ObjetoGeometrico* objeto = displayFile.at(indice);
        double target_angulo = ui->caixaDupla_rot_angulo->value();
        double pivoX = ui->caixa_rot_pivo_x->value();
        double pivoY = ui->caixa_rot_pivo_y->value();
        double current_angulo = objeto->getTotalRotacao();
        QPointF pivo(pivoX, pivoY);
        double delta_angulo = target_angulo - current_angulo;
        objeto->rotacionar(delta_angulo, pivo);
        atualizarPainelTransformacao(objeto);
        ui->Frame->update();
    }
}

void MainWindow::atualizarPainelTransformacao(ObjetoGeometrico* objeto)
{
    if (!objeto) {
        ui->caixa_trans_dx->setValue(0);
        ui->caixa_trans_dy->setValue(0);
        ui->caixaDupla_escala_sx->setValue(1.0);
        ui->caixaDupla_escala_sy->setValue(1.0);
        ui->caixaDupla_rot_angulo->setValue(0);
        return;
    }
    QPointF translacao = objeto->getTotalTranslacao();
    QPointF escala = objeto->getTotalEscala();
    double rotacao = objeto->getTotalRotacao();
    ui->caixa_trans_dx->setValue(translacao.x());
    ui->caixa_trans_dy->setValue(translacao.y());
    ui->caixaDupla_escala_sx->setValue(escala.x());
    ui->caixaDupla_escala_sy->setValue(escala.y());
    ui->caixaDupla_rot_angulo->setValue(rotacao);
}
