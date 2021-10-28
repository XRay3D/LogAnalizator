#include "AnalyzerChannel.h"


namespace LogicAnalyzer
{

	const std::shared_ptr<PointCollection> &AnalyzerChannel::getGraphPoints() const
	{
		return graphpoints;
	}

	const int &AnalyzerChannel::getIndex() const
	{
		return privateIndex;
	}

	void AnalyzerChannel::setIndex(const int &value)
	{
		privateIndex = value;
	}

	const int &AnalyzerChannel::getStart_Y() const
	{
		return privateStart_Y;
	}

	void AnalyzerChannel::setStart_Y(const int &value)
	{
		privateStart_Y = value;
	}

	const int &AnalyzerChannel::getPulseHeight() const
	{
		return privatePulseHeight;
	}

	void AnalyzerChannel::setPulseHeight(const int &value)
	{
		privatePulseHeight = value;
	}

	const std::string &AnalyzerChannel::getTrigger() const
	{
		return privateTrigger;
	}

	void AnalyzerChannel::setTrigger(const std::string &value)
	{
		privateTrigger = value;
	}

	const std::string &AnalyzerChannel::getName() const
	{
		return privateName;
	}

	void AnalyzerChannel::setName(const std::string &value)
	{
		privateName = value;
	}

	void AnalyzerChannel::RedrawGraph(int StartPoint, int PointsCount)
	{
		std::shared_ptr<PointCollection> DataToPlot = std::make_shared<PointCollection>();

		if ((StartPoint < 0) || (StartPoint >= CapturedData->Count))
		{
			return;
		}

		XOffset = CapturedData[StartPoint].X;

		for (int PointIndex = 0; PointIndex < PointsCount; PointIndex++)
		{
			if ((PointIndex + StartPoint) == CapturedData->Count)
			{
				break;
			}
			Point NewPoint = Point(CapturedData[PointIndex + StartPoint].X - XOffset, CapturedData[PointIndex + StartPoint].Y);
			DataToPlot->Add(NewPoint);
		}

		for (int PointIndex = 0; PointIndex < (DataToPlot->Count - 1); PointIndex++)
		{
			if (DataToPlot[PointIndex].X != DataToPlot[PointIndex + 1].X)
			{
				if (((DataToPlot[PointIndex].Y == getStart_Y()) && (DataToPlot[PointIndex + 1].Y == (getStart_Y() + getPulseHeight()))) || ((DataToPlot[PointIndex].Y == (getStart_Y() + getPulseHeight())) && (DataToPlot[PointIndex + 1].Y == getStart_Y())))
				{
					DataToPlot->Insert(PointIndex + 1, Point(DataToPlot[PointIndex + 1].X, DataToPlot[PointIndex].Y));
				}
			}
		}

		graphpoints = DataToPlot;
	}

	void AnalyzerChannel::CaptureData(const std::shared_ptr<PointCollection> &InputData)
	{
		CapturedData = InputData;
	}

	std::shared_ptr<PointCollection> AnalyzerChannel::GetCapturedData()
	{
		return CapturedData;
	}

	double AnalyzerChannel::Get_X_Offset()
	{
		return XOffset;
	}

	void AnalyzerChannel::Scale(double NewScaleFactor)
	{
		double PrevScaleFactor = 1;

		for (int RawPoint = 0; RawPoint < CapturedData->Count; RawPoint++)
		{
			PrevScaleFactor = (CapturedData[RawPoint + 1].X - CapturedData[RawPoint].X);

			if (PrevScaleFactor != 0)
			{
				break;
			}
		}

		for (int RawPoint = 0; RawPoint < CapturedData->Count; RawPoint++)
		{
			Point NewPoint = CapturedData[RawPoint];
			NewPoint.X /= PrevScaleFactor;
			NewPoint.X *= NewScaleFactor;
			CapturedData[RawPoint] = NewPoint;
		}
	}
}
