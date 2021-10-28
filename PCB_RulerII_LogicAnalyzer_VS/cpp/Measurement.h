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
	class Measurement : public std::enable_shared_from_this<Measurement>
	{
	private:
		std::shared_ptr<StackPanel> privateResult;

		static const int LEVEL = -2;
		static const int ONE_PULSE = -1;
		static const int MANY_PULSES = 0;

		std::shared_ptr<PointCollection> InputData;
		double TimeResolution = 0;
		int PulseHighLevel = 0;


	public:
		Measurement(const std::shared_ptr<PointCollection> &GraphSegment, double TimeRes, double ScaleFactor, int HighLevel);

		const std::shared_ptr<StackPanel> &getResult() const;
		void setResult(const std::shared_ptr<StackPanel> &value);

	private:
		double GetDuration(double X1, double X2);

		int PulseCounter(int &Edges, int &Rising, int &Falling);

		void GetPulseWidth(double &PulseWidth);

		void AddData(const std::string &DataString);
	};
}
