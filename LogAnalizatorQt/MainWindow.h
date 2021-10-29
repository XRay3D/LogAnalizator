#ifndef DEMOAPP_H
#define DEMOAPP_H

#include "usbdevice.h"
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow* ui;
    UsbDevice* usbDevice;
    void start();
};

#endif // DEMOAPP_H
