#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow){
    ui->setupUi(this);

    QGLFormat format;
    format.setVersion(4,1);
    format.setProfile(QGLFormat::CoreProfile);

    m_openGLWidget = new OpenGLWidget(format,this);
    ui->gridLayout->addWidget(m_openGLWidget,0,0,1,1);

    ui->m_horizontalSlider->setMinimum(0);
    ui->m_horizontalSlider->setMaximum(100);

    connect(ui->m_horizontalSlider, SIGNAL(sliderMoved(int)), ui->m_lcdNumber, SLOT(display(int)));
    connect(ui->m_horizontalSlider, SIGNAL(sliderMoved(int)), m_openGLWidget, SLOT(setRoughness(int)));

}

MainWindow::~MainWindow(){
    delete ui;
    delete m_openGLWidget;

}
