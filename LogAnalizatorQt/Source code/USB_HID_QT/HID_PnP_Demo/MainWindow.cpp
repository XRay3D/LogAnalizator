#include  "MainWindow.h"
#include "ui_demoapp.h"

DemoApp::DemoApp(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DemoApp)
{
    ui->setupUi(this);
    ui->progressBar->setMaximum(255);

    plugNPlay = new HID_PnP();

    //connect(this, SIGNAL(toggle_leds_button_pressed()), plugNPlay, SLOT(toggle_leds()));
    connect(plugNPlay, SIGNAL(hid_comm_update(bool,  int)), this, SLOT(update_gui(bool,  int)));
}

DemoApp::~DemoApp()
{
    //disconnect(this, SIGNAL(toggle_leds_button_pressed()), plugNPlay, SLOT(toggle_leds()));
    disconnect(plugNPlay, SIGNAL(hid_comm_update(bool,  int)), this, SLOT(update_gui(bool,  int)));
    delete ui;
    delete plugNPlay;
}

void DemoApp::update_gui(bool isConnected,  int potentiometerValue)
{
    if(isConnected)
    {
        ui->progressBar->setEnabled(true);
        ui->deviceConnectedStatus->setText("Device Found: AttachedState = TRUE");
        ui->progressBar->setValue(potentiometerValue);
    }
    else
    {
        ui->progressBar->setEnabled(false);
        ui->deviceConnectedStatus->setText("Device Not Detected: Verify Connection/Correct Firmware");
        ui->progressBar->setValue(0);
    }
}

void DemoApp::on_horizontalSlider_sliderMoved(int position)
{
    p=position;
    plugNPlay->asd(p,r,g);
}

void DemoApp::on_pushButton_clicked(bool checked)
{
    g=checked;
    plugNPlay->asd(p,r,g);
}

void DemoApp::on_pushButton_2_clicked(bool checked)
{
    r=checked;
    plugNPlay->asd(p,r,g);
}

void DemoApp::on_horizontalSlider_2_sliderMoved(int position)
{
    char i;
    i=1<<position;
    p=0xff-i;
    plugNPlay->asd(p,r,g);
}


void DemoApp::on_pushButton_3_clicked()
{

}
