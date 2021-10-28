#pragma once

#include <string>
#include <vector>
#include <memory>

//C# TO C++ CONVERTER NOTE: Forward class declarations:
namespace LogicAnalyzer { class AnalyzerChannel; }
namespace LogicAnalyzer { class IHardwareInterface; }

/*
********************************************************************************
* COPYRIGHT(c) ЗАО «ЧИП и ДИП», 2018
* 
* Программное обеспечение предоставляется на условиях «как есть» (as is).
* При распространении указание автора обязательно.
********************************************************************************
*/



namespace LogicAnalyzer
{
	/// <summary>
	/// Логика взаимодействия для MainWindow.xaml
	/// </summary>
	class MainWindow : public Window
	{
	private:
		static const unsigned char CHANNEL_COUNT = 8;
		static const int START_X_VALUE = 0;
		static const unsigned char FRAME_UPDATE_PART = 3;
		static const int POINTS_COUNT = 512;
		static const unsigned char GRAPHS_GAP = 55;
		static const unsigned char ZOOM_OUT = 0;
		static const unsigned char ZOOM_IN = 1;
		static std::vector<double> const ScaleX; //шаги по оси х при мастабировании: 1, 3, 5, 10, 20
		static std::vector<int> const MainTimeMarkStep = {50, 100, 90, 150, 200, 200};
		static std::vector<int> const SmallTimeMarkStep = {5, 10, 9, 15, 20, 20};
		static std::vector<double> const DescretTime; //мкс
										//частота 100 кГц; 400 кГц; 800 кГц; 1 МГц; 1,6 МГц; 2 МГц; 3 МГц; 4 МГц; 6 МГц; 8 МГц; 
		static std::vector<std::string> const TimeUnits;
		static std::vector<int> const SampleCount = {128, 256, 512, 1024, 2048, 4096};
		static std::vector<std::string> const FreqStrings;
		static std::vector<unsigned char> const CaptureTimPsc = {23, 11, 9, 7, 5, 5, 3, 3, 3, 2};
		static std::vector<unsigned char> const CaptureTimArr = {19, 9, 5, 5, 4, 3, 3, 2, 1, 1};
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
	public:
		static const int PULSE_LOW = START_Y;
		static const int PULSE_HIGH = START_Y + PULSE_HEIGHT;
	private:
		static const int MAIN_TIME_MARK_HEIGHT = 20;
		static const int SMALL_TIME_MARK_HEIGHT = MAIN_TIME_MARK_HEIGHT / 2;
		static const double TOOLS_ELEMENT_OPACITY;
		static const std::string TOOLS_ELEMENT_COLOR;
		static const std::string INTERFACE_TEXT_COLOR;

		static const unsigned char USB_CMD_PACKET_SIZE = 64;
		static const unsigned char USB_REPORT_ID_POS = 0;
		static const unsigned char USB_CMD_POS = 1;

		static const unsigned char USB_CMD_ID = 1;
		static const unsigned char USB_CMD_START_CAPTURE = 1;

		static const unsigned char CAPTURE_SYNC_OFFSET = USB_CMD_POS + 1;
		static const unsigned char CAPTURE_TIM_PSC_OFFSET = CAPTURE_SYNC_OFFSET + 1;
		static const unsigned char CAPTURE_TIM_ARR_OFFSET = CAPTURE_TIM_PSC_OFFSET + 1;
		static const unsigned char CAPTURE_SAMPLE_OFFSET = CAPTURE_TIM_ARR_OFFSET + 1;
		static const unsigned char CAPTURE_TRIGGER_ENABLE_OFFSET = CAPTURE_SAMPLE_OFFSET + 2;
		static const unsigned char CAPTURE_TRIGGER_MODE_OFFSET = CAPTURE_TRIGGER_ENABLE_OFFSET + 1;
		static const unsigned char CAPTURE_TRIGGER_SET_OFFSET = CAPTURE_TRIGGER_MODE_OFFSET + 1;

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
		std::vector<AnalyzerChannel> Channels;

		bool MeasuringActive = false;
		bool LineActive = false;
		std::shared_ptr<Polyline> PanPolyline = std::make_shared<Polyline>();
		std::shared_ptr<Polyline> ToolPolyline = nullptr;
		bool USBDevDetected = false;
		std::shared_ptr<ObservableCollection<IHardwareInterface>> IAnalyzers;


	public:
		MainWindow();

	private:
		void GraphMeasuring_MouseLeftButtonUp(const std::shared_ptr<void> &sender, const std::shared_ptr<MouseButtonEventArgs> &e);

		void GraphMeasuring_MouseMove(const std::shared_ptr<void> &sender, const std::shared_ptr<MouseEventArgs> &e);

		void MeasureResult_Loaded(const std::shared_ptr<void> &sender, const std::shared_ptr<RoutedEventArgs> &e);

		void GraphMeasuring_MouseLeftButtonDown(const std::shared_ptr<void> &sender, const std::shared_ptr<MouseButtonEventArgs> &e);

		void PrevFrameButton_Click(const std::shared_ptr<void> &sender, const std::shared_ptr<RoutedEventArgs> &e);

		void NextFrameButton_Click(const std::shared_ptr<void> &sender, const std::shared_ptr<RoutedEventArgs> &e);

		void FrameChange();

		void UpdateFrameParameters();

		void ZoomOutButton_Click(const std::shared_ptr<void> &sender, const std::shared_ptr<RoutedEventArgs> &e);

		void ZoomInButton_Click(const std::shared_ptr<void> &sender, const std::shared_ptr<RoutedEventArgs> &e);

		void Zooming(unsigned char Direction);

		void MeasureButton_Click(const std::shared_ptr<void> &sender, const std::shared_ptr<RoutedEventArgs> &e);

		void GraphsCanvasSizeChanged(const std::shared_ptr<void> &sender, const std::shared_ptr<SizeChangedEventArgs> &e);

		void ChannelColor_MouseLeftButtonDown(const std::shared_ptr<void> &sender, const std::shared_ptr<MouseButtonEventArgs> &e);

		void GraphsCanvas_MouseWheel(const std::shared_ptr<void> &sender, const std::shared_ptr<MouseWheelEventArgs> &e);

		void GraphsCanvas_MouseDown(const std::shared_ptr<void> &sender, const std::shared_ptr<MouseButtonEventArgs> &e);

		void GraphsCanvas_MouseMove_Panning(const std::shared_ptr<void> &sender, const std::shared_ptr<MouseEventArgs> &e);

		void PanningLeft();

		void PanningRight();

		void StartButton_Click(const std::shared_ptr<void> &sender, const std::shared_ptr<RoutedEventArgs> &e);

		void LineButton_Click(const std::shared_ptr<void> &sender, const std::shared_ptr<RoutedEventArgs> &e);

		void GraphLine_MouseLeftButtonUp(const std::shared_ptr<void> &sender, const std::shared_ptr<MouseButtonEventArgs> &e);

		void GraphLine_MouseLeftButtonDown(const std::shared_ptr<void> &sender, const std::shared_ptr<MouseButtonEventArgs> &e);

		void TimeValueForLineTool_Loaded(const std::shared_ptr<void> &sender, const std::shared_ptr<RoutedEventArgs> &e);

		void MeasureToolSelect();

		void MeasureToolDeSelect();

		void LineToolSelect();

		void LineToolDeSelect();

		void TriggerButton_Click(const std::shared_ptr<void> &sender, const std::shared_ptr<RoutedEventArgs> &e);

		void Trigger_MouseLeftButtonDown(const std::shared_ptr<void> &sender, const std::shared_ptr<MouseButtonEventArgs> &e);

		void Name_LostKeyboardFocus(const std::shared_ptr<void> &sender, const std::shared_ptr<KeyboardFocusChangedEventArgs> &e);

		void InterfaceDataChange(int decoder);

		void InterfaceItemsListView_MouseDoubleClick(const std::shared_ptr<void> &sender, const std::shared_ptr<MouseButtonEventArgs> &e);

		void AddInterfaceButton_Click(const std::shared_ptr<void> &sender, const std::shared_ptr<RoutedEventArgs> &e);

		void AddI2CAnalyzer();

		void GetPointsAndAnalyze(int AnalyzerNumber);

		void InterfaceComboBox_SelectionChanged(const std::shared_ptr<void> &sender, const std::shared_ptr<SelectionChangedEventArgs> &e);

		void AddInterListView_MouseDoubleClick(const std::shared_ptr<void> &sender, const std::shared_ptr<MouseButtonEventArgs> &e);

		void RemoveInterfaceButton_Click(const std::shared_ptr<void> &sender, const std::shared_ptr<RoutedEventArgs> &e);

		void SearchButton_Click(const std::shared_ptr<void> &sender, const std::shared_ptr<RoutedEventArgs> &e);

		void NextResButton_Click(const std::shared_ptr<void> &sender, const std::shared_ptr<RoutedEventArgs> &e);

		void PrevResButton_Click(const std::shared_ptr<void> &sender, const std::shared_ptr<RoutedEventArgs> &e);

		void GoToInterfaceItem(int PointToGoTo);

		void ActiveInterfaceItemChange();

		void AnalyzerButton_Click(const std::shared_ptr<void> &sender, const std::shared_ptr<RoutedEventArgs> &e);

protected:
		std::shared_ptr<MainWindow> shared_from_this()
		{
			return std::static_pointer_cast<MainWindow>(Window::shared_from_this());
		}
	};
}
