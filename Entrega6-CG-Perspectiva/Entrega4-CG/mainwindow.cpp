//Murilo Albuquerque   2510154
//Julia Yukari         2422654
//Marcus Vinicius      2524651

//Fiz essa versão porque foi a que fez mais sentido para mim
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tela.h"
#include <QColor>
#include <QPixmap>
#include <QKeyEvent>

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow){
    ui->setupUi(this);
    ui->Frame->DisplayFile(&displayFile);
    connect(ui->Frame, &Tela::objetoModificado, this, &MainWindow::atualizarUIAposModificacao);

    //Conecta os novos widgets da UI (que você adicionou no Designer)
    connect(ui->combo_projecao, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::on_btn_atualizar_visualizacao_clicked);
    connect(ui->caixa_zNear, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::on_btn_atualizar_visualizacao_clicked);

    listaDeCores << Qt::cyan << Qt::magenta << Qt::green << Qt::red << Qt::lightGray << QColor(255, 165, 0) /*Laranja*/ << Qt::blue;
    indiceCorAtual = 0;

    resetarPaineisDeVisualizacao();
    atualizarPainelTransformacao(nullptr);

    on_btn_atualizar_visualizacao_clicked();
}

MainWindow::~MainWindow(){
    qDeleteAll(displayFile);
    displayFile.clear();
    delete ui;
}

QColor MainWindow::getProximaCor(){
    QColor cor = listaDeCores[indiceCorAtual];
    indiceCorAtual = (indiceCorAtual + 1) % listaDeCores.size();
    return cor;
}

void MainWindow::on_btn_carregarObj_clicked(){
    QString caminho = QFileDialog::getOpenFileName(this, "Abrir Arquivo .obj", "", "Wavefront OBJ (*.obj)");
    if (caminho.isEmpty()) return;

    finalizarObjetoAtual();
    ui->listWidget_opcoes->setCurrentRow(-1);

    contadorMalhas++;
    QString nome = QString("Malha_%1").arg(contadorMalhas);
    Malha3D* novaMalha = new Malha3D(nome);
    novaMalha->setCor(getProximaCor());

    if (carregarArquivoObj(caminho, novaMalha)) {
        displayFile.append(novaMalha);
        atualizarListaObjetos();
        ui->listWidget_opcoes->setCurrentRow(displayFile.size() - 1);
        ui->Frame->update();
    } else {
        delete novaMalha;
        qDebug() << "Erro ao carregar o arquivo .obj";
    }
}

//Parser 3D (Lê e armazena X, Y, Z)
bool MainWindow::carregarArquivoObj(const QString& caminho, Malha3D* malha){
    QFile file(caminho);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "Não foi possível abrir o arquivo:" << caminho;
        return false;
    }

    QTextStream in(&file);

    while (!in.atEnd()) {
        QString linha = in.readLine().trimmed();
        if (linha.isEmpty() || linha.startsWith('#')) continue;

        QStringList parts = linha.split(" ", Qt::SkipEmptyParts);
        if (parts.isEmpty()) continue;

        QString tipo = parts[0];

        if (tipo == "v") {
            if (parts.length() < 4) continue;
            float x = parts[1].toFloat();
            float y = parts[2].toFloat();
            float z = parts[3].toFloat();
            malha->addPonto(Ponto4D(x, y, z));
        }
        else if (tipo == "f") {
            if (parts.length() < 4) continue;

            int totalVertices = malha->getPontos().size();

            auto getIndex = [&](const QString& part) -> int {
                int index = part.split('/').first().toInt();
                if (index > 0) {
                    return index - 1;
                } else if (index < 0) {
                    return totalVertices + index;
                }
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
    qDebug() << "Carregou" << malha->getPontos().size() << "vértices 3D.";
    return true;
}

void MainWindow::finalizarObjetoAtual(){
    if (objetoEmConstrucao) {
        if (objetoEmConstrucao->getTipo() == POLIGONO) {
            Poligono* poli = static_cast<Poligono*>(objetoEmConstrucao);
            if (!poli->isFinalizado()) {
                poli->finalizar();
            }
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
        ObjetoGeometrico* obj = displayFile.at(Linha_Atual);
        atualizarListaObjetos();
        atualizarListaDeCoordenadas(obj);
    } else if (objetoEmConstrucao) {
        atualizarListaObjetos();
        atualizarListaDeCoordenadas(objetoEmConstrucao);
    }
}
void MainWindow::on_btn_novoPoligono_clicked(){
    finalizarObjetoAtual();
    ui->listWidget_opcoes->setCurrentRow(-1);
    contadorPoligonos++;
    QString nomePoligono = QString("Poligono_%1").arg(contadorPoligonos);
    Poligono *novoPoligono = new Poligono(nomePoligono);
    novoPoligono->setCor(getProximaCor());
    displayFile.append(novoPoligono);
    atualizarListaObjetos();
    ui->listWidget_opcoes->setCurrentRow(displayFile.size() - 1);
    this->objetoEmConstrucao = novoPoligono;
    ui->Frame->setObjetoAtual(this->objetoEmConstrucao);
    ui->Frame->update();
}
void MainWindow::on_btn_novaCurva_clicked(){
    finalizarObjetoAtual();
    ui->listWidget_opcoes->setCurrentRow(-1);
    contadorCurvas++;
    QString nomeCurva = QString("Curva_%1").arg(contadorCurvas);
    CurvaBezier *novaCurva = new CurvaBezier(nomeCurva);
    novaCurva->setCor(getProximaCor());
    displayFile.append(novaCurva);
    atualizarListaObjetos();
    ui->listWidget_opcoes->setCurrentRow(displayFile.size() - 1);
    this->objetoEmConstrucao = novaCurva;
    ui->Frame->setObjetoAtual(this->objetoEmConstrucao);
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
    atualizarListaObjetos();

    ui->listWidget_coordenadas->clear();
    resetarPaineisDeVisualizacao();
    on_btn_atualizar_visualizacao_clicked();
    ui->Frame->update();
}
void MainWindow::on_listWidget_opcoes_currentRowChanged(int Linha_Atual){
    for (int i = 0; i < displayFile.size(); ++i) {
        displayFile.at(i)->deselecionar();
    }
    ObjetoGeometrico *objetoSelecionado = nullptr;
    if(Linha_Atual>= 0 && Linha_Atual< displayFile.size()){
        objetoSelecionado = displayFile.at(Linha_Atual);
        objetoSelecionado->selecionar();

        if(objetoSelecionado->getTipo() == POLIGONO || objetoSelecionado->getTipo() == CURVA_BEZIER) {
            objetoEmConstrucao = objetoSelecionado;
            ui->Frame->setObjetoAtual(objetoEmConstrucao);
        } else {
            finalizarObjetoAtual();
        }
    } else {
        finalizarObjetoAtual();
    }
    atualizarPainelTransformacao(objetoSelecionado);
    atualizarListaDeCoordenadas(objetoSelecionado);
    ui->Frame->update();
}

void MainWindow::on_btn_atualizar_visualizacao_clicked(){
    double wc_x = ui->caixa_wc_x->value();
    double wc_y = ui->caixa_wc_y->value();
    double wc_z = ui->caixa_wc_z->value();
    double zoom = ui->caixa_zoom->value();
    double rotX = ui->caixa_rot_cam_x->value();
    double rotY = ui->caixa_rot_cam_y->value();
    double rotZ = ui->caixa_rot_cam_z->value();

    //Pega os valores dos novos widgets da UI
    ProjecaoTipo tipo;
    if (ui->combo_projecao->currentIndex() == 0) {
        tipo = ORTOGONAL;
    } else {
        tipo = PERSPECTIVA;
    }
    double zNear = ui->caixa_zNear->value();

    if (tipo == ORTOGONAL) {
        ui->caixa_zNear->setEnabled(false);
    } else {
        ui->caixa_zNear->setEnabled(true);
    }

    ui->Frame->setVisualizacao(Ponto4D(wc_x, wc_y, wc_z), zoom, rotX, rotY, rotZ, tipo, zNear);
}

void MainWindow::resetarPaineisDeVisualizacao(){
    ui->caixa_wc_x->setValue(0.0);
    ui->caixa_wc_y->setValue(0.0);
    //Câmera Z=500 (para ver objetos em Z=0)
    ui->caixa_wc_z->setValue(500.0);

    ui->caixa_zoom->setValue(500.0);
    ui->caixa_rot_cam_x->setValue(0.0);
    ui->caixa_rot_cam_y->setValue(0.0);
    ui->caixa_rot_cam_z->setValue(0.0);

    //Reseta os novos widgets da UI
    ui->combo_projecao->setCurrentIndex(0); //0 = Ortogonal
    ui->caixa_zNear->setValue(100.0);
    ui->caixa_zNear->setEnabled(false);
}

void MainWindow::atualizarListaObjetos(){
    int linhaAtual = ui->listWidget_opcoes->currentRow();

    ui->listWidget_opcoes->clear();

    for (int i = 0; i < displayFile.size(); ++i) {
        ObjetoGeometrico *objeto = displayFile.at(i);
        QListWidgetItem *item = new QListWidgetItem();

        Ponto4D centro = objeto->getCentroGeometrico();
        QString nome = objeto->getNome();
        QString textoItem = QString("%1: (%2, %3, %4)").arg(nome).arg(centro.x, 0, 'f', 2).arg(centro.y, 0, 'f', 2).arg(centro.z, 0, 'f', 2);
        item->setText(textoItem);

        QPixmap pixelmap(16, 16);
        pixelmap.fill(objeto->getCor());
        item->setIcon(QIcon(pixelmap));

        ui->listWidget_opcoes->addItem(item);
    }

    if (linhaAtual >= 0 && linhaAtual < displayFile.size()) {
        ui->listWidget_opcoes->setCurrentRow(linhaAtual);
    }
}
void MainWindow::atualizarListaDeCoordenadas(ObjetoGeometrico* objeto){
    ui->listWidget_coordenadas->clear();
    if (!objeto)
        return;
    ui->listWidget_coordenadas->addItem(objeto->getNome() + ":");

    if(objeto->getTipo() == MALHA_3D && objeto->getPontos().size() > 100) {
        ui->listWidget_coordenadas->addItem(QString("  ... (Malha com %1 vértices)").arg(objeto->getPontos().size()));
        return;
    }
    const QList<Ponto4D>& pontos = objeto->getPontos();
    for (int i = 0; i < pontos.size(); ++i) {
        const Ponto4D& p = pontos.at(i);
        QString textoPonto = QString("  Ponto %1: (%2, %3, %4)").arg(i + 1).arg(p.x, 0, 'f', 2).arg(p.y, 0, 'f', 2).arg(p.z, 0, 'f', 2);
        ui->listWidget_coordenadas->addItem(textoPonto);
    }
}

void MainWindow::on_btn_aplicar_translacao_clicked(){
    int indice = ui->listWidget_opcoes->currentRow();
    if (indice >= 0 && indice < displayFile.size()) {
        ObjetoGeometrico* objeto = displayFile.at(indice);
        double target_dx = ui->caixa_trans_dx->value();
        double target_dy = ui->caixa_trans_dy->value();
        double target_dz = ui->caixa_trans_dz->value();
        Ponto4D current_translacao = objeto->getTotalTranslacao();
        double delta_dx = target_dx - current_translacao.x;
        double delta_dy = target_dy - current_translacao.y;
        double delta_dz = target_dz - current_translacao.z;
        objeto->transladar(delta_dx, delta_dy, delta_dz);
        atualizarPainelTransformacao(objeto);
        ui->Frame->update();
        atualizarListaObjetos();
    }
}
void MainWindow::on_btn_aplicar_escala_clicked(){
    int indice = ui->listWidget_opcoes->currentRow();
    if (indice >= 0 && indice < displayFile.size()) {
        ObjetoGeometrico* objeto = displayFile.at(indice);

        double target_sx = ui->caixaDupla_escala_sx->value();
        double target_sy = ui->caixaDupla_escala_sy->value();
        double target_sz = ui->caixaDupla_escala_sz->value();
        Ponto4D current_escala = objeto->getTotalEscala();
        if (current_escala.x == 0 || current_escala.y == 0 || current_escala.z == 0)
            return;
        double delta_sx = target_sx / current_escala.x;
        double delta_sy = target_sy / current_escala.y;
        double delta_sz = target_sz / current_escala.z;
        objeto->escalar(delta_sx, delta_sy, delta_sz);
        atualizarPainelTransformacao(objeto);
        ui->Frame->update();
        atualizarListaObjetos();
    }
}
void MainWindow::on_btn_aplicar_rotacao_clicked(){
    int indice = ui->listWidget_opcoes->currentRow();
    if (indice >= 0 && indice < displayFile.size()) {
        ObjetoGeometrico* objeto = displayFile.at(indice);
        double target_rotX = ui->caixaDupla_rot_angulo_x->value();
        double target_rotY = ui->caixaDupla_rot_angulo_y->value();
        double target_rotZ = ui->caixaDupla_rot_angulo_z->value();
        Ponto4D current_rotacao = objeto->getTotalRotacao();
        double delta_rotX = target_rotX - current_rotacao.x;
        double delta_rotY = target_rotY - current_rotacao.y;
        double delta_rotZ = target_rotZ - current_rotacao.z;
        objeto->rotacionarX(delta_rotX);
        objeto->rotacionarY(delta_rotY);
        objeto->rotacionarZ(delta_rotZ);
        atualizarPainelTransformacao(objeto);
        ui->Frame->update();
        atualizarListaObjetos();
    }
}

void MainWindow::atualizarPainelTransformacao(ObjetoGeometrico* objeto){
    //Se o objeto for nulo
    if (!objeto) {
        ui->caixa_trans_dx->setValue(0); ui->caixa_trans_dx->setEnabled(false);
        ui->caixa_trans_dy->setValue(0); ui->caixa_trans_dy->setEnabled(false);
        ui->caixa_trans_dz->setValue(0); ui->caixa_trans_dz->setEnabled(false);
        ui->caixaDupla_escala_sx->setValue(1.0); ui->caixaDupla_escala_sx->setEnabled(false);
        ui->caixaDupla_escala_sy->setValue(1.0); ui->caixaDupla_escala_sy->setEnabled(false);
        ui->caixaDupla_escala_sz->setValue(1.0); ui->caixaDupla_escala_sz->setEnabled(false);
        ui->caixaDupla_rot_angulo_x->setValue(0); ui->caixaDupla_rot_angulo_x->setEnabled(false);

        ui->caixaDupla_rot_angulo_y->setValue(0); ui->caixaDupla_rot_angulo_y->setEnabled(false);

        ui->caixaDupla_rot_angulo_z->setValue(0); ui->caixaDupla_rot_angulo_z->setEnabled(false);
        ui->btn_aplicar_translacao->setEnabled(false);
        ui->btn_aplicar_escala->setEnabled(false);
        ui->btn_aplicar_rotacao->setEnabled(false);
        return;
    }

    //Se for um objeto normal, ativa os painéis e preenche
    ui->caixa_trans_dx->setEnabled(true);
    ui->caixa_trans_dy->setEnabled(true);
    ui->caixa_trans_dz->setEnabled(true);
    ui->caixaDupla_escala_sx->setEnabled(true);
    ui->caixaDupla_escala_sy->setEnabled(true);
    ui->caixaDupla_escala_sz->setEnabled(true);
    ui->caixaDupla_rot_angulo_x->setEnabled(true);
    ui->caixaDupla_rot_angulo_y->setEnabled(true);
    ui->caixaDupla_rot_angulo_z->setEnabled(true);

    ui->btn_aplicar_translacao->setEnabled(true);
    ui->btn_aplicar_escala->setEnabled(true);
    ui->btn_aplicar_rotacao->setEnabled(true);

    Ponto4D translacao = objeto->getTotalTranslacao();
    Ponto4D escala = objeto->getTotalEscala();
    Ponto4D rotacao = objeto->getTotalRotacao();
    ui->caixa_trans_dx->setValue(translacao.x);
    ui->caixa_trans_dy->setValue(translacao.y);
    ui->caixa_trans_dz->setValue(translacao.z);
    ui->caixaDupla_escala_sx->setValue(escala.x);
    ui->caixaDupla_escala_sy->setValue(escala.y);
    ui->caixaDupla_escala_sz->setValue(escala.z);
    ui->caixaDupla_rot_angulo_x->setValue(rotacao.x);
    ui->caixaDupla_rot_angulo_y->setValue(rotacao.y);
    ui->caixaDupla_rot_angulo_z->setValue(rotacao.z);
}

void MainWindow::keyPressEvent(QKeyEvent *event){
    if (event->key() == Qt::Key_Escape) {
        ui->listWidget_opcoes->setCurrentRow(-1);
    }
    QMainWindow::keyPressEvent(event);
}
