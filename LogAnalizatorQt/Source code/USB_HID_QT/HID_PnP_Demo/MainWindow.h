#ifndef DEMOAPP_H
#define DEMOAPP_H

#include "hid_pnp.h"
#include <QMainWindow>

namespace Ui {
    class DemoApp;
}

class DemoApp : public QMainWindow
{
    Q_OBJECT

public:
    explicit DemoApp(QWidget *parent = 0);
    ~DemoApp();

private:
    Ui::DemoApp *ui;
    HID_PnP *plugNPlay;

    bool r,g;
    int p;

public slots:
    void update_gui(bool isConnected, int potentiometerValue);

signals:


private slots:

    void on_horizontalSlider_sliderMoved(int position);
    void on_pushButton_clicked(bool checked);
    void on_pushButton_2_clicked(bool checked);
    void on_horizontalSlider_2_sliderMoved(int position);
    void on_pushButton_3_clicked();
};

#endif // DEMOAPP_H
