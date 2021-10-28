#pragma once

#include <string>
#include <memory>

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
	class AnalyzerChannel : public std::enable_shared_from_this<AnalyzerChannel>
	{
	private:
		int privateIndex = 0;
		int privateStart_Y = 0;
		int privatePulseHeight = 0;
		std::string privateTrigger;
		std::string privateName;

		std::shared_ptr<PointCollection> CapturedData = std::make_shared<PointCollection>();
		double XOffset = 0;
		std::shared_ptr<PointCollection> graphpoints = std::make_shared<PointCollection>();


	public:
		const std::shared_ptr<PointCollection> &getGraphPoints() const;

		const int &getIndex() const;
		void setIndex(const int &value);
		const int &getStart_Y() const;
		void setStart_Y(const int &value);
		const int &getPulseHeight() const;
		void setPulseHeight(const int &value);
		const std::string &getTrigger() const;
		void setTrigger(const std::string &value);
		const std::string &getName() const;
		void setName(const std::string &value);

		void RedrawGraph(int StartPoint, int PointsCount);

		void CaptureData(const std::shared_ptr<PointCollection> &InputData);

		std::shared_ptr<PointCollection> GetCapturedData();

		double Get_X_Offset();

		void Scale(double NewScaleFactor);
	};
}
