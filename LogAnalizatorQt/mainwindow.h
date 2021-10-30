#pragma once

#include "usbdevice.h"
#include <QMainWindow>

class Model;

class Wire;
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
    class Model* model;
    class Wire* wires[8];
    void start();

    void saveSettings();
    void loadSettings();
};
