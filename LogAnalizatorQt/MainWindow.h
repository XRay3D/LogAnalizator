#ifndef DEMOAPP_H
#define DEMOAPP_H

#include "hid_pnp.h"
#include <QMainWindow>

namespace Ui {
class DemoApp;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

private:
    Ui::DemoApp* ui;
    UsbDevice* usbDevice;
    void start();
};

#endif // DEMOAPP_H
