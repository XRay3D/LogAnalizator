#include "mainwindow.h"
#include "model.h"
#include "ui_mainwindow.h"
#include "wire.h"
#include <QDebug>
#include <QSettings>
#include <array>

enum {
    USB_CMD_ID = 1,
    USB_CMD_START_CAPTURE = 0x55,
    USB_CMD_PACKET_SIZE = 64
};

#pragma pack(push, 1)
union CAPTURE {
    struct {
        uint8_t USB_REPORT_ID;
        uint8_t USB_CMD;
        uint8_t SYNC;
        uint16_t TIM_PSC;
        uint32_t TIM_ARR;
        uint16_t SAMPLE;
        uint8_t TRIGGER_ENABLE;
        uint8_t TRIGGER_MODE;
        uint8_t TRIGGER_SET[8];
    };
    uint8_t array[USB_CMD_PACKET_SIZE] {};
};
#pragma pack(pop)

#include <QToolBar>

struct Freq {
    QString Name;
    uint16_t Prescaler;
    uint32_t AutoReload;
};

//Freq frequencies[] {
//    { "	100 кГц", 0, 799 },
//    { "	400 кГц", 0, 199 },
//    { "	800 кГц", 0, 99 },
//    { "	1 МГц", 0, 79 },
//    { "	2 МГц", 0, 39 },
//    { "	4 МГц", 0, 19 },
//    { "	5 МГц", 0, 15 },
//    { "	8 МГц", 0, 9 },
//    { "	10 МГц", 0, 7 },
//    { "	16 МГц", 0, 4 },
//    { "	20 МГц", 0, 3 },
//    { "	40 МГц", 0, 1 },
//};

std::vector<Freq> frequencies;

QAction* Trigger;

auto operator""_kHz(unsigned long long val) { return val * 1000; }
auto operator""_MHz(unsigned long long val) { return val * 1000000; }

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui { new Ui::MainWindow }
    , usbDevice { new UsbDevice(this) } {
    ui->setupUi(this);
    ui->cbxSamples->addItems({
        "128",
        "256",
        "512",
        "1024",
        "2048",
        "4096",
        "8192",
        "16384",
    });

    frequencies.reserve(1000);

    for (int freq = 100_kHz; freq <= 20_MHz; freq += 100_kHz) {
        bool ok {};
        for (int pre {}; pre < 80; ++pre) {
            auto tf = 80_MHz - pre * 1_MHz;
            if (!(tf % freq)) {
                ok = true;
                frequencies.emplace_back((freq >= 1_MHz) ? QString("%1 MHz").arg(freq / 1000000.) : QString("%1 kHz").arg(freq / 1000.),
                    pre, tf / freq - 1);
                break;
            }
        }
        //        if (ok) {
        //            ui->plainTextEdit->appendPlainText(QString("err F%1 A%2 P%3").arg(freq).arg(80_MHz / freq - 1).arg(-1));
        //        }
    }

    for (auto&& freq : frequencies)
        ui->cbxFreq->addItem(freq.Name);

    {
        auto toolBar = addToolBar("");
        auto action = toolBar->addAction(QIcon::fromTheme({}), "Start", this, &MainWindow::start);
        action->setShortcut(Qt::Key_F1);

        Trigger = toolBar->addAction(QIcon::fromTheme({}), "Triggers" /*, this, &MainWindow::start*/);
        Trigger->setCheckable(true);

        action = toolBar->addAction(QIcon::fromTheme({}), "Analiz" /*, this, &MainWindow::start*/);
        //        action->setShortcut(Qt::Key_F1);
    }

    enum TRIGGER : uint8_t {
        NONE,
        LOW_LEVEL,
        HIGH_LEVEL,
        RISING_EDGE,
        FALLING_EDGE,
        ANY_EDGE,
    };

    /*
↑↓↗↘╳■□●◯
    */

    QStringList Trigger {
        "Х", // - триггер неактивен",
        "0", // - триггер по низкому уровню",
        "1", // - триггер по высокому уровню",
        "↑", // - триггер по переднему фронту",
        "↓", // - триггер по заднему фронту",
        "↕", // - триггер по любому фронту",
    };

    ui->cbxTrig0->addItems(Trigger);
    ui->cbxTrig1->addItems(Trigger);
    ui->cbxTrig2->addItems(Trigger);
    ui->cbxTrig3->addItems(Trigger);
    ui->cbxTrig4->addItems(Trigger);
    ui->cbxTrig5->addItems(Trigger);
    ui->cbxTrig6->addItems(Trigger);
    ui->cbxTrig7->addItems(Trigger);

    //    ui->tableView->setModel(model = new Model { ui->tableView });

    //    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    //    ui->tableView->horizontalHeader()->setDefaultSectionSize(10);
    //    ui->tableView->horizontalHeader()->setVisible(false);

    //    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    ui->graphicsView->setScene(new QGraphicsScene { ui->graphicsView });
    for (int i {}; auto&& wire : wires) {
        wire = new Wire(i++);
        ui->graphicsView->scene()->addItem(wire);
    }
    //connect(this, SIGNAL(toggle_leds_button_pressed()), plugNPlay, SLOT(toggle_leds()));
    //    connect(plugNPlay, SIGNAL(hid_comm_update(bool, int)), this, SLOT(update_gui(bool, int)));
    //    plugNPlay->PollUSB();

    loadSettings();
}

MainWindow::~MainWindow() {
    //disconnect(this, SIGNAL(toggle_leds_button_pressed()), plugNPlay, SLOT(toggle_leds()));
    //    disconnect(plugNPlay, SIGNAL(hid_comm_update(bool, int)), this, SLOT(update_gui(bool, int)));
    saveSettings();
    delete ui;
}

void MainWindow::start() {
    if (usbDevice->isConnected()) {

        CAPTURE capture;
        capture.USB_REPORT_ID = USB_CMD_ID;
        capture.USB_CMD = USB_CMD_START_CAPTURE;

        int Value = ui->cbxFreq->currentIndex();
        capture.SYNC = 0;
        capture.TIM_PSC = frequencies[Value].Prescaler;
        capture.TIM_ARR = frequencies[Value].AutoReload;

        capture.SAMPLE = ui->cbxSamples->currentText().toUInt();
        capture.TRIGGER_ENABLE = Trigger->isChecked();
        if (capture.TRIGGER_ENABLE) {
            capture.TRIGGER_MODE = 8;
            capture.TRIGGER_SET[0] = ui->cbxTrig0->currentIndex();
            capture.TRIGGER_SET[1] = ui->cbxTrig1->currentIndex();
            capture.TRIGGER_SET[2] = ui->cbxTrig2->currentIndex();
            capture.TRIGGER_SET[3] = ui->cbxTrig3->currentIndex();
            capture.TRIGGER_SET[4] = ui->cbxTrig4->currentIndex();
            capture.TRIGGER_SET[5] = ui->cbxTrig5->currentIndex();
            capture.TRIGGER_SET[6] = ui->cbxTrig6->currentIndex();
            capture.TRIGGER_SET[7] = ui->cbxTrig7->currentIndex();
            if (capture.TRIGGER_SET[0]
                || capture.TRIGGER_SET[1]
                || capture.TRIGGER_SET[2]
                || capture.TRIGGER_SET[3]
                || capture.TRIGGER_SET[4]
                || capture.TRIGGER_SET[5]
                || capture.TRIGGER_SET[6]
                || capture.TRIGGER_SET[7]) {
            } else {
                capture.TRIGGER_ENABLE = 0;
            }
        } else {
            capture.TRIGGER_ENABLE = 0;
        }

        [[maybe_unused]] int USBSuccess = usbDevice->Write({ capture.array });

        std::vector<uint8_t> InputData(capture.SAMPLE);

        Value = capture.SAMPLE / USB_CMD_PACKET_SIZE;

        uint16_t ctr {};
        while (ctr < capture.SAMPLE && USBSuccess > 0) {
            uint8_t data[USB_CMD_PACKET_SIZE + 1];
            USBSuccess = usbDevice->Read(data);
            std::memcpy(InputData.data() + ctr, data + 1, USB_CMD_PACKET_SIZE);
            ctr += USB_CMD_PACKET_SIZE;
            qDebug() << 1 << ctr;
        }

        if (USBSuccess == -1)
            usbDevice->Close();

        for (auto&& wire : wires) {
            wire->setData(InputData);
        }

        auto rect { ui->graphicsView->scene()->itemsBoundingRect() };
        ui->graphicsView->setSceneRect(rect);
        //        ui->graphicsView->fitInView(rect);
        //        model->setData(std::move(InputData));

        //        std::vector<PointCollection> NewChannelsData(CHANNEL_COUNT);
        //        for (int chnl = 0; chnl < CHANNEL_COUNT; chnl++) {
        //            NewChannelsData[chnl] = std::make_shared<PointCollection>();
        //        }

        //        for (int PointIndex = 0; PointIndex < SamplesToCapture; PointIndex++) {
        //            for (int chnl = 0; chnl < CHANNEL_COUNT; chnl++) {
        //                Value = START_Y;
        //                if ((InputData[PointIndex] & (1 << chnl)) == (1 << chnl)) {
        //                    Value += PULSE_HEIGHT;
        //                }
        //                NewChannelsData[chnl]->Add(Point(PointIndex, Value));
        //            }
        //        }

        //если активированы триггеры, в начало графика добавляются 2 точки,
        //соответствующие триггеру канала;
        //если у канала нет триггера, добавленные точки повторяют начальную точку канала
        //        if (TriggerActivationCheckBox->IsChecked == true) {
        //            for (int chnl = 0; chnl < CHANNEL_COUNT; chnl++) {
        //                for (int PointIndex = 0; PointIndex < SamplesToCapture; PointIndex++) {
        //                    NewChannelsData[chnl][PointIndex] = Point(NewChannelsData[chnl][PointIndex]->X + EXTRA_POINTS_COUNT_FOR_TRIGGER, NewChannelsData[chnl][PointIndex]->Y);
        //                }
        //            }

        //            SamplesToCapture += EXTRA_POINTS_COUNT_FOR_TRIGGER;

        //            for (int chnl = 0; chnl < CHANNEL_COUNT; chnl++) {
        //                switch (Array::IndexOf(TriggersString, Channels[chnl]->getTrigger())) {
        //                case TRIGGER_NONE:
        //                    NewChannelsData[chnl]->Insert(0, Point(1, NewChannelsData[chnl][0]->Y));
        //                    NewChannelsData[chnl]->Insert(0, Point(0, NewChannelsData[chnl][0]->Y));
        //                    break;

        //                case TRIGGER_LOW_LEVEL:
        //                    NewChannelsData[chnl]->Insert(0, Point(1, PULSE_LOW));
        //                    NewChannelsData[chnl]->Insert(0, Point(0, PULSE_LOW));
        //                    break;

        //                case TRIGGER_HIGH_LEVEL:
        //                    NewChannelsData[chnl]->Insert(0, Point(1, PULSE_HIGH));
        //                    NewChannelsData[chnl]->Insert(0, Point(0, PULSE_HIGH));
        //                    break;

        //                case TRIGGER_RISING_EDGE:
        //                    NewChannelsData[chnl]->Insert(0, Point(1, PULSE_HIGH));
        //                    NewChannelsData[chnl]->Insert(0, Point(0, PULSE_LOW));
        //                    break;

        //                case TRIGGER_FALLING_EDGE:
        //                case TRIGGER_ANY_EDGE:
        //                    NewChannelsData[chnl]->Insert(0, Point(1, PULSE_LOW));
        //                    NewChannelsData[chnl]->Insert(0, Point(0, PULSE_HIGH));
        //                    break;
        //                }
        //            }
        //        }

        //        for (int chnl = 0; chnl < CHANNEL_COUNT; chnl++) {
        //            Channels[chnl]->CaptureData(NewChannelsData[chnl]);
        //        }

        //        for (int decoder = 0; decoder < IAnalyzers->Count; decoder++) {
        //            GetPointsAndAnalyze(decoder);

        //            if (decoder == InterfaceComboBox::SelectedIndex) {
        //                InterfaceItemsListView->ItemsSource = IAnalyzers[decoder]->getInterfaceParts();
        //            }
        //        }

        //        ScaleXIndex = 1;
        //        Zooming(ZOOM_OUT);
    }
}

void MainWindow::saveSettings() {
    QSettings settings;
    settings.beginGroup("MainWindow");
    settings.setValue("State", saveState());
    settings.setValue("Geometry", saveGeometry());
    settings.endGroup();

    settings.beginGroup("Trig");
    settings.setValue("Trigger", Trigger->isChecked());
    settings.setValue("cbxTrig0", ui->cbxTrig0->currentIndex());
    settings.setValue("cbxTrig1", ui->cbxTrig1->currentIndex());
    settings.setValue("cbxTrig2", ui->cbxTrig2->currentIndex());
    settings.setValue("cbxTrig3", ui->cbxTrig3->currentIndex());
    settings.setValue("cbxTrig4", ui->cbxTrig4->currentIndex());
    settings.setValue("cbxTrig5", ui->cbxTrig5->currentIndex());
    settings.setValue("cbxTrig6", ui->cbxTrig6->currentIndex());
    settings.setValue("cbxTrig7", ui->cbxTrig7->currentIndex());
    settings.endGroup();

    settings.beginGroup("Cap");
    settings.setValue("cbxFreq", ui->cbxFreq->currentIndex());
    settings.setValue("cbxSamples", ui->cbxSamples->currentIndex());
    settings.endGroup();
}

void MainWindow::loadSettings() {
    QSettings settings;
    settings.beginGroup("MainWindow");
    restoreState(settings.value("State").toByteArray());
    restoreGeometry(settings.value("Geometry").toByteArray());
    settings.endGroup();

    settings.beginGroup("Trig");
    Trigger->setChecked(settings.value("Trigger").toBool());
    ui->cbxTrig0->setCurrentIndex(settings.value("cbxTrig0").toUInt());
    ui->cbxTrig1->setCurrentIndex(settings.value("cbxTrig1").toUInt());
    ui->cbxTrig2->setCurrentIndex(settings.value("cbxTrig2").toUInt());
    ui->cbxTrig3->setCurrentIndex(settings.value("cbxTrig3").toUInt());
    ui->cbxTrig4->setCurrentIndex(settings.value("cbxTrig4").toUInt());
    ui->cbxTrig5->setCurrentIndex(settings.value("cbxTrig5").toUInt());
    ui->cbxTrig6->setCurrentIndex(settings.value("cbxTrig6").toUInt());
    ui->cbxTrig7->setCurrentIndex(settings.value("cbxTrig7").toUInt());
    settings.endGroup();

    settings.beginGroup("Cap");
    ui->cbxFreq->setCurrentIndex(settings.value("cbxFreq").toUInt());
    ui->cbxSamples->setCurrentIndex(settings.value("cbxSamples").toUInt());
    settings.endGroup();
}

//void DemoApp::update_gui(bool isConnected, int potentiometerValue) {
//    if (isConnected) {
//        ui->deviceConnectedStatus->setText("Device Found: AttachedState = TRUE");
//    } else {
//        ui->deviceConnectedStatus->setText("Device Not Detected: Verify Connection/Correct Firmware");
//    }
//}
