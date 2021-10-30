#include "mainwindow.h"
#include "model.h"
#include "ui_MainWindow.h"
#include "wire.h"
#include <QDebug>
#include <array>

static const unsigned char CHANNEL_COUNT = 8;
static const int START_X_VALUE = 0;
static const unsigned char FRAME_UPDATE_PART = 3;
static const int POINTS_COUNT = 512;
static const unsigned char GRAPHS_GAP = 55;
static const unsigned char ZOOM_OUT = 0;
static const unsigned char ZOOM_IN = 1;
static std::vector<double> const ScaleX; //шаги по оси х при мастабировании: 1, 3, 5, 10, 20
static std::array const MainTimeMarkStep {
    50,
    100,
    90,
    150,
    200,
    200,
};
static std::array const SmallTimeMarkStep {
    5,
    10,
    9,
    15,
    20,
    20,
};
static std::vector<double> const DescretTime; //мкс
static std::vector<std::string> const TimeUnits; //частота 100 кГц; 400 кГц; 800 кГц; 1 МГц; 1,6 МГц; 2 МГц; 3 МГц; 4 МГц; 6 МГц; 8 МГц;

//static std::vector<int> const SampleCount { 128, 256, 512, 1024, 2048, 4096 };
static std::vector<std::string> const FreqStrings;

static std::vector<std::string> const TriggersString;
static const int TRIGGER_NONE = 0;
static const int TRIGGER_LOW_LEVEL = 1;
static const int TRIGGER_HIGH_LEVEL = 2;
static const int TRIGGER_RISING_EDGE = 3;
static const int TRIGGER_FALLING_EDGE = 4;
static const int TRIGGER_ANY_EDGE = 5;

static const std::string TriggerHelp;

static const int PULSE_HEIGHT = 30;
static const int START_Y = 10;

static const int MAIN_TIME_MARK_HEIGHT = 20;
static const int SMALL_TIME_MARK_HEIGHT = MAIN_TIME_MARK_HEIGHT / 2;
static const double TOOLS_ELEMENT_OPACITY = 0.0;
static const std::string TOOLS_ELEMENT_COLOR;
static const std::string INTERFACE_TEXT_COLOR;

static const unsigned char USB_CMD_PACKET_SIZE = 64; // because first is REPORT_ID
//static const unsigned char USB_REPORT_ID_POS = 0;
//static const unsigned char USB_CMD_POS = 1;

static const unsigned char USB_CMD_ID = 1;
static const unsigned char USB_CMD_START_CAPTURE = 0x55;

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
        uint8_t TRIGGER_SET;
        uint8_t TRIGGER_MASK;
    };
    uint8_t array[USB_CMD_PACKET_SIZE] {};
};
#pragma pack(pop)

static const unsigned char SYNC_INTERNAL = 0;
static const unsigned char MODE_EXTERNAL_CLK = 1;
static const unsigned char TRIGGER_BYTES_COUNT = 4;
static const unsigned char TRIGGER_DISABLE = 0;
static const unsigned char TRIGGER_ENABLE = 1;
static const unsigned char TRIGGER_MODE_CHANNELS = 0;
static const unsigned char TRIGGER_MODE_EXT_LINES = 1;

static const unsigned char EXTRA_POINTS_COUNT_FOR_TRIGGER = 2;

int End_X_Value = 900;
int FrameStartPoint = 0;
int FramePoints = 0;
int UpdatePoints = 0;
int DescretTimeIndex = 0;
int ScaleXIndex = 0;
//int SamplesToCapture = SampleCount[2];
//std::vector<AnalyzerChannel> Channels;

bool MeasuringActive = false;
bool LineActive = false;
//std::shared_ptr<Polyline> PanPolyline = std::make_shared<Polyline>();
//std::shared_ptr<Polyline> ToolPolyline = nullptr;
bool USBDevDetected = false;
//std::shared_ptr<ObservableCollection<IHardwareInterface>> IAnalyzers;

#include <QToolBar>

struct Freq {
    QString Name;
    uint16_t Prescaler;
    uint32_t AutoReload;
};

Freq frequencies[] {
    { "	100 кГц", 0, 799 },
    { "	400 кГц", 0, 199 },
    { "	800 кГц", 0, 99 },
    { "	1 МГц", 0, 79 },
    { "	2 МГц", 0, 39 },
    { "	4 МГц", 0, 19 },
    { "	5 МГц", 0, 15 },
    { "	8 МГц", 0, 9 },
    { "	10 МГц", 0, 7 },
    { "	16 МГц", 0, 4 },
    { "	20 МГц", 0, 3 },
    { "	40 МГц", 0, 1 },
};

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

    for (auto&& freq : frequencies)
        ui->cbxFreq->addItem(freq.Name);

    auto toolBar = addToolBar("");
    auto action = toolBar->addAction(QIcon::fromTheme({}), "start", this, &MainWindow::start);
    action->setShortcut(Qt::Key_F1);
    ui->tableView->setModel(model = new Model { ui->tableView });

    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableView->horizontalHeader()->setDefaultSectionSize(10);
    ui->tableView->horizontalHeader()->setVisible(false);

    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    ui->graphicsView->setScene(new QGraphicsScene { ui->graphicsView });
    for (int i {}; auto&& wire : wires) {
        wire = new Wire(i++);
        ui->graphicsView->scene()->addItem(wire);
    }
    //connect(this, SIGNAL(toggle_leds_button_pressed()), plugNPlay, SLOT(toggle_leds()));
    //    connect(plugNPlay, SIGNAL(hid_comm_update(bool, int)), this, SLOT(update_gui(bool, int)));
    //    plugNPlay->PollUSB();
}

MainWindow::~MainWindow() {
    //disconnect(this, SIGNAL(toggle_leds_button_pressed()), plugNPlay, SLOT(toggle_leds()));
    //    disconnect(plugNPlay, SIGNAL(hid_comm_update(bool, int)), this, SLOT(update_gui(bool, int)));
    delete ui;
}

void MainWindow::start() {
    if (usbDevice->isConnected()) {

        CAPTURE capture;
        capture.USB_REPORT_ID = USB_CMD_ID;
        capture.USB_CMD = USB_CMD_START_CAPTURE;

        int Value = ui->cbxFreq->currentIndex();
        capture.SYNC = SYNC_INTERNAL;
        capture.TIM_PSC = frequencies[Value].Prescaler;
        capture.TIM_ARR = frequencies[Value].AutoReload;
        DescretTimeIndex = Value;

        capture.SAMPLE = ui->cbxSamples->currentText().toUInt();
        if (0) { //TriggerActivationCheckBox->IsChecked == true) {
            //            capture.TRIGGER_ENABLE = TRIGGER_ENABLE;
            //            capture.TRIGGER_MODE = TRIGGER_MODE_CHANNELS;
            //            for (int TrigByte = 0; TrigByte < TRIGGER_BYTES_COUNT; TrigByte++) {
            //                USBPacket[TRIGGER_SET + TrigByte] = static_cast<unsigned char>((Array::IndexOf(TriggersString, Channels[2 * TrigByte + 1]->getTrigger()) << 4) | (Array::IndexOf(TriggersString, Channels[2 * TrigByte]->getTrigger())));
            //            }
            //            if ((capture.TRIGGER_SET == 0) && (USBPacket[TRIGGER_SET + 1] == 0) && (USBPacket[TRIGGER_SET + 2] == 0) && (USBPacket[TRIGGER_SET + 3] == 0)) {
            //                capture.TRIGGER_ENABLE = TRIGGER_DISABLE;
            //            }
        } else {
            capture.TRIGGER_ENABLE = TRIGGER_DISABLE;
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
        model->setData(std::move(InputData));

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

//void DemoApp::update_gui(bool isConnected, int potentiometerValue) {
//    if (isConnected) {
//        ui->deviceConnectedStatus->setText("Device Found: AttachedState = TRUE");
//    } else {
//        ui->deviceConnectedStatus->setText("Device Not Detected: Verify Connection/Correct Firmware");
//    }
//}
