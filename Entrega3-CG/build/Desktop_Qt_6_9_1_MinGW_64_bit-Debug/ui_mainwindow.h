/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>
#include <tela.h>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QPushButton *btn_desenhar;
    QPushButton *btn_reset;
    QPushButton *btn_novoPoligono;
    QListWidget *listWidget_opcoes;
    QPushButton *btn_sair;
    Tela *Frame;
    QGroupBox *Translacao;
    QLabel *label;
    QLabel *label_2;
    QSpinBox *caixa_trans_dx;
    QLabel *label_3;
    QSpinBox *caixa_trans_dy;
    QPushButton *btn_aplicar_translacao;
    QGroupBox *Escala;
    QLabel *label_4;
    QLabel *label_5;
    QDoubleSpinBox *caixaDupla_escala_sx;
    QLabel *label_6;
    QDoubleSpinBox *caixaDupla_escala_sy;
    QPushButton *btn_aplicar_escala;
    QGroupBox *Rotacao;
    QLabel *label_7;
    QLabel *label_8;
    QDoubleSpinBox *caixaDupla_rot_angulo;
    QLabel *label_9;
    QSpinBox *caixa_rot_pivo_x;
    QSpinBox *caixa_rot_pivo_y;
    QPushButton *btn_aplicar_rotacao;
    QListWidget *listWidget_coordenadas;
    QGroupBox *ControledeCamera;
    QDoubleSpinBox *caixa_wc_x;
    QDoubleSpinBox *caixa_wc_y;
    QDoubleSpinBox *caixa_zoom;
    QDoubleSpinBox *caixa_angulo;
    QPushButton *btn_atualizar_visualizacao;
    QLabel *label_10;
    QLabel *label_11;
    QLabel *label_12;
    QLabel *label_13;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1405, 771);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        btn_desenhar = new QPushButton(centralwidget);
        btn_desenhar->setObjectName("btn_desenhar");
        btn_desenhar->setGeometry(QRect(100, 210, 141, 61));
        btn_reset = new QPushButton(centralwidget);
        btn_reset->setObjectName("btn_reset");
        btn_reset->setGeometry(QRect(100, 300, 141, 61));
        btn_novoPoligono = new QPushButton(centralwidget);
        btn_novoPoligono->setObjectName("btn_novoPoligono");
        btn_novoPoligono->setGeometry(QRect(100, 120, 141, 61));
        listWidget_opcoes = new QListWidget(centralwidget);
        listWidget_opcoes->setObjectName("listWidget_opcoes");
        listWidget_opcoes->setGeometry(QRect(60, 10, 231, 91));
        btn_sair = new QPushButton(centralwidget);
        btn_sair->setObjectName("btn_sair");
        btn_sair->setGeometry(QRect(100, 390, 141, 61));
        Frame = new Tela(centralwidget);
        Frame->setObjectName("Frame");
        Frame->setGeometry(QRect(460, 20, 681, 591));
        Frame->setFrameShape(QFrame::Shape::Box);
        Translacao = new QGroupBox(centralwidget);
        Translacao->setObjectName("Translacao");
        Translacao->setGeometry(QRect(300, 10, 151, 181));
        label = new QLabel(Translacao);
        label->setObjectName("label");
        label->setGeometry(QRect(10, 20, 111, 41));
        QFont font;
        font.setBold(true);
        font.setUnderline(false);
        label->setFont(font);
        label->setTextFormat(Qt::TextFormat::PlainText);
        label_2 = new QLabel(Translacao);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(10, 60, 41, 21));
        caixa_trans_dx = new QSpinBox(Translacao);
        caixa_trans_dx->setObjectName("caixa_trans_dx");
        caixa_trans_dx->setGeometry(QRect(61, 60, 61, 25));
        caixa_trans_dx->setMinimum(-1000);
        caixa_trans_dx->setMaximum(1000);
        label_3 = new QLabel(Translacao);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(10, 100, 49, 16));
        caixa_trans_dy = new QSpinBox(Translacao);
        caixa_trans_dy->setObjectName("caixa_trans_dy");
        caixa_trans_dy->setGeometry(QRect(61, 100, 61, 25));
        caixa_trans_dy->setMinimum(-1000);
        caixa_trans_dy->setMaximum(1000);
        btn_aplicar_translacao = new QPushButton(Translacao);
        btn_aplicar_translacao->setObjectName("btn_aplicar_translacao");
        btn_aplicar_translacao->setGeometry(QRect(30, 140, 91, 31));
        Escala = new QGroupBox(centralwidget);
        Escala->setObjectName("Escala");
        Escala->setGeometry(QRect(300, 190, 151, 181));
        label_4 = new QLabel(Escala);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(10, 30, 91, 21));
        QFont font1;
        font1.setBold(true);
        label_4->setFont(font1);
        label_5 = new QLabel(Escala);
        label_5->setObjectName("label_5");
        label_5->setGeometry(QRect(10, 60, 49, 16));
        caixaDupla_escala_sx = new QDoubleSpinBox(Escala);
        caixaDupla_escala_sx->setObjectName("caixaDupla_escala_sx");
        caixaDupla_escala_sx->setGeometry(QRect(60, 60, 62, 25));
        caixaDupla_escala_sx->setSingleStep(0.100000000000000);
        caixaDupla_escala_sx->setValue(1.000000000000000);
        label_6 = new QLabel(Escala);
        label_6->setObjectName("label_6");
        label_6->setGeometry(QRect(10, 100, 49, 16));
        caixaDupla_escala_sy = new QDoubleSpinBox(Escala);
        caixaDupla_escala_sy->setObjectName("caixaDupla_escala_sy");
        caixaDupla_escala_sy->setGeometry(QRect(60, 100, 62, 25));
        caixaDupla_escala_sy->setSingleStep(0.100000000000000);
        caixaDupla_escala_sy->setValue(1.000000000000000);
        btn_aplicar_escala = new QPushButton(Escala);
        btn_aplicar_escala->setObjectName("btn_aplicar_escala");
        btn_aplicar_escala->setGeometry(QRect(30, 140, 91, 31));
        Rotacao = new QGroupBox(centralwidget);
        Rotacao->setObjectName("Rotacao");
        Rotacao->setGeometry(QRect(300, 380, 151, 211));
        label_7 = new QLabel(Rotacao);
        label_7->setObjectName("label_7");
        label_7->setGeometry(QRect(10, 30, 49, 16));
        label_7->setFont(font1);
        label_8 = new QLabel(Rotacao);
        label_8->setObjectName("label_8");
        label_8->setGeometry(QRect(10, 50, 51, 21));
        caixaDupla_rot_angulo = new QDoubleSpinBox(Rotacao);
        caixaDupla_rot_angulo->setObjectName("caixaDupla_rot_angulo");
        caixaDupla_rot_angulo->setGeometry(QRect(70, 50, 62, 25));
        caixaDupla_rot_angulo->setMinimum(-360.000000000000000);
        caixaDupla_rot_angulo->setMaximum(360.000000000000000);
        label_9 = new QLabel(Rotacao);
        label_9->setObjectName("label_9");
        label_9->setGeometry(QRect(10, 110, 51, 20));
        caixa_rot_pivo_x = new QSpinBox(Rotacao);
        caixa_rot_pivo_x->setObjectName("caixa_rot_pivo_x");
        caixa_rot_pivo_x->setGeometry(QRect(70, 90, 61, 25));
        caixa_rot_pivo_x->setMinimum(-5000);
        caixa_rot_pivo_x->setMaximum(5000);
        caixa_rot_pivo_y = new QSpinBox(Rotacao);
        caixa_rot_pivo_y->setObjectName("caixa_rot_pivo_y");
        caixa_rot_pivo_y->setGeometry(QRect(70, 120, 61, 25));
        caixa_rot_pivo_y->setMinimum(-5000);
        caixa_rot_pivo_y->setMaximum(5000);
        btn_aplicar_rotacao = new QPushButton(Rotacao);
        btn_aplicar_rotacao->setObjectName("btn_aplicar_rotacao");
        btn_aplicar_rotacao->setGeometry(QRect(20, 160, 111, 31));
        listWidget_coordenadas = new QListWidget(centralwidget);
        listWidget_coordenadas->setObjectName("listWidget_coordenadas");
        listWidget_coordenadas->setGeometry(QRect(60, 500, 231, 101));
        ControledeCamera = new QGroupBox(centralwidget);
        ControledeCamera->setObjectName("ControledeCamera");
        ControledeCamera->setGeometry(QRect(1150, 20, 181, 311));
        caixa_wc_x = new QDoubleSpinBox(ControledeCamera);
        caixa_wc_x->setObjectName("caixa_wc_x");
        caixa_wc_x->setGeometry(QRect(80, 40, 91, 25));
        caixa_wc_x->setMinimum(-10000.000000000000000);
        caixa_wc_x->setMaximum(10000.000000000000000);
        caixa_wc_x->setSingleStep(10.000000000000000);
        caixa_wc_y = new QDoubleSpinBox(ControledeCamera);
        caixa_wc_y->setObjectName("caixa_wc_y");
        caixa_wc_y->setGeometry(QRect(80, 80, 91, 25));
        caixa_wc_y->setMinimum(-100000.000000000000000);
        caixa_wc_y->setMaximum(100000.000000000000000);
        caixa_wc_y->setSingleStep(10.000000000000000);
        caixa_zoom = new QDoubleSpinBox(ControledeCamera);
        caixa_zoom->setObjectName("caixa_zoom");
        caixa_zoom->setGeometry(QRect(80, 120, 91, 25));
        caixa_zoom->setMinimum(10.000000000000000);
        caixa_zoom->setMaximum(20000.000000000000000);
        caixa_zoom->setSingleStep(50.000000000000000);
        caixa_zoom->setValue(10.000000000000000);
        caixa_angulo = new QDoubleSpinBox(ControledeCamera);
        caixa_angulo->setObjectName("caixa_angulo");
        caixa_angulo->setGeometry(QRect(80, 160, 91, 25));
        caixa_angulo->setMinimum(-360.000000000000000);
        caixa_angulo->setMaximum(360.000000000000000);
        caixa_angulo->setValue(5.000000000000000);
        btn_atualizar_visualizacao = new QPushButton(ControledeCamera);
        btn_atualizar_visualizacao->setObjectName("btn_atualizar_visualizacao");
        btn_atualizar_visualizacao->setGeometry(QRect(40, 220, 91, 41));
        label_10 = new QLabel(ControledeCamera);
        label_10->setObjectName("label_10");
        label_10->setGeometry(QRect(20, 40, 31, 21));
        label_11 = new QLabel(ControledeCamera);
        label_11->setObjectName("label_11");
        label_11->setGeometry(QRect(20, 80, 21, 21));
        label_12 = new QLabel(ControledeCamera);
        label_12->setObjectName("label_12");
        label_12->setGeometry(QRect(10, 120, 51, 21));
        label_13 = new QLabel(ControledeCamera);
        label_13->setObjectName("label_13");
        label_13->setGeometry(QRect(10, 160, 51, 21));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1405, 21));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);
        QObject::connect(btn_sair, &QPushButton::clicked, MainWindow, qOverload<>(&QMainWindow::close));

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        btn_desenhar->setText(QCoreApplication::translate("MainWindow", "Desenhar Pol\303\255gono", nullptr));
        btn_reset->setText(QCoreApplication::translate("MainWindow", "Reset", nullptr));
        btn_novoPoligono->setText(QCoreApplication::translate("MainWindow", "Novo Pol\303\255gono", nullptr));
        btn_sair->setText(QCoreApplication::translate("MainWindow", "Sair", nullptr));
        Translacao->setTitle(QCoreApplication::translate("MainWindow", "Objeto", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "Transla\303\247\303\243o ", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "dX", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "dY", nullptr));
        btn_aplicar_translacao->setText(QCoreApplication::translate("MainWindow", "Transla\303\247\303\243o", nullptr));
        Escala->setTitle(QCoreApplication::translate("MainWindow", "Objeto", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "Escala", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "Fator X", nullptr));
        label_6->setText(QCoreApplication::translate("MainWindow", "Fator Y", nullptr));
        btn_aplicar_escala->setText(QCoreApplication::translate("MainWindow", "Escala", nullptr));
        Rotacao->setTitle(QCoreApplication::translate("MainWindow", "Objeto", nullptr));
        label_7->setText(QCoreApplication::translate("MainWindow", "Rotacao", nullptr));
        label_8->setText(QCoreApplication::translate("MainWindow", "angulo", nullptr));
        label_9->setText(QCoreApplication::translate("MainWindow", "Pivo_X_Y", nullptr));
        btn_aplicar_rotacao->setText(QCoreApplication::translate("MainWindow", "Rotacao", nullptr));
        ControledeCamera->setTitle(QCoreApplication::translate("MainWindow", "Window", nullptr));
        btn_atualizar_visualizacao->setText(QCoreApplication::translate("MainWindow", "Atualizar", nullptr));
        label_10->setText(QCoreApplication::translate("MainWindow", "X", nullptr));
        label_11->setText(QCoreApplication::translate("MainWindow", "Y", nullptr));
        label_12->setText(QCoreApplication::translate("MainWindow", "Distancia", nullptr));
        label_13->setText(QCoreApplication::translate("MainWindow", "Angulo", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
