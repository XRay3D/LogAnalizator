#pragma once

#include <string>

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
	class InterfaceItem : public std::enable_shared_from_this<InterfaceItem>
	{
	private:
		std::string privateItem;
		int privateStartPoint = 0;
		std::string privateBackground;
		int privateWidth = 0;
		double privateCaptureTime = 0;

	public:
		const std::string &getItem() const;
		void setItem(const std::string &value);
		const int &getStartPoint() const;
		void setStartPoint(const int &value);
		const std::string &getBackground() const;
		void setBackground(const std::string &value);
		const int &getWidth() const;
		void setWidth(const int &value);
		const double &getCaptureTime() const;
		void setCaptureTime(const double &value);

		InterfaceItem(const std::string &TextItem, int PointIndex);

		InterfaceItem();
	};
}
