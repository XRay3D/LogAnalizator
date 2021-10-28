#include "LogicAnalyzer.MainWindow.h"
#include "AnalyzerChannel.h"
#include "IHardwareInterface.h"
#include "USB_device.h"
#include "Measurement.h"
#include "LogicAnalyzer.ColorSelect.h"
#include "InterfaceDataItem.h"
#include "LogicAnalyzer.I2CAnalyzerSettings.h"
#include "I2C.h"


namespace LogicAnalyzer
{

std::vector<double> const MainWindow::ScaleX = {0.5, 1, 3, 5, 10, 20};
std::vector<double> const MainWindow::DescretTime = {10, 2.5, 1.25, 1, 0.625, 0.5, 1.0 / 3.0, 0.25, 1.0 / 6.0, 0.125};
std::vector<std::string> const MainWindow::TimeUnits = {"мкс", "мс"};
std::vector<std::string> const MainWindow::FreqStrings = {"100 кГц", "400 кГц", "800 кГц", "1 МГц", "1,6 МГц", "2 МГц", "3 МГц", "4 МГц", "6 МГц", "8 МГц"};
std::vector<std::string> const MainWindow::TriggersString = {"Х", "0", "1", "<", ">", "<>"};
const std::string MainWindow::TriggerHelp = std::string("Установите триггеры для каналов. ") + std::string("Для активации выберите активировать триггеры. ") + std::string("Если триггеры установлены для нескольких каналов, захват начнется при одновременном детектировании всех активных триггеров. \n") + TriggersString[TRIGGER_NONE] + std::string(" - триггер неактивен\n") + TriggersString[TRIGGER_LOW_LEVEL] + std::string(" - триггер по низкому уровню\n") + TriggersString[TRIGGER_HIGH_LEVEL] + std::string(" - триггер по высокому уровню\n") + TriggersString[TRIGGER_RISING_EDGE] + std::string(" - триггер по переднему фронту\n") + TriggersString[TRIGGER_FALLING_EDGE] + std::string(" - триггер по заднему фронту\n") + TriggersString[TRIGGER_ANY_EDGE] + std::string(" - триггер по любому фронту\n");
const double MainWindow::TOOLS_ELEMENT_OPACITY = 0.65;
const std::string MainWindow::TOOLS_ELEMENT_COLOR = "#FFADFF2F";
const std::string MainWindow::INTERFACE_TEXT_COLOR = "#FFFFFFFF";

	MainWindow::MainWindow()
	{
		InitializeComponent();

		Channels = std::vector<AnalyzerChannel>(CHANNEL_COUNT);
		IAnalyzers = std::make_shared<ObservableCollection<IHardwareInterface>>();

		for (unsigned char i = 0; i < CHANNEL_COUNT; i++)
		{
			Channels[i] = std::make_shared<AnalyzerChannel>();
			Channels[i]->setIndex(i);
			Channels[i]->setName(std::string("канал ") + std::to_string(i));
			Channels[i]->setTrigger(TriggersString[0]);
			Channels[i]->setStart_Y(START_Y);
			Channels[i]->setPulseHeight(PULSE_HEIGHT);
		}

		GraphsCanvas::MouseMove += GraphsCanvas_MouseMove_Panning;
		//demo starts
	/*
	            byte BitCnt = 0;
	            for (byte Chnl = 1; Chnl < CHANNEL_COUNT; Chnl++)
	            {
	                BitCnt = Chnl;
	                if (Chnl > 3)
	                    BitCnt -= 4;
	
	                PointCollection RawData = new PointCollection();
	
	                for (UInt32 i = 0; i < POINTS_COUNT; i++)
	                {
	                    if (BitCnt <= 1)
	                    {
	                        Point NewPoint1 = new Point(i * ScaleX[ScaleXIndex], START_Y);
	                        RawData.Add(NewPoint1);
	                    }
	                    else
	                    {
	                        Point NewPoint2 = new Point(i * ScaleX[ScaleXIndex], START_Y + PULSE_HEIGHT);
	                        RawData.Add(NewPoint2);
	                    }
	
	                    BitCnt++;
	                    if (BitCnt == 4)
	                        BitCnt = 0;
	                }
	
	                Channels[Chnl].CaptureData(RawData);
	            }
	
	            PointCollection CapturedRawData = new PointCollection();
	            for (UInt32 i = 0; i < POINTS_COUNT; i++)
	                CapturedRawData.Add(new Point(i * ScaleX[ScaleXIndex], START_Y + PULSE_HEIGHT));
	            CapturedRawData[120] = new Point(120 * ScaleX[ScaleXIndex], START_Y);
	            Channels[0].CaptureData(CapturedRawData);
	*/
		//demo ends

		ChnlInfo->ItemsSource = Channels;
		Samples->ItemsSource = SampleCount;
		Frequencies->ItemsSource = FreqStrings;
		InterfaceComboBox->ItemsSource = IAnalyzers;
		InterfaceComboBox->DisplayMemberPath = "Name";
		UpdateFrameParameters();

		ChnlsTrigHelpTextBox->Text = TriggerHelp;

		USBDevDetected = USB_device::Open();

		if (USBDevDetected)
		{

		}
	}

	void MainWindow::GraphMeasuring_MouseLeftButtonUp(const std::shared_ptr<void> &sender, const std::shared_ptr<MouseButtonEventArgs> &e)
	{
		if (ToolPolyline != nullptr)
		{
			while (!((std::dynamic_pointer_cast<Line>(GraphsCanvas::Children[GraphsCanvas::Children->Count - 1]) != nullptr) || (std::dynamic_pointer_cast<Label>(GraphsCanvas::Children[GraphsCanvas::Children->Count - 1]) != nullptr)))
			{
				GraphsCanvas::Children->RemoveAt(GraphsCanvas::Children->Count - 1);
			}

			ToolPolyline.reset();

			Mouse::Capture(nullptr);
		}
	}

	void MainWindow::GraphMeasuring_MouseMove(const std::shared_ptr<void> &sender, const std::shared_ptr<MouseEventArgs> &e)
	{
		if (((e->LeftButton == MouseButtonState::Pressed) && (e->MiddleButton == MouseButtonState::Released)))
		{
			if (ToolPolyline != nullptr)
			{
				ToolPolyline->Points[1] = Point(e->GetPosition(std::dynamic_pointer_cast<Canvas>(sender)).X, ToolPolyline->Points[0].Y);

				if (ToolPolyline->Points[1].X >= (ToolPolyline->Points[0].X + ScaleX[ScaleXIndex]))
				{
					if ((std::dynamic_pointer_cast<Polyline>(GraphsCanvas::Children[GraphsCanvas::Children->Count - 2]) != nullptr) && ((std::dynamic_pointer_cast<Polyline>(GraphsCanvas::Children[GraphsCanvas::Children->Count - 2]))->Name == "EndMeasMark"))
					{
						GraphsCanvas::Children->RemoveAt(GraphsCanvas::Children->Count - 2);
					}

					double SegmentStart = ToolPolyline->Points[0].X;
					double SegmentEnd = ToolPolyline->Points[1].X;
					int StartIndex = 0;
					int EndIndex = 0;

					if (SegmentStart > SegmentEnd)
					{
						double Value = SegmentStart;
						SegmentStart = SegmentEnd;
						SegmentEnd = Value;
					}

					unsigned char Chnl = static_cast<unsigned char>(ToolPolyline->Tag);
					for (int PointIndex = 0; PointIndex < Channels[Chnl]->getGraphPoints()->Count; PointIndex++)
					{
						if (SegmentStart <= Channels[Chnl]->getGraphPoints()[PointIndex].X)
						{
							StartIndex = PointIndex;
							break;
						}
					}

					for (int PointIndex = Channels[Chnl]->getGraphPoints()->Count - 1; PointIndex >= 0; PointIndex--)
					{
						if (SegmentEnd >= Channels[Chnl]->getGraphPoints()[PointIndex].X)
						{
							EndIndex = PointIndex;
							ToolPolyline->Points[1] = Point(Channels[Chnl]->getGraphPoints()[PointIndex].X, ToolPolyline->Points[1].Y);
							std::shared_ptr<Polyline> MeasuringMark = std::make_shared<Polyline>();
							MeasuringMark->StrokeThickness = 2;
							MeasuringMark->Stroke = ToolPolyline->Stroke;
							MeasuringMark->Name = "EndMeasMark";
							MeasuringMark->Opacity = TOOLS_ELEMENT_OPACITY;
							MeasuringMark->Points->Add(Point(ToolPolyline->Points[1].X, ToolPolyline->Points[1].Y - ((START_Y + PULSE_HEIGHT) / 2 + START_Y)));
							MeasuringMark->Points->Add(Point(ToolPolyline->Points[1].X, ToolPolyline->Points[1].Y + ((START_Y + PULSE_HEIGHT) / 2 + START_Y)));
							GraphsCanvas::Children->Add(MeasuringMark);
							break;
						}
					}

					if (std::dynamic_pointer_cast<StackPanel>(GraphsCanvas::Children[GraphsCanvas::Children->Count - 2]) != nullptr)
					{
						GraphsCanvas::Children->RemoveAt(GraphsCanvas::Children->Count - 2);
					}

					std::shared_ptr<PointCollection> PointsToMeasure = std::make_shared<PointCollection>();
					for (int PointIndex = StartIndex; PointIndex <= EndIndex; PointIndex++)
					{
						PointsToMeasure->Add(Channels[Chnl]->getGraphPoints()[PointIndex]);
					}

					std::shared_ptr<Measurement> MeasureStart = std::make_shared<Measurement>(PointsToMeasure, DescretTime[DescretTimeIndex], ScaleX[ScaleXIndex], PULSE_HEIGHT + START_Y);

					if (Chnl > 5)
					{
						Chnl -= 2;
					}
					MeasureStart->getResult()->Margin = Thickness(ToolPolyline->Points[0].X, ((CHANNEL_COUNT - Chnl - 1) * GRAPHS_GAP) - 0, 0, 0);
					MeasureStart->getResult()->Visibility = getVisibility()::Hidden;
					MeasureStart->getResult()->Loaded += MeasureResult_Loaded;
					GraphsCanvas::Children->Add(MeasureStart->getResult());
				}

				else
				{
					ToolPolyline->Points[1] = ToolPolyline->Points[0];
				}
			}
		}
	}

	void MainWindow::MeasureResult_Loaded(const std::shared_ptr<void> &sender, const std::shared_ptr<RoutedEventArgs> &e)
	{
		std::shared_ptr<StackPanel> MeasureResult = std::dynamic_pointer_cast<StackPanel>(sender);
		int Value = static_cast<int>(MeasureResult->ActualWidth);
		if ((MeasureResult->Margin.Left + Value) > GraphsCanvas::ActualWidth)
		{
			MeasureResult->Margin = Thickness(MeasureResult->Margin.Left - Value, MeasureResult->Margin.Top, 0, 0);
		}
		MeasureResult->Visibility = getVisibility()::Visible;
	}

	void MainWindow::GraphMeasuring_MouseLeftButtonDown(const std::shared_ptr<void> &sender, const std::shared_ptr<MouseButtonEventArgs> &e)
	{
		Point NewPoint = e->GetPosition(std::dynamic_pointer_cast<Canvas>(sender));
		Point TestPoint = Point();
		unsigned char Chnl;
		for (Chnl = 0; Chnl < CHANNEL_COUNT; Chnl++)
		{
			TestPoint = NewPoint;
			double TopMargin = (std::dynamic_pointer_cast<Polyline>(GraphsCanvas::Children[Chnl]))->Margin.Top;
			int LowLimit = START_Y + static_cast<int>(TopMargin);
			int HighLimit = LowLimit + PULSE_HEIGHT;
			if ((TestPoint.Y >= LowLimit) && (TestPoint.Y <= HighLimit))
			{
				TestPoint.Y = LowLimit + (HighLimit - LowLimit) / 2;
				break;
			}

			else
			{
				TestPoint.Y = -1;
			}
		}

		if (TestPoint.Y == -1)
		{
			return;
		}

		NewPoint.Y = TestPoint.Y;
		ToolPolyline = std::make_shared<Polyline>();
		ToolPolyline->StrokeThickness = 2;
		ToolPolyline->Stroke = std::make_shared<SolidColorBrush>(static_cast<Color>(ColorConverter::ConvertFromString(TOOLS_ELEMENT_COLOR)));
		ToolPolyline->Opacity = TOOLS_ELEMENT_OPACITY;
		ToolPolyline->Tag = Chnl;
		for (int PointIndex = 0; PointIndex < Channels[Chnl]->getGraphPoints()->Count; PointIndex++)
		{
			if (NewPoint.X <= Channels[Chnl]->getGraphPoints()[PointIndex].X)
			{
				NewPoint = Point(Channels[Chnl]->getGraphPoints()[PointIndex].X, NewPoint.Y);
				break;
			}
		}
		ToolPolyline->Points->Add(NewPoint);
		ToolPolyline->Points->Add(NewPoint);
		std::shared_ptr<Polyline> MeasuringMark = std::make_shared<Polyline>();
		MeasuringMark->StrokeThickness = 2;
		MeasuringMark->Stroke = ToolPolyline->Stroke;
		MeasuringMark->Opacity = TOOLS_ELEMENT_OPACITY;
		MeasuringMark->Points->Add(Point(ToolPolyline->Points[0].X, ToolPolyline->Points[0].Y - ((START_Y + PULSE_HEIGHT) / 2 + START_Y)));
		MeasuringMark->Points->Add(Point(ToolPolyline->Points[0].X, ToolPolyline->Points[0].Y + ((START_Y + PULSE_HEIGHT) / 2 + START_Y)));
		GraphsCanvas::Children->Add(MeasuringMark);
		GraphsCanvas::Children->Add(ToolPolyline);
		Mouse::Capture(GraphsCanvas, CaptureMode::Element);
	}

	void MainWindow::PrevFrameButton_Click(const std::shared_ptr<void> &sender, const std::shared_ptr<RoutedEventArgs> &e)
	{
		PanningLeft();
	}

	void MainWindow::NextFrameButton_Click(const std::shared_ptr<void> &sender, const std::shared_ptr<RoutedEventArgs> &e)
	{
		PanningRight();
	}

	void MainWindow::FrameChange()
	{
		for (unsigned char Chnl = 0; Chnl < CHANNEL_COUNT; Chnl++)
		{
			Channels[Chnl]->RedrawGraph(FrameStartPoint, FramePoints);
			(std::dynamic_pointer_cast<Polyline>(GraphsCanvas::Children[Chnl]))->Points = Channels[Chnl]->getGraphPoints();
		}

		double XOffset = Channels[0]->Get_X_Offset();

		GraphsCanvas::Children->RemoveRange(2 * CHANNEL_COUNT, GraphsCanvas::Children->Count - 2 * CHANNEL_COUNT);

		int TimeMarkCount = (End_X_Value - START_X_VALUE) / MainTimeMarkStep[ScaleXIndex];
		int TimeUnitsIndex;
		for (unsigned char TimeMark = 0; TimeMark <= TimeMarkCount; TimeMark++)
		{
			std::shared_ptr<Line> TimeMarkLine = std::make_shared<Line>();
			TimeMarkLine->X1 = MainTimeMarkStep[ScaleXIndex] * TimeMark;
			TimeMarkLine->Y1 = GRAPHS_GAP * (CHANNEL_COUNT + 1) - 20;
			TimeMarkLine->X2 = TimeMarkLine->X1;
			TimeMarkLine->Y2 = TimeMarkLine->Y1 - MAIN_TIME_MARK_HEIGHT;
			TimeMarkLine->Stroke = std::make_shared<SolidColorBrush>(Colors::Gray);
			TimeMarkLine->StrokeThickness = 2;
			if (TimeMarkLine->X1 < End_X_Value)
			{
				GraphsCanvas::Children->Add(TimeMarkLine);
				std::shared_ptr<Label> TimeLabel = std::make_shared<Label>();
				double TimeValue = DescretTime[DescretTimeIndex] * ((TimeMarkLine->X1 + XOffset) / ScaleX[ScaleXIndex]);
				TimeUnitsIndex = 0;
				if (TimeValue >= 1000)
				{
					TimeValue /= 1000;
					TimeUnitsIndex = 1;
				}
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
				TimeLabel->Content = (TimeValue).ToString("0.000") + TimeUnits[TimeUnitsIndex];
				TimeLabel->FontSize = 12;
				TimeLabel->Foreground = std::make_shared<SolidColorBrush>(static_cast<Color>(ColorConverter::ConvertFromString(TOOLS_ELEMENT_COLOR)));
				TimeLabel->Margin = Thickness(TimeMarkLine->X1 - 3, TimeMarkLine->Y2 + 10, 0, 0);
				TimeLabel->RenderTransform = std::make_shared<ScaleTransform>(1, -1);
				GraphsCanvas::Children->Add(TimeLabel);
			}

			for (unsigned char SmallTimeMark = 1; SmallTimeMark < 10; SmallTimeMark++)
			{
				std::shared_ptr<Line> SmallTimeMarkLine = std::make_shared<Line>();
				SmallTimeMarkLine->X1 = TimeMarkLine->X1 + SmallTimeMarkStep[ScaleXIndex] * SmallTimeMark;
				SmallTimeMarkLine->Y1 = TimeMarkLine->Y1;
				SmallTimeMarkLine->X2 = SmallTimeMarkLine->X1;
				SmallTimeMarkLine->Y2 = SmallTimeMarkLine->Y1 - SMALL_TIME_MARK_HEIGHT;
				SmallTimeMarkLine->Stroke = std::make_shared<SolidColorBrush>(Colors::Gray);
				SmallTimeMarkLine->StrokeThickness = 2;
				if (SmallTimeMarkLine->X1 > (End_X_Value - SmallTimeMarkStep[ScaleXIndex]))
				{
					break;
				}
				GraphsCanvas::Children->Add(SmallTimeMarkLine);
			}
		}

		for (int decoder = 0; decoder < IAnalyzers->Count; decoder++)
		{
			InterfaceDataChange(decoder);
		}
	}

	void MainWindow::UpdateFrameParameters()
	{
		FramePoints = static_cast<int>((End_X_Value - START_X_VALUE) / ScaleX[ScaleXIndex]);
		if (FramePoints > SamplesToCapture)
		{
			FramePoints = SamplesToCapture;
		}
		UpdatePoints = static_cast<int>(((ScaleX.size() - ScaleXIndex) * MainTimeMarkStep[ScaleXIndex]) / ScaleX[ScaleXIndex]);
	}

	void MainWindow::ZoomOutButton_Click(const std::shared_ptr<void> &sender, const std::shared_ptr<RoutedEventArgs> &e)
	{
		Zooming(ZOOM_OUT);
	}

	void MainWindow::ZoomInButton_Click(const std::shared_ptr<void> &sender, const std::shared_ptr<RoutedEventArgs> &e)
	{
		Zooming(ZOOM_IN);
	}

	void MainWindow::Zooming(unsigned char Direction)
	{
		if (((Direction == ZOOM_OUT) && (ScaleXIndex != 0)) || ((Direction == ZOOM_IN) && (ScaleXIndex != (ScaleX.size() - 1))))
		{
			if (Direction == ZOOM_OUT)
			{
				ScaleXIndex--;
			}
			else
			{
				ScaleXIndex++;
			}

			for (unsigned char Chnl = 0; Chnl < CHANNEL_COUNT; Chnl++)
			{
				Channels[Chnl]->Scale(ScaleX[ScaleXIndex]);
			}

			UpdateFrameParameters();
			if ((FrameStartPoint + FramePoints) >= SamplesToCapture)
			{
				FrameStartPoint = SamplesToCapture - FramePoints;
			}
			FrameChange();
		}
	}

	void MainWindow::MeasureButton_Click(const std::shared_ptr<void> &sender, const std::shared_ptr<RoutedEventArgs> &e)
	{
		if (MeasuringActive)
		{
			MeasureToolDeSelect();
		}
		else
		{
			if (LineActive)
			{
				LineToolDeSelect();
			}

			MeasureToolSelect();
		}
	}

	void MainWindow::GraphsCanvasSizeChanged(const std::shared_ptr<void> &sender, const std::shared_ptr<SizeChangedEventArgs> &e)
	{
		End_X_Value = static_cast<int>(GraphsCanvas::ActualWidth);

		UpdateFrameParameters();
		if ((FrameStartPoint + FramePoints) >= SamplesToCapture)
		{
			FrameStartPoint = SamplesToCapture - FramePoints;
		}
		FrameChange();
	}

	void MainWindow::ChannelColor_MouseLeftButtonDown(const std::shared_ptr<void> &sender, const std::shared_ptr<MouseButtonEventArgs> &e)
	{
		std::shared_ptr<Border> SenderBorder = std::dynamic_pointer_cast<Border>(sender);

		std::shared_ptr<ColorSelect> ColorWindow = std::make_shared<ColorSelect>(SenderBorder->Background);
		ColorWindow->setOwner(shared_from_this());
		if ((ColorWindow->ShowDialog() == true) && (ColorWindow->getNewColor() != nullptr))
		{
			SenderBorder->Background = ColorWindow->getNewColor();
			(std::dynamic_pointer_cast<Polyline>(GraphsCanvas::Children[std::stoi(SenderBorder->Uid)]))->Stroke = SenderBorder->Background;
		}
	}

	void MainWindow::GraphsCanvas_MouseWheel(const std::shared_ptr<void> &sender, const std::shared_ptr<MouseWheelEventArgs> &e)
	{
		if (e->Delta > 0)
		{
			Zooming(ZOOM_IN);
		}
		else
		{
			Zooming(ZOOM_OUT);
		}
	}

	void MainWindow::GraphsCanvas_MouseDown(const std::shared_ptr<void> &sender, const std::shared_ptr<MouseButtonEventArgs> &e)
	{
		if ((e->ChangedButton == MouseButton::Middle) && (e->ButtonState == MouseButtonState::Pressed))
		{
			Point StartPoint = e->GetPosition(std::dynamic_pointer_cast<Canvas>(sender));
			PanPolyline->Points->Add(StartPoint);
		}
	}

	void MainWindow::GraphsCanvas_MouseMove_Panning(const std::shared_ptr<void> &sender, const std::shared_ptr<MouseEventArgs> &e)
	{
		if ((e->MiddleButton == MouseButtonState::Pressed))
		{
			Point EndPoint = e->GetPosition(std::dynamic_pointer_cast<Canvas>(sender));
			PanPolyline->Points->Add(EndPoint);
			if ((PanPolyline->Points[0] - PanPolyline->Points[PanPolyline->Points->Count - 1]).Length > 50)
			{
				if (PanPolyline->Points[0].X > PanPolyline->Points[PanPolyline->Points->Count - 1].X)
				{
					PanningRight();
				}
				else
				{
					PanningLeft();
				}

				EndPoint = PanPolyline->Points[PanPolyline->Points->Count - 1];
				PanPolyline->Points->Clear();
				PanPolyline->Points->Add(EndPoint);
			}
		}
	}

	void MainWindow::PanningLeft()
	{
		if (FrameStartPoint != 0)
		{
			if (FrameStartPoint > UpdatePoints)
			{
				FrameStartPoint -= UpdatePoints;
			}
			else
			{
				FrameStartPoint = 0;
			}

			FrameChange();
		}
	}

	void MainWindow::PanningRight()
	{
		if (FrameStartPoint < SamplesToCapture)
		{
			FrameStartPoint += UpdatePoints;

			if ((FrameStartPoint + FramePoints) >= SamplesToCapture)
			{
				FrameStartPoint = SamplesToCapture - FramePoints;
			}

			FrameChange();
		}
	}

	void MainWindow::StartButton_Click(const std::shared_ptr<void> &sender, const std::shared_ptr<RoutedEventArgs> &e)
	{
		if (USBDevDetected)
		{
			std::vector<unsigned char> USBPacket(USB_CMD_PACKET_SIZE);

			USBPacket[USB_REPORT_ID_POS] = USB_CMD_ID;
			USBPacket[USB_CMD_POS] = USB_CMD_START_CAPTURE;

			int Value = Frequencies::SelectedIndex;
			USBPacket[CAPTURE_SYNC_OFFSET] = CAPTURE_SYNC_INTERNAL;
			USBPacket[CAPTURE_TIM_PSC_OFFSET] = CaptureTimPsc[Value];
			USBPacket[CAPTURE_TIM_ARR_OFFSET] = CaptureTimArr[Value];
			DescretTimeIndex = Value;

			SamplesToCapture = SampleCount[Samples::SelectedIndex];
			USBPacket[CAPTURE_SAMPLE_OFFSET] = static_cast<unsigned char>(SamplesToCapture);
			USBPacket[CAPTURE_SAMPLE_OFFSET + 1] = static_cast<unsigned char>(SamplesToCapture >> 8);
			if (TriggerActivationCheckBox->IsChecked == true)
			{
				USBPacket[CAPTURE_TRIGGER_ENABLE_OFFSET] = CAPTURE_TRIGGER_ENABLE;
				USBPacket[CAPTURE_TRIGGER_MODE_OFFSET] = CAPTURE_TRIGGER_MODE_CHANNELS;
				for (int TrigByte = 0; TrigByte < CAPTURE_TRIGGER_BYTES_COUNT; TrigByte++)
				{
					USBPacket[CAPTURE_TRIGGER_SET_OFFSET + TrigByte] = static_cast<unsigned char>((Array::IndexOf(TriggersString, Channels[2 * TrigByte + 1]->getTrigger()) << 4) | (Array::IndexOf(TriggersString, Channels[2 * TrigByte]->getTrigger())));
				}
				if ((USBPacket[CAPTURE_TRIGGER_SET_OFFSET] == 0) && (USBPacket[CAPTURE_TRIGGER_SET_OFFSET + 1] == 0) && (USBPacket[CAPTURE_TRIGGER_SET_OFFSET + 2] == 0) && (USBPacket[CAPTURE_TRIGGER_SET_OFFSET + 3] == 0))
				{
					USBPacket[CAPTURE_TRIGGER_ENABLE_OFFSET] = CAPTURE_TRIGGER_DISABLE;
				}
			}

			else
			{
				USBPacket[CAPTURE_TRIGGER_ENABLE_OFFSET] = CAPTURE_TRIGGER_DISABLE;
			}

			bool USBSuccess = USB_device::Write(USBPacket);

			std::vector<unsigned char> InputData(SamplesToCapture);

			Value = SamplesToCapture / USB_CMD_PACKET_SIZE;

			for (int i = 0; i < Value; i++)
			{
				USBSuccess = USB_device::Read(USBPacket);
				USBPacket.CopyTo(InputData, i * USB_CMD_PACKET_SIZE);
			}

			std::vector<PointCollection> NewChannelsData(CHANNEL_COUNT);
			for (int chnl = 0; chnl < CHANNEL_COUNT; chnl++)
			{
				NewChannelsData[chnl] = std::make_shared<PointCollection>();
			}

			for (int PointIndex = 0; PointIndex < SamplesToCapture; PointIndex++)
			{
				for (int chnl = 0; chnl < CHANNEL_COUNT; chnl++)
				{
					Value = START_Y;
					if ((InputData[PointIndex] & (1 << chnl)) == (1 << chnl))
					{
						Value += PULSE_HEIGHT;
					}
					NewChannelsData[chnl]->Add(Point(PointIndex, Value));
				}
			}

			//если активированы триггеры, в начало графика добавляются 2 точки,
			//соответствующие триггеру канала;
			//если у канала нет триггера, добавленные точки повторяют начальную точку канала
			if (TriggerActivationCheckBox->IsChecked == true)
			{
				for (int chnl = 0; chnl < CHANNEL_COUNT; chnl++)
				{
					for (int PointIndex = 0; PointIndex < SamplesToCapture; PointIndex++)
					{
						NewChannelsData[chnl][PointIndex] = Point(NewChannelsData[chnl][PointIndex]->X + EXTRA_POINTS_COUNT_FOR_TRIGGER, NewChannelsData[chnl][PointIndex]->Y);
					}
				}

				SamplesToCapture += EXTRA_POINTS_COUNT_FOR_TRIGGER;

				for (int chnl = 0; chnl < CHANNEL_COUNT; chnl++)
				{
					switch (Array::IndexOf(TriggersString, Channels[chnl]->getTrigger()))
					{
						case TRIGGER_NONE:
							NewChannelsData[chnl]->Insert(0, Point(1, NewChannelsData[chnl][0]->Y));
							NewChannelsData[chnl]->Insert(0, Point(0, NewChannelsData[chnl][0]->Y));
						break;

						case TRIGGER_LOW_LEVEL:
							NewChannelsData[chnl]->Insert(0, Point(1, PULSE_LOW));
							NewChannelsData[chnl]->Insert(0, Point(0, PULSE_LOW));
						break;

						case TRIGGER_HIGH_LEVEL:
							NewChannelsData[chnl]->Insert(0, Point(1, PULSE_HIGH));
							NewChannelsData[chnl]->Insert(0, Point(0, PULSE_HIGH));
						break;

						case TRIGGER_RISING_EDGE:
							NewChannelsData[chnl]->Insert(0, Point(1, PULSE_HIGH));
							NewChannelsData[chnl]->Insert(0, Point(0, PULSE_LOW));
						break;

						case TRIGGER_FALLING_EDGE:
						case TRIGGER_ANY_EDGE:
							NewChannelsData[chnl]->Insert(0, Point(1, PULSE_LOW));
							NewChannelsData[chnl]->Insert(0, Point(0, PULSE_HIGH));
						break;
					}
				}
			}

			for (int chnl = 0; chnl < CHANNEL_COUNT; chnl++)
			{
				Channels[chnl]->CaptureData(NewChannelsData[chnl]);
			}

			for (int decoder = 0; decoder < IAnalyzers->Count; decoder++)
			{
				GetPointsAndAnalyze(decoder);

				if (decoder == InterfaceComboBox::SelectedIndex)
				{
					InterfaceItemsListView->ItemsSource = IAnalyzers[decoder]->getInterfaceParts();
				}
			}

			ScaleXIndex = 1;
			Zooming(ZOOM_OUT);
		}
	}

	void MainWindow::LineButton_Click(const std::shared_ptr<void> &sender, const std::shared_ptr<RoutedEventArgs> &e)
	{
		if (LineActive)
		{
			LineToolDeSelect();
		}
		else
		{
			if (MeasuringActive)
			{
				MeasureToolDeSelect();
			}

			LineToolSelect();
		}
	}

	void MainWindow::GraphLine_MouseLeftButtonUp(const std::shared_ptr<void> &sender, const std::shared_ptr<MouseButtonEventArgs> &e)
	{
		if (ToolPolyline != nullptr)
		{
			GraphsCanvas::Children->RemoveAt(GraphsCanvas::Children->Count - 1);
			GraphsCanvas::Children->RemoveAt(GraphsCanvas::Children->Count - 1);
			ToolPolyline.reset();
			Mouse::Capture(nullptr);
		}
	}

	void MainWindow::GraphLine_MouseLeftButtonDown(const std::shared_ptr<void> &sender, const std::shared_ptr<MouseButtonEventArgs> &e)
	{
		ToolPolyline = std::make_shared<Polyline>();
		ToolPolyline->StrokeThickness = 2;
		ToolPolyline->Stroke = std::make_shared<SolidColorBrush>(static_cast<Color>(ColorConverter::ConvertFromString(TOOLS_ELEMENT_COLOR)));
		ToolPolyline->Opacity = TOOLS_ELEMENT_OPACITY;
		ToolPolyline->StrokeDashArray = {10, 5};

		double NewXPos = e->GetPosition(std::dynamic_pointer_cast<Canvas>(sender)).X;

		for (int PointIndex = 0; PointIndex < Channels[0]->getGraphPoints()->Count; PointIndex++)
		{
			if (NewXPos <= Channels[0]->getGraphPoints()[PointIndex].X)
			{
				NewXPos = Channels[0]->getGraphPoints()[PointIndex].X;
				break;
			}
		}

		ToolPolyline->Points->Add(Point(NewXPos, 0));
		ToolPolyline->Points->Add(Point(NewXPos, GraphsCanvas::ActualHeight));
		GraphsCanvas::Children->Add(ToolPolyline);
		std::shared_ptr<Label> TimeValue = std::make_shared<Label>();
		TimeValue->Foreground = std::make_shared<SolidColorBrush>(static_cast<Color>(ColorConverter::ConvertFromString(TOOLS_ELEMENT_COLOR)));
		TimeValue->Margin = Thickness(NewXPos + 5, e->GetPosition(std::dynamic_pointer_cast<Canvas>(sender)).Y, 0, 0);
		TimeValue->RenderTransform = std::make_shared<ScaleTransform>(1, -1);
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
		TimeValue->Content = (DescretTime[DescretTimeIndex] * ((Channels[0]->Get_X_Offset() + NewXPos) / ScaleX[ScaleXIndex])).ToString("0.000") + std::string(" мкс");
		TimeValue->Visibility = getVisibility()::Hidden;
		TimeValue->Loaded += TimeValueForLineTool_Loaded;
		GraphsCanvas::Children->Add(TimeValue);
		Mouse::Capture(GraphsCanvas, CaptureMode::Element);
	}

	void MainWindow::TimeValueForLineTool_Loaded(const std::shared_ptr<void> &sender, const std::shared_ptr<RoutedEventArgs> &e)
	{
		std::shared_ptr<Label> TimeValueForLineTool = (std::dynamic_pointer_cast<Label>(sender));
		int Value = static_cast<int>(TimeValueForLineTool->ActualWidth);
		if ((TimeValueForLineTool->Margin.Left + Value) > GraphsCanvas::ActualWidth)
		{
			TimeValueForLineTool->Margin = Thickness(TimeValueForLineTool->Margin.Left - Value - 10, TimeValueForLineTool->Margin.Top, 0, 0);
		}
		TimeValueForLineTool->Visibility = getVisibility()::Visible;
	}

	void MainWindow::MeasureToolSelect()
	{
		MeasuringActive = true;
		GraphsCanvas::MouseLeftButtonDown += GraphMeasuring_MouseLeftButtonDown;
		GraphsCanvas::MouseMove += GraphMeasuring_MouseMove;
		GraphsCanvas::MouseLeftButtonUp += GraphMeasuring_MouseLeftButtonUp;
		GraphsCanvas->Cursor = Cursors::Cross;
	}

	void MainWindow::MeasureToolDeSelect()
	{
		MeasuringActive = false;
		GraphsCanvas::MouseLeftButtonDown -= GraphMeasuring_MouseLeftButtonDown;
		GraphsCanvas::MouseMove -= GraphMeasuring_MouseMove;
		GraphsCanvas::MouseLeftButtonUp -= GraphMeasuring_MouseLeftButtonUp;
		GraphsCanvas->Cursor = Cursors::Arrow;
	}

	void MainWindow::LineToolSelect()
	{
		LineActive = true;
		GraphsCanvas::MouseLeftButtonDown += GraphLine_MouseLeftButtonDown;
		GraphsCanvas::MouseLeftButtonUp += GraphLine_MouseLeftButtonUp;
		GraphsCanvas->Cursor = Cursors::Cross;
	}

	void MainWindow::LineToolDeSelect()
	{
		LineActive = false;
		GraphsCanvas::MouseLeftButtonDown -= GraphLine_MouseLeftButtonDown;
		GraphsCanvas::MouseLeftButtonUp -= GraphLine_MouseLeftButtonUp;
		GraphsCanvas->Cursor = Cursors::Arrow;
	}

	void MainWindow::TriggerButton_Click(const std::shared_ptr<void> &sender, const std::shared_ptr<RoutedEventArgs> &e)
	{
		if (TriggerBorderMain->Visibility == getVisibility()::Collapsed)
		{
			TriggerBorderMain->Visibility = getVisibility()::Visible;
		}
		else if (TriggerBorderMain->Visibility == getVisibility()::Visible)
		{
			TriggerBorderMain->Visibility = getVisibility()::Collapsed;
		}
	}

	void MainWindow::Trigger_MouseLeftButtonDown(const std::shared_ptr<void> &sender, const std::shared_ptr<MouseButtonEventArgs> &e)
	{
		std::shared_ptr<TextBlock> ChannelTrigger = std::dynamic_pointer_cast<TextBlock>(sender);

		int NewTriggerIndex = Array::IndexOf(TriggersString, ChannelTrigger->Text);
		NewTriggerIndex++;
		if (NewTriggerIndex == TriggersString.size())
		{
			NewTriggerIndex = 0;
		}

		ChannelTrigger->Text = TriggersString[NewTriggerIndex];
	}

	void MainWindow::Name_LostKeyboardFocus(const std::shared_ptr<void> &sender, const std::shared_ptr<KeyboardFocusChangedEventArgs> &e)
	{
		std::shared_ptr<TextBox> ChannelName = std::dynamic_pointer_cast<TextBox>(sender);
		if (ChannelName->Text == "")
		{
			ChannelName->Text = std::string("канал ") + ChannelName->Uid;
		}
	}

	void MainWindow::InterfaceDataChange(int decoder)
	{
		if (IAnalyzers[decoder]->getInterfaceParts()->Count != 0)
		{
			for (auto dataline : IAnalyzers[decoder]->getDataLinesNumbers())
			{
				(std::dynamic_pointer_cast<Grid>(GraphsCanvas::Children[CHANNEL_COUNT + dataline]))->Children->Clear();
			}

			int StartItem = 0;
			int EndItem = 0;

			for (int i = 0; i < IAnalyzers[decoder]->getInterfaceParts()->Count; i++)
			{
				if (IAnalyzers[decoder]->getInterfaceParts()[i].getStartPoint() >= FrameStartPoint)
				{
					StartItem = i;
					break;
				}
			}

			for (int i = IAnalyzers[decoder]->getInterfaceParts()->Count - 1; i > StartItem; i--)
			{
				if (IAnalyzers[decoder]->getInterfaceParts()[i].getStartPoint() <= (FrameStartPoint + FramePoints))
				{
					EndItem = i;
					break;
				}
			}

			for (int i = StartItem; i <= EndItem; i++)
			{
				std::shared_ptr<TextBlock> FirstTExtItem = std::make_shared<TextBlock>();
				FirstTExtItem->Text = IAnalyzers[decoder]->getInterfaceParts()[i].getItem();
				FirstTExtItem->FontSize = 12;
				FirstTExtItem->Foreground = std::make_shared<SolidColorBrush>(static_cast<Color>(ColorConverter::ConvertFromString(INTERFACE_TEXT_COLOR)));
				FirstTExtItem->HorizontalAlignment = getHorizontalAlignment()::Center;

				std::shared_ptr<Border> NewBorder = std::make_shared<Border>();
				NewBorder->CornerRadius = CornerRadius(7);
				NewBorder->Child = FirstTExtItem;
				NewBorder->Width = IAnalyzers[decoder]->getInterfaceParts()[i].getWidth() * ScaleX[ScaleXIndex];
				NewBorder->Background = std::make_shared<SolidColorBrush>(static_cast<Color>(ColorConverter::ConvertFromString(IAnalyzers[decoder]->getInterfaceParts()[i].getBackground())));
				NewBorder->Margin = Thickness((IAnalyzers[decoder]->getInterfaceParts()[i].getStartPoint() - FrameStartPoint) * ScaleX[ScaleXIndex], 15, 0, 0);
				NewBorder->HorizontalAlignment = getHorizontalAlignment()::Left;
				NewBorder->RenderTransform = std::make_shared<ScaleTransform>(1, -1);
				for (auto dataline : IAnalyzers[decoder]->getDataLinesNumbers())
				{
					(std::dynamic_pointer_cast<Grid>(GraphsCanvas::Children[CHANNEL_COUNT + dataline]))->Children->Add(NewBorder);
				}
			}
		}
	}

	void MainWindow::InterfaceItemsListView_MouseDoubleClick(const std::shared_ptr<void> &sender, const std::shared_ptr<MouseButtonEventArgs> &e)
	{
		if (e->ChangedButton == MouseButton::Left)
		{
			GoToInterfaceItem((std::dynamic_pointer_cast<InterfaceItem>(InterfaceItemsListView::SelectedItem))->getStartPoint());
		}
	}

	void MainWindow::AddInterfaceButton_Click(const std::shared_ptr<void> &sender, const std::shared_ptr<RoutedEventArgs> &e)
	{
		AddInterListView->Visibility = getVisibility()::Visible;
	}

	void MainWindow::AddI2CAnalyzer()
	{
		std::shared_ptr<I2CAnalyzerSettings> AnalyzerWin = std::make_shared<I2CAnalyzerSettings>();
		AnalyzerWin->setOwner(shared_from_this());

		if (AnalyzerWin->ShowDialog() == true)
		{
			std::shared_ptr<IHardwareInterface> NewI2C = std::make_shared<I2C>();
			NewI2C->setName(AnalyzerWin->getName() + std::string("(") + NewI2C->getType() + std::string(")"));
			NewI2C->Initialize(AnalyzerWin->getInterfaceLines());
			NewI2C->setInterfaceVisibility(AnalyzerWin->getI2CInterVisibility());
			IAnalyzers->Add(NewI2C);
			GetPointsAndAnalyze(IAnalyzers->Count - 1);
			InterfaceDataChange(IAnalyzers->Count - 1);
		}
	}

	void MainWindow::GetPointsAndAnalyze(int AnalyzerNumber)
	{
		std::vector<PointCollection> SignalPoints;
		for (auto lineNum : IAnalyzers[AnalyzerNumber]->getAllLinesNumbers())
		{
			SignalPoints.push_back(Channels[lineNum]->GetCapturedData());
		}
		IAnalyzers[AnalyzerNumber]->Analyze(SignalPoints, DescretTime[DescretTimeIndex]);
	}

	void MainWindow::InterfaceComboBox_SelectionChanged(const std::shared_ptr<void> &sender, const std::shared_ptr<SelectionChangedEventArgs> &e)
	{
		if (InterfaceComboBox::SelectedIndex != -1)
		{
			InterfaceItemsListView->ItemsSource = IAnalyzers[InterfaceComboBox::SelectedIndex]->getInterfaceParts();
		}
	}

	void MainWindow::AddInterListView_MouseDoubleClick(const std::shared_ptr<void> &sender, const std::shared_ptr<MouseButtonEventArgs> &e)
	{
		if (e->ChangedButton == MouseButton::Left)
		{
			int InterfaceIndex = AddInterListView::SelectedIndex;
			AddInterListView->Visibility = getVisibility()::Collapsed;

			if (InterfaceIndex != (AddInterListView::Items->Count - 1))
			{
				switch (InterfaceIndex)
				{
					case 0: //I2C
						AddI2CAnalyzer();
						break;
				}
			}
		}
	}

	void MainWindow::RemoveInterfaceButton_Click(const std::shared_ptr<void> &sender, const std::shared_ptr<RoutedEventArgs> &e)
	{
		if (InterfaceComboBox::SelectedIndex != -1)
		{
			for (auto dataline : IAnalyzers[InterfaceComboBox::SelectedIndex]->getDataLinesNumbers())
			{
				(std::dynamic_pointer_cast<Grid>(GraphsCanvas::Children[CHANNEL_COUNT + dataline]))->Children->Clear();
			}
			InterfaceItemsListView->ItemsSource = nullptr;
			IAnalyzers->RemoveAt(InterfaceComboBox::SelectedIndex);
			MatchingCountTextBlock->Text = "";
			SearchTextBox->Clear();
		}
	}

	void MainWindow::SearchButton_Click(const std::shared_ptr<void> &sender, const std::shared_ptr<RoutedEventArgs> &e)
	{
		if (InterfaceComboBox::SelectedIndex != -1)
		{
			IAnalyzers[InterfaceComboBox::SelectedIndex]->SearchItem(SearchTextBox->Text);
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
			MatchingCountTextBlock->Text = IAnalyzers[InterfaceComboBox::SelectedIndex]->getMatchedItemsCount().ToString();
			ActiveInterfaceItemChange();
		}
	}

	void MainWindow::NextResButton_Click(const std::shared_ptr<void> &sender, const std::shared_ptr<RoutedEventArgs> &e)
	{
		IAnalyzers[InterfaceComboBox::SelectedIndex]->NextMatchItem();
		ActiveInterfaceItemChange();
	}

	void MainWindow::PrevResButton_Click(const std::shared_ptr<void> &sender, const std::shared_ptr<RoutedEventArgs> &e)
	{
		IAnalyzers[InterfaceComboBox::SelectedIndex]->PreviousMatchItem();
		ActiveInterfaceItemChange();
	}

	void MainWindow::GoToInterfaceItem(int PointToGoTo)
	{
		FrameStartPoint = PointToGoTo;

		if ((FrameStartPoint + FramePoints) >= SamplesToCapture)
		{
			FrameStartPoint = SamplesToCapture - FramePoints;
		}

		FrameChange();
	}

	void MainWindow::ActiveInterfaceItemChange()
	{
		InterfaceItemsListView->SelectedIndex = IAnalyzers[InterfaceComboBox::SelectedIndex]->getCurrentMatchedItemIndex();
		InterfaceItemsListView::ScrollIntoView(InterfaceItemsListView::SelectedItem);
		GoToInterfaceItem((std::dynamic_pointer_cast<InterfaceItem>(InterfaceItemsListView::SelectedItem))->getStartPoint());
	}

	void MainWindow::AnalyzerButton_Click(const std::shared_ptr<void> &sender, const std::shared_ptr<RoutedEventArgs> &e)
	{
		if (AnalyzerBorderMain->Visibility == getVisibility()::Collapsed)
		{
			AnalyzerBorderMain->Visibility = getVisibility()::Visible;
		}
		else if (AnalyzerBorderMain->Visibility == getVisibility()::Visible)
		{
			AnalyzerBorderMain->Visibility = getVisibility()::Collapsed;
		}
	}
}
