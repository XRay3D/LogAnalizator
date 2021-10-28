#include "Measurement.h"


namespace LogicAnalyzer
{

	Measurement::Measurement(const std::shared_ptr<PointCollection> &GraphSegment, double TimeRes, double ScaleFactor, int HighLevel)
	{
		setResult(std::make_shared<StackPanel>());

		if (GraphSegment->Count != 0)
		{
			InputData = std::make_shared<PointCollection>();
			for (int PointIndex = 0; PointIndex < GraphSegment->Count; PointIndex++)
			{
				Point NewPoint = Point(GraphSegment[PointIndex].X / ScaleFactor, GraphSegment[PointIndex].Y);
				InputData->Add(NewPoint);
			}

			TimeResolution = TimeRes;
			PulseHighLevel = HighLevel;

			getResult()->Background = std::make_shared<SolidColorBrush>(static_cast<Color>(ColorConverter::ConvertFromString("#FFADFF2F")));
			getResult()->Name = "MeasureGrid";
			getResult()->Opacity = 0.65; // 0.75;
			getResult()->Orientation = Orientation::Vertical;
			getResult()->RenderTransform = std::make_shared<ScaleTransform>(1, -1);
			double Duration = GetDuration(InputData[0].X, InputData[InputData->Count - 1].X);
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
			AddData(std::string("Ширина: ") + Duration.ToString("0.000") + std::string(" мкс"));

			int EdgeCount;
			int RisingEdgeCount;
			int FallingEdgeCount;
			int PulseCntResult = PulseCounter(EdgeCount, RisingEdgeCount, FallingEdgeCount);

			if (PulseCntResult == ONE_PULSE)
			{
				double DutyCycle;
				double Frequency;
				double Period;
				double PulseWidth;

				Period = Duration;
				Frequency = 1000 / Period; //кГц
				GetPulseWidth(PulseWidth);
				DutyCycle = 100 * PulseWidth / Period; //%

//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
				AddData(std::string("Период: ") + Period.ToString("0.000") + std::string(" мкс"));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
				AddData(std::string("Частота: ") + Frequency.ToString("0.000") + std::string(" кГц"));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
				AddData(std::string("Длительность импульса: ") + DutyCycle.ToString("0.000") + std::string(" % / ") + PulseWidth.ToString("0.000") + std::string(" мкс"));
			}

			else if (PulseCntResult == MANY_PULSES)
			{
				AddData(std::string("Количество фронтов: ") + std::to_string(EdgeCount));
				AddData(std::string("Передних фронтов: ") + std::to_string(RisingEdgeCount));
				AddData(std::string("Задних фронтов: ") + std::to_string(FallingEdgeCount));
			}
		}
	}

	const std::shared_ptr<StackPanel> &Measurement::getResult() const
	{
		return privateResult;
	}

	void Measurement::setResult(const std::shared_ptr<StackPanel> &value)
	{
		privateResult = value;
	}

	double Measurement::GetDuration(double X1, double X2)
	{
		return ((X2 - X1) * TimeResolution);
	}

	int Measurement::PulseCounter(int &Edges, int &Rising, int &Falling)
	{
		int ReturnCode = LEVEL;
		Edges = 0;
		Rising = 0;
		Falling = 0;

		for (int DataPoint = 0; DataPoint < (InputData->Count - 1); DataPoint++)
		{
			if (InputData[DataPoint].Y > InputData[DataPoint + 1].Y)
			{
				Falling++;
			}
			else if (InputData[DataPoint].Y < InputData[DataPoint + 1].Y)
			{
				Rising++;
			}
		}

		Edges = Rising + Falling;

		if (Edges == 0)
		{
			return ReturnCode;
		}

		if ((Edges == 3) && (InputData[0].X == InputData[1].X) && (InputData[InputData->Count - 2].X == InputData[InputData->Count - 1].X))
		{
			ReturnCode = ONE_PULSE;
			return ReturnCode;
		}

		ReturnCode = MANY_PULSES;

		return ReturnCode;
	}

	void Measurement::GetPulseWidth(double &PulseWidth)
	{
		PulseWidth = 0;

		bool StartPoint = false;
		double HighLevelStart = 0;

		for (int PointIndex = 0; PointIndex < (InputData->Count - 1); PointIndex++)
		{
			if ((InputData[PointIndex].Y == PulseHighLevel) && (InputData[PointIndex + 1].Y == PulseHighLevel))
			{
				if (StartPoint == false)
				{
					StartPoint = true;
					HighLevelStart = InputData[PointIndex].X;
				}
			}

			else if ((InputData[PointIndex].Y == PulseHighLevel) && ((InputData[PointIndex + 1].Y != PulseHighLevel) || ((PointIndex + 1) == (InputData->Count - 1))))
			{
				if (StartPoint == true)
				{
					StartPoint = false;
					PulseWidth += InputData[PointIndex].X - HighLevelStart;
					break;
				}
			}
		}

		PulseWidth *= TimeResolution;
	}

	void Measurement::AddData(const std::string &DataString)
	{
		std::shared_ptr<TextBlock> NewDataTextBlock = std::make_shared<TextBlock>();
		NewDataTextBlock->FontSize = 14;
		NewDataTextBlock->Text = DataString;
		NewDataTextBlock->VerticalAlignment = VerticalAlignment::Bottom;

		//FallingEdgeCountTextBlock.Margin = new Thickness(0, 2, 0, 0);
		getResult()->Children->Add(NewDataTextBlock);
	}
}
