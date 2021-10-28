#pragma once

#include <string>
#include <vector>
#include <memory>

//C# TO C++ CONVERTER NOTE: Forward class declarations:
namespace LogicAnalyzer { class InterfaceItem; }
namespace LogicAnalyzer { class InterfaceLine; }

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
	class IHardwareInterface
	{
	public:
		virtual const std::string &getName() const = 0;
		virtual void setName(const std::string &value) = 0;
		virtual const std::string &getType() const = 0;
		virtual const std::vector<int> &getDataLinesNumbers() const = 0;
		virtual const std::vector<int> &getAllLinesNumbers() const = 0;
		virtual std::shared_ptr<ObservableCollection<InterfaceItem>> getInterfaceParts() const = 0;
		virtual std::shared_ptr<ObservableCollection<InterfaceItem>> getInterfaceBits() const = 0;
		virtual const int &getMatchedItemsCount() const = 0;
		virtual const int &getCurrentMatchedItemIndex() const = 0;
		virtual const std::vector<bool> &getInterfaceVisibility() const = 0;
		virtual void setInterfaceVisibility(const std::vector<bool> &value) = 0;

		virtual void Initialize(std::vector<InterfaceLine> &InitParams) = 0;
		virtual void Analyze(std::vector<PointCollection> &SignalPoints, double TimeRes) = 0;
		virtual void SearchItem(const std::string &ItemToSearch) = 0;
		virtual void NextMatchItem() = 0;
		virtual void PreviousMatchItem() = 0;
	};
}
