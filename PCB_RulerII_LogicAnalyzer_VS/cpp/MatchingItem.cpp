#include "MatchingItem.h"


namespace LogicAnalyzer
{

	const int &MatchingItem::getItemIndex() const
	{
		return privateItemIndex;
	}

	void MatchingItem::setItemIndex(const int &value)
	{
		privateItemIndex = value;
	}
}
