#pragma once

#include "InterfaceDataItem.h"
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
	class MatchingItem : public InterfaceItem
	{
	private:
		int privateItemIndex = 0;

	public:
		const int &getItemIndex() const;
		void setItemIndex(const int &value);

protected:
		std::shared_ptr<MatchingItem> shared_from_this()
		{
			return std::static_pointer_cast<MatchingItem>(InterfaceItem::shared_from_this());
		}
	};
}
