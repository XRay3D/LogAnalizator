#include "MainWindow.h"
#include "ui_demoapp.h"

DemoApp::DemoApp(QWidget* parent)
    : QMainWindow(parent)
    , ui { new Ui::DemoApp }
    , plugNPlay { new HID_PnP(this) }
{
    ui->setupUi(this);

    //connect(this, SIGNAL(toggle_leds_button_pressed()), plugNPlay, SLOT(toggle_leds()));
    //    connect(plugNPlay, SIGNAL(hid_comm_update(bool, int)), this, SLOT(update_gui(bool, int)));
    //    plugNPlay->PollUSB();
}

DemoApp::~DemoApp()
{
    //disconnect(this, SIGNAL(toggle_leds_button_pressed()), plugNPlay, SLOT(toggle_leds()));
    //    disconnect(plugNPlay, SIGNAL(hid_comm_update(bool, int)), this, SLOT(update_gui(bool, int)));
    delete ui;
}

void DemoApp::update_gui(bool isConnected, int potentiometerValue)
{
    if (isConnected) {
        ui->deviceConnectedStatus->setText("Device Found: AttachedState = TRUE");
    } else {
        ui->deviceConnectedStatus->setText("Device Not Detected: Verify Connection/Correct Firmware");
    }
}
