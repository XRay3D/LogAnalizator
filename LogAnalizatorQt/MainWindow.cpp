#include "MainWindow.h"
#include "ui_demoapp.h"
#include <array>

static const unsigned char CHANNEL_COUNT = 8;
static const int START_X_VALUE = 0;
static const unsigned char FRAME_UPDATE_PART = 3;
static const int POINTS_COUNT = 512;
static const unsigned char GRAPHS_GAP = 55;
static const unsigned char ZOOM_OUT = 0;
static const unsigned char ZOOM_IN = 1;
static std::vector<double> const ScaleX; //шаги по оси х при мастабировании: 1, 3, 5, 10, 20
static std::array const MainTimeMarkStep { 50, 100, 90, 150, 200, 200 };
static std::array const SmallTimeMarkStep { 5, 10, 9, 15, 20, 20 };
static std::vector<double> const DescretTime; //мкс
static std::vector<std::string> const TimeUnits; //частота 100 кГц; 400 кГц; 800 кГц; 1 МГц; 1,6 МГц; 2 МГц; 3 МГц; 4 МГц; 6 МГц; 8 МГц;

static std::vector<int> const SampleCount { 128, 256, 512, 1024, 2048, 4096 };
static std::vector<std::string> const FreqStrings;
static std::vector<unsigned char> const CaptureTimPsc { 23, 11, 9, 7, 5, 5, 3, 3, 3, 2 };
static std::vector<unsigned char> const CaptureTimArr { 19, 9, 5, 5, 4, 3, 3, 2, 1, 1 };
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

static const unsigned char USB_CMD_PACKET_SIZE = 64;
//static const unsigned char USB_REPORT_ID_POS = 0;
//static const unsigned char USB_CMD_POS = 1;

static const unsigned char USB_CMD_ID = 1;
static const unsigned char USB_CMD_START_CAPTURE = 1;

#pragma pack(push, 1)
union CAPTURE {
    struct {
        uint8_t USB_REPORT_ID_POS;
        uint8_t USB_CMD_POS;
        uint8_t CAPTURE_SYNC_OFFSET;
        uint8_t CAPTURE_TIM_PSC_OFFSET;
        uint8_t CAPTURE_TIM_ARR_OFFSET;
        uint16_t CAPTURE_SAMPLE_OFFSET;
        uint8_t CAPTURE_TRIGGER_ENABLE_OFFSET;
        uint8_t CAPTURE_TRIGGER_MODE_OFFSET;
        uint8_t CAPTURE_TRIGGER_SET_OFFSET;
        uint8_t CAPTURE_TRIGGER_MASK;
    };
    uint8_t array[USB_CMD_PACKET_SIZE] {};
};
#pragma pack(pop)
//static const unsigned char CAPTURE_SYNC_OFFSET = USB_CMD_POS + 1;
//static const unsigned char CAPTURE_TIM_PSC_OFFSET = CAPTURE_SYNC_OFFSET + 1;
//static const unsigned char CAPTURE_TIM_ARR_OFFSET = CAPTURE_TIM_PSC_OFFSET + 1;
//static const unsigned char CAPTURE_SAMPLE_OFFSET = CAPTURE_TIM_ARR_OFFSET + 1;
//static const unsigned char CAPTURE_TRIGGER_ENABLE_OFFSET = CAPTURE_SAMPLE_OFFSET + 2;
//static const unsigned char CAPTURE_TRIGGER_MODE_OFFSET = CAPTURE_TRIGGER_ENABLE_OFFSET + 1;
//static const unsigned char CAPTURE_TRIGGER_SET_OFFSET = CAPTURE_TRIGGER_MODE_OFFSET + 1;

static const unsigned char CAPTURE_SYNC_INTERNAL = 0;
static const unsigned char CAPTURE_MODE_EXTERNAL_CLK = 1;
static const unsigned char CAPTURE_TRIGGER_BYTES_COUNT = 4;
static const unsigned char CAPTURE_TRIGGER_DISABLE = 0;
static const unsigned char CAPTURE_TRIGGER_ENABLE = 1;
static const unsigned char CAPTURE_TRIGGER_MODE_CHANNELS = 0;
static const unsigned char CAPTURE_TRIGGER_MODE_EXT_LINES = 1;

static const unsigned char EXTRA_POINTS_COUNT_FOR_TRIGGER = 2;

int End_X_Value = 900;
int FrameStartPoint = 0;
int FramePoints = 0;
int UpdatePoints = 0;
int DescretTimeIndex = 0;
int ScaleXIndex = 0;
int SamplesToCapture = SampleCount[2];
//std::vector<AnalyzerChannel> Channels;

bool MeasuringActive = false;
bool LineActive = false;
//std::shared_ptr<Polyline> PanPolyline = std::make_shared<Polyline>();
//std::shared_ptr<Polyline> ToolPolyline = nullptr;
bool USBDevDetected = false;
//std::shared_ptr<ObservableCollection<IHardwareInterface>> IAnalyzers;

#include <QToolBar>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui { new Ui::DemoApp }
    , usbDevice { new UsbDevice(this) } {
    ui->setupUi(this);
    ui->cbxSamples->addItems({ "128", "256", "512", "1024", "2048", "4096" });
    ui->cbxFreq->addItems({ "100 кГц", "400 кГц", "800 кГц", "1 МГц", "1,6 МГц", "2 МГц", "3 МГц", "4 МГц", "6 МГц", "8 МГц" });
    auto toolBar = addToolBar("");
    auto action = toolBar->addAction(QIcon::fromTheme({}), "start", this, &MainWindow::start);

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
        capture.USB_REPORT_ID_POS = USB_CMD_ID;
        capture.USB_CMD_POS = USB_CMD_START_CAPTURE;

        int Value = ui->cbxFreq->currentIndex();
        capture.CAPTURE_SYNC_OFFSET = CAPTURE_SYNC_INTERNAL;
        capture.CAPTURE_TIM_PSC_OFFSET = CaptureTimPsc[Value];
        capture.CAPTURE_TIM_ARR_OFFSET = CaptureTimArr[Value];
        DescretTimeIndex = Value;

        SamplesToCapture = SampleCount[ui->cbxSamples->currentIndex()];
        capture.CAPTURE_SAMPLE_OFFSET = SamplesToCapture;
        if (0) { //TriggerActivationCheckBox->IsChecked == true) {
            //            capture.CAPTURE_TRIGGER_ENABLE_OFFSET = CAPTURE_TRIGGER_ENABLE;
            //            capture.CAPTURE_TRIGGER_MODE_OFFSET = CAPTURE_TRIGGER_MODE_CHANNELS;
            //            for (int TrigByte = 0; TrigByte < CAPTURE_TRIGGER_BYTES_COUNT; TrigByte++) {
            //                USBPacket[CAPTURE_TRIGGER_SET_OFFSET + TrigByte] = static_cast<unsigned char>((Array::IndexOf(TriggersString, Channels[2 * TrigByte + 1]->getTrigger()) << 4) | (Array::IndexOf(TriggersString, Channels[2 * TrigByte]->getTrigger())));
            //            }
            //            if ((capture.CAPTURE_TRIGGER_SET_OFFSET == 0) && (USBPacket[CAPTURE_TRIGGER_SET_OFFSET + 1] == 0) && (USBPacket[CAPTURE_TRIGGER_SET_OFFSET + 2] == 0) && (USBPacket[CAPTURE_TRIGGER_SET_OFFSET + 3] == 0)) {
            //                capture.CAPTURE_TRIGGER_ENABLE_OFFSET = CAPTURE_TRIGGER_DISABLE;
            //            }
        } else {
            capture.CAPTURE_TRIGGER_ENABLE_OFFSET = CAPTURE_TRIGGER_DISABLE;
        }

        bool USBSuccess = usbDevice->Write({ capture.array });

        std::vector<unsigned char> InputData(SamplesToCapture);

        Value = SamplesToCapture / USB_CMD_PACKET_SIZE;

        for (int i = 0; i < Value; i++) {
            USBSuccess = usbDevice->Read({ InputData.data() + i * USB_CMD_PACKET_SIZE, USB_CMD_PACKET_SIZE });
        }

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
