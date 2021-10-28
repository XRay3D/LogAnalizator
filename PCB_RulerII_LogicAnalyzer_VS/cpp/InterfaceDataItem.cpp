#include "InterfaceDataItem.h"


namespace LogicAnalyzer
{

	const std::string &InterfaceItem::getItem() const
	{
		return privateItem;
	}

	void InterfaceItem::setItem(const std::string &value)
	{
		privateItem = value;
	}

	const int &InterfaceItem::getStartPoint() const
	{
		return privateStartPoint;
	}

	void InterfaceItem::setStartPoint(const int &value)
	{
		privateStartPoint = value;
	}

	const std::string &InterfaceItem::getBackground() const
	{
		return privateBackground;
	}

	void InterfaceItem::setBackground(const std::string &value)
	{
		privateBackground = value;
	}

	const int &InterfaceItem::getWidth() const
	{
		return privateWidth;
	}

	void InterfaceItem::setWidth(const int &value)
	{
		privateWidth = value;
	}

	const double &InterfaceItem::getCaptureTime() const
	{
		return privateCaptureTime;
	}

	void InterfaceItem::setCaptureTime(const double &value)
	{
		privateCaptureTime = value;
	}

	InterfaceItem::InterfaceItem(const std::string &TextItem, int PointIndex)
	{
		setItem(TextItem);
		setStartPoint(PointIndex);
	}

	InterfaceItem::InterfaceItem()
	{

	}
}
