#include "InterfaceLine.h"


namespace LogicAnalyzer
{

	InterfaceLine::InterfaceLine(int LineNumber)
	{
		linenum = LineNumber;
	}

	const int &InterfaceLine::getType() const
	{
		return privateType;
	}

	void InterfaceLine::setType(const int &value)
	{
		privateType = value;
	}

	const int &InterfaceLine::getNumber() const
	{
		return linenum;
	}
}
