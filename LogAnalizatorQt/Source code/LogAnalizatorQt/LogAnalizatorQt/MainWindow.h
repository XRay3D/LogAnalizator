#ifndef DEMOAPP_H
#define DEMOAPP_H

#include "hid_pnp.h"
#include <QMainWindow>

namespace Ui {
class DemoApp;
}

class DemoApp : public QMainWindow {
    Q_OBJECT

public:
    explicit DemoApp(QWidget* parent = 0);
    ~DemoApp();

private:
    Ui::DemoApp* ui;
    HID_PnP* plugNPlay;

    bool r, g;
    int p;

public slots:
    void update_gui(bool isConnected, int potentiometerValue);
};

#endif // DEMOAPP_H
