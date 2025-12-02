//Murilo Albuquerque 2510154
//Julia Yukari   2422654
//Marcus Vinicius 2524651


//Aproveitar que esse é a classe principal para dar detalhes da entrega 7!
//Foi adicionado o clipping volumetrico, ilumionaçao, algoritmo do pintor, mexer a camera com o mouse, sombras (junto com o chao), renderizacao hibrida e curvas
//Nao sei se vai aparecer tudo na apresentaçao, mas esta registrado!


//Lembrando que se adicionar poligono ou reta no ortogonal ele ficará diferente da perspectiva!
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tela.h"
#include <QColor>
#include <QPixmap>
#include <QKeyEvent>
#include <QFileDialog>
#include <QTextStream>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow){
    ui->setupUi(this);

    //Conecta a Tela
    ui->Frame->DisplayFile(&displayFile);
    connect(ui->Frame, &Tela::objetoModificado, this, &MainWindow::atualizarUIAposModificacao);

    //CONECTA SINAL DE ROTAÇÃO
    connect(ui->Frame, &Tela::cameraRotacionada, this, [=](double rx, double ry, double rz){
        //Bloqueia sinais
        ui->caixa_rot_cam_x->blockSignals(true);
        ui->caixa_rot_cam_y->blockSignals(true);
        ui->caixa_rot_cam_z->blockSignals(true);

        ui->caixa_rot_cam_x->setValue(rx);
        ui->caixa_rot_cam_y->setValue(ry);
        ui->caixa_rot_cam_z->setValue(rz);

        //Desbloqueia
        ui->caixa_rot_cam_x->blockSignals(false);
        ui->caixa_rot_cam_y->blockSignals(false);
        ui->caixa_rot_cam_z->blockSignals(false);
    });

    connect(ui->combo_projecao, QOverload<int>::of(&QComboBox::currentIndexChanged),this, &MainWindow::on_combo_projecao_currentIndexChanged);


    connect(ui->caixa_zNear, QOverload<double>::of(&QDoubleSpinBox::valueChanged),this, &MainWindow::on_btn_atualizar_visualizacao_clicked);

    listaDeCores << Qt::cyan << Qt::magenta << Qt::green << Qt::red << Qt::lightGray << QColor(255, 165, 0) << Qt::blue;
    indiceCorAtual = 0;

    configOrtogonal.x = 0.0;
    configOrtogonal.y = 0.0;
    configOrtogonal.z = 300.0;
    configOrtogonal.rotX = -16.0;
    configOrtogonal.rotY = -34;
    configOrtogonal.rotZ = 0.0;
    configOrtogonal.zoom = 850.0;

    configPerspectiva.x = 0.0;
    configPerspectiva.y = 0.0;
    configPerspectiva.z = 500.0;
    configPerspectiva.rotX = -16.0;
    configPerspectiva.rotY = -4.0;
    configPerspectiva.rotZ = 0.0;
    configPerspectiva.zoom = 150.0;

    indiceProjecaoAtual = 0;
    ui->combo_projecao->setCurrentIndex(0);
    carregarConfiguracao(0);
    ui->caixa_zNear->setEnabled(false);

    atualizarPainelTransformacao(nullptr);
    on_btn_atualizar_visualizacao_clicked();
}

MainWindow::~MainWindow(){
    qDeleteAll(displayFile);
    displayFile.clear();
    delete ui;
}

void MainWindow::salvarConfiguracaoAtual(int indice) {
    CameraConfig* config;
    //Seleciona qual configuração salvar
    if (indice == 0) {
        config = &configOrtogonal;
    } else {
        config = &configPerspectiva;
    }
    config->x = ui->caixa_wc_x->value();
    config->y = ui->caixa_wc_y->value();
    config->z = ui->caixa_wc_z->value();

    config->rotX = ui->caixa_rot_cam_x->value();
    config->rotY = ui->caixa_rot_cam_y->value();
    config->rotZ = ui->caixa_rot_cam_z->value();

    config->zoom = ui->caixa_zoom->value();
}

void MainWindow::carregarConfiguracao(int indice) {
    CameraConfig* config;

    //Seleção da configuração
    if (indice == 0) {
        config = &configOrtogonal;
    } else {
        config = &configPerspectiva;
    }
    bool oldStateX = ui->caixa_wc_x->blockSignals(true);
    ui->caixa_wc_y->blockSignals(true);
    ui->caixa_wc_z->blockSignals(true);

    ui->caixa_rot_cam_x->blockSignals(true);
    ui->caixa_rot_cam_y->blockSignals(true);
    ui->caixa_rot_cam_z->blockSignals(true);

    ui->caixa_zoom->blockSignals(true);

    ui->caixa_wc_x->setValue(config->x);
    ui->caixa_wc_y->setValue(config->y);
    ui->caixa_wc_z->setValue(config->z);

    ui->caixa_rot_cam_x->setValue(config->rotX);
    ui->caixa_rot_cam_y->setValue(config->rotY);
    ui->caixa_rot_cam_z->setValue(config->rotZ);

    ui->caixa_zoom->setValue(config->zoom);

    ui->caixa_wc_x->blockSignals(oldStateX);
    ui->caixa_wc_y->blockSignals(oldStateX);
    ui->caixa_wc_z->blockSignals(oldStateX);

    ui->caixa_rot_cam_x->blockSignals(oldStateX);
    ui->caixa_rot_cam_y->blockSignals(oldStateX);
    ui->caixa_rot_cam_z->blockSignals(oldStateX);

    ui->caixa_zoom->blockSignals(oldStateX);
}

void MainWindow::on_combo_projecao_currentIndexChanged(int index) {
    salvarConfiguracaoAtual(indiceProjecaoAtual);
    indiceProjecaoAtual = index;
    carregarConfiguracao(index);
    if (index == 0) ui->caixa_zNear->setEnabled(false);
    else {
        ui->caixa_zNear->setEnabled(true);
        if(ui->caixa_zNear->value() < 1.0) ui->caixa_zNear->setValue(100.0);
    }
    on_btn_atualizar_visualizacao_clicked();
}

void MainWindow::resetarPaineisDeVisualizacao(){
    configOrtogonal = {0.0, 0.0, 300.0, -16.0, -34.0, 0.0, 850.0};
    configPerspectiva = {0.0, 0.0, 500.0, -16.0, -4.0, 0.0, 150.0};
    ui->combo_projecao->blockSignals(true);
    ui->combo_projecao->setCurrentIndex(0);
    ui->combo_projecao->blockSignals(false);
    indiceProjecaoAtual = 0;
    carregarConfiguracao(0);
    ui->caixa_zNear->setValue(100.0); ui->caixa_zNear->setEnabled(false);
}

QColor MainWindow::getProximaCor(){
    QColor cor = listaDeCores[indiceCorAtual];
    indiceCorAtual = (indiceCorAtual + 1) % listaDeCores.size();
    return cor;
}

void MainWindow::on_btn_carregarObj_clicked(){
    QString caminho = QFileDialog::getOpenFileName(this, "Abrir Arquivo .obj", "", "Wavefront OBJ (*.obj)");
    if (caminho.isEmpty())
        return;
    finalizarObjetoAtual();
    ui->listWidget_opcoes->setCurrentRow(-1);
    contadorMalhas++;
    QString nome = QString("Malha_%1").arg(contadorMalhas);
    Malha3D* novaMalha = new Malha3D(nome);
    novaMalha->setCor(getProximaCor());
    if (carregarArquivoObj(caminho, novaMalha)) {
        novaMalha->escalar(100.0, 100.0, 100.0);
        displayFile.append(novaMalha);
        atualizarListaObjetos();
        ui->listWidget_opcoes->setCurrentRow(displayFile.size() - 1);
        ui->Frame->update();
    } else {
        delete novaMalha;
        qDebug() << "Erro ao carregar o arquivo .obj";
    }
}

bool MainWindow::carregarArquivoObj(const QString& caminho, Malha3D* malha){
    QFile file(caminho);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString linha = in.readLine().trimmed();
        if (linha.isEmpty() || linha.startsWith('#'))
            continue;
        QStringList parts = linha.split(" ", Qt::SkipEmptyParts);
        if (parts.isEmpty())
            continue;
        if (parts[0] == "v") {
            if (parts.length() < 4)
                continue;
            malha->addPonto(Ponto4D(parts[1].toFloat(), parts[2].toFloat(), parts[3].toFloat()));
        } else if (parts[0] == "f") {
            if (parts.length() < 4)
                continue;
            int totalVertices = malha->getPontos().size();
            auto getIndex = [&](const QString& part) -> int {
                int index = part.split('/').first().toInt();
                if (index > 0)
                    return index - 1;
                else if (index < 0)
                    return totalVertices + index;
                return 0;
            };
            int v1 = getIndex(parts[1]);
            int v2 = getIndex(parts[2]);
            for (int i = 3; i < parts.length(); ++i) {
                int v3 = getIndex(parts[i]);
                malha->addFace(v1, v2, v3);
                v2 = v3;
            }
        }
    }
    file.close();
    return true;
}

void MainWindow::finalizarObjetoAtual(){
    if (objetoEmConstrucao) {
        if (objetoEmConstrucao->getTipo() == POLIGONO) {
            static_cast<Poligono*>(objetoEmConstrucao)->finalizar();
        }
        objetoEmConstrucao = nullptr;
        ui->Frame->setObjetoAtual(nullptr);
        ui->Frame->update();
        atualizarListaObjetos();
    }
}

void MainWindow::atualizarUIAposModificacao(){
    int Linha_Atual= ui->listWidget_opcoes->currentRow();
    if (Linha_Atual>= 0 && Linha_Atual< displayFile.size()) {
        atualizarListaObjetos();
        atualizarListaDeCoordenadas(displayFile.at(Linha_Atual));
    } else if (objetoEmConstrucao) {
        atualizarListaObjetos();
        atualizarListaDeCoordenadas(objetoEmConstrucao);
    }
}

void MainWindow::on_btn_novoPoligono_clicked(){
    finalizarObjetoAtual();
    ui->listWidget_opcoes->setCurrentRow(-1);
    contadorPoligonos++;
    Poligono *novo = new Poligono(QString("Poligono_%1").arg(contadorPoligonos));
    novo->setCor(getProximaCor());
    novo->escalar(100.0, 100.0, 100.0);
    displayFile.append(novo);
    atualizarListaObjetos();
    ui->listWidget_opcoes->setCurrentRow(displayFile.size() - 1);
    objetoEmConstrucao = novo;
    ui->Frame->setObjetoAtual(objetoEmConstrucao);
    ui->Frame->update();
}

void MainWindow::on_btn_novaCurva_clicked(){
    finalizarObjetoAtual();
    ui->listWidget_opcoes->setCurrentRow(-1);
    contadorCurvas++;
    CurvaBezier *nova = new CurvaBezier(QString("Curva_%1").arg(contadorCurvas));
    nova->setCor(getProximaCor());
    nova->escalar(100.0, 100.0, 100.0);
    displayFile.append(nova);
    atualizarListaObjetos();
    ui->listWidget_opcoes->setCurrentRow(displayFile.size() - 1);
    objetoEmConstrucao = nova;
    ui->Frame->setObjetoAtual(objetoEmConstrucao);
    ui->Frame->update();
}

void MainWindow::on_btn_desenhar_clicked(){
    finalizarObjetoAtual();
    ui->listWidget_opcoes->setCurrentRow(-1);
}

void MainWindow::on_btn_reset_clicked(){
    finalizarObjetoAtual();
    qDeleteAll(displayFile);
    displayFile.clear();
    contadorPoligonos = 0;
    contadorCurvas = 0;
    contadorMalhas = 0;
    indiceCorAtual = 0;
    ui->listWidget_opcoes->clear();
    ui->listWidget_coordenadas->clear();
    resetarPaineisDeVisualizacao();
    on_btn_atualizar_visualizacao_clicked();
}

void MainWindow::on_listWidget_opcoes_currentRowChanged(int Linha_Atual){
    for (auto obj : displayFile) obj->deselecionar();
    ObjetoGeometrico *sel = nullptr;
    if(Linha_Atual>= 0 && Linha_Atual< displayFile.size()){
        sel = displayFile.at(Linha_Atual);
        sel->selecionar();
        if(sel->getTipo() == POLIGONO || sel->getTipo() == CURVA_BEZIER) {
            objetoEmConstrucao = sel;
            ui->Frame->setObjetoAtual(objetoEmConstrucao);
        } else finalizarObjetoAtual();
    } else finalizarObjetoAtual();
    atualizarPainelTransformacao(sel);
    atualizarListaDeCoordenadas(sel);
    ui->Frame->update();
}

void MainWindow::on_btn_atualizar_visualizacao_clicked(){
    ui->Frame->setVisualizacao(
        Ponto4D(ui->caixa_wc_x->value(), ui->caixa_wc_y->value(), ui->caixa_wc_z->value()), ui->caixa_zoom->value(), ui->caixa_rot_cam_x->value(), ui->caixa_rot_cam_y->value(), ui->caixa_rot_cam_z->value(),(ui->combo_projecao->currentIndex() == 0) ? ORTOGONAL : PERSPECTIVA,ui->caixa_zNear->value());
}

void MainWindow::atualizarListaObjetos(){
    int row = ui->listWidget_opcoes->currentRow();
    ui->listWidget_opcoes->clear();
    for (auto obj : displayFile) {
        QListWidgetItem *item = new QListWidgetItem();
        Ponto4D c = obj->getCentroGeometrico();
        item->setText(QString("%1: (%2, %3, %4)").arg(obj->getNome()).arg(c.x,0,'f',2).arg(c.y,0,'f',2).arg(c.z,0,'f',2));
        QPixmap pix(16, 16); pix.fill(obj->getCor());
        item->setIcon(QIcon(pix));
        ui->listWidget_opcoes->addItem(item);
    }
    if (row >= 0 && row < displayFile.size()) ui->listWidget_opcoes->setCurrentRow(row);
}

void MainWindow::atualizarListaDeCoordenadas(ObjetoGeometrico* obj){
    ui->listWidget_coordenadas->clear();
    if (!obj)
        return;
    ui->listWidget_coordenadas->addItem(obj->getNome() + ":");
    if(obj->getTipo() == MALHA_3D && obj->getPontos().size() > 100) {
        ui->listWidget_coordenadas->addItem(QString("... (%1 vértices)").arg(obj->getPontos().size()));
        return;
    }
    const auto& pts = obj->getPontos();
    for (int i = 0; i < pts.size(); ++i) {
        ui->listWidget_coordenadas->addItem(QString(" P%1: (%2, %3, %4)").arg(i+1).arg(pts[i].x,0,'f',2).arg(pts[i].y,0,'f',2).arg(pts[i].z,0,'f',2));
    }
}

void MainWindow::on_btn_aplicar_translacao_clicked(){
    int idx = ui->listWidget_opcoes->currentRow();
    if (idx >= 0 && idx < displayFile.size()) {
        auto obj = displayFile.at(idx);
        Ponto4D t = obj->getTotalTranslacao();
        obj->transladar(ui->caixa_trans_dx->value()-t.x, ui->caixa_trans_dy->value()-t.y, ui->caixa_trans_dz->value()-t.z);
        atualizarPainelTransformacao(obj); ui->Frame->update(); atualizarListaObjetos();
    }
}
void MainWindow::on_btn_aplicar_escala_clicked(){
    int idx = ui->listWidget_opcoes->currentRow();
    if (idx >= 0 && idx < displayFile.size()) {
        auto obj = displayFile.at(idx);
        Ponto4D s = obj->getTotalEscala();
        if(s.x!=0 && s.y!=0 && s.z!=0) obj->escalar(ui->caixaDupla_escala_sx->value()/s.x, ui->caixaDupla_escala_sy->value()/s.y, ui->caixaDupla_escala_sz->value()/s.z);
        atualizarPainelTransformacao(obj);
        ui->Frame->update();
        atualizarListaObjetos();
    }
}
void MainWindow::on_btn_aplicar_rotacao_clicked(){
    int idx = ui->listWidget_opcoes->currentRow();
    if (idx >= 0 && idx < displayFile.size()) {
        auto obj = displayFile.at(idx);
        Ponto4D r = obj->getTotalRotacao();
        obj->rotacionarX(ui->caixaDupla_rot_angulo_x->value()-r.x);
        obj->rotacionarY(ui->caixaDupla_rot_angulo_y->value()-r.y);
        obj->rotacionarZ(ui->caixaDupla_rot_angulo_z->value()-r.z);
        atualizarPainelTransformacao(obj); ui->Frame->update();
        atualizarListaObjetos();
    }
}

void MainWindow::atualizarPainelTransformacao(ObjetoGeometrico* obj){
    bool st = (obj != nullptr);
    ui->caixa_trans_dx->setEnabled(st);
    ui->caixa_trans_dy->setEnabled(st);
    ui->caixa_trans_dz->setEnabled(st);
    ui->caixaDupla_escala_sx->setEnabled(st);
    ui->caixaDupla_escala_sy->setEnabled(st);
    ui->caixaDupla_escala_sz->setEnabled(st);
    ui->caixaDupla_rot_angulo_x->setEnabled(st);
    ui->caixaDupla_rot_angulo_y->setEnabled(st);
    ui->caixaDupla_rot_angulo_z->setEnabled(st);
    ui->btn_aplicar_translacao->setEnabled(st);
    ui->btn_aplicar_escala->setEnabled(st);
    ui->btn_aplicar_rotacao->setEnabled(st);

    if (obj) {
        Ponto4D t = obj->getTotalTranslacao(), s = obj->getTotalEscala(), r = obj->getTotalRotacao();
        ui->caixa_trans_dx->setValue(t.x); ui->caixa_trans_dy->setValue(t.y); ui->caixa_trans_dz->setValue(t.z);
        ui->caixaDupla_escala_sx->setValue(s.x); ui->caixaDupla_escala_sy->setValue(s.y); ui->caixaDupla_escala_sz->setValue(s.z);
        ui->caixaDupla_rot_angulo_x->setValue(r.x); ui->caixaDupla_rot_angulo_y->setValue(r.y); ui->caixaDupla_rot_angulo_z->setValue(r.z);
    } else {
        ui->caixa_trans_dx->setValue(0); ui->caixa_trans_dy->setValue(0); ui->caixa_trans_dz->setValue(0);
        ui->caixaDupla_escala_sx->setValue(1); ui->caixaDupla_escala_sy->setValue(1); ui->caixaDupla_escala_sz->setValue(1);
        ui->caixaDupla_rot_angulo_x->setValue(0); ui->caixaDupla_rot_angulo_y->setValue(0); ui->caixaDupla_rot_angulo_z->setValue(0);
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event){
    if (event->key() == Qt::Key_Escape) ui->listWidget_opcoes->setCurrentRow(-1);
    QMainWindow::keyPressEvent(event);
}
