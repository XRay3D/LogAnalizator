#pragma once

#include "IHardwareInterface.h"
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
	class I2C : public IHardwareInterface
	{
	private:
		std::string privateName;
		std::vector<bool> privateInterfaceVisibility;

		static const std::string StartItem;
		static const std::string StartRepItem;
		static const std::string AddressItem;
		static const std::string ACKItem;
		static const std::string NACKItem;
		static const std::string ReadItem;
		static const std::string WriteItem;
		static const std::string StopItem;
		static const std::string StartItemBackground;
		static const std::string StartRepItemBackground;
		static const std::string StopItemBackground;
		static const int StartStopItemWidth = 3;
		static const std::string DataBackground;
		static const std::string AddrBackground;
		static const double TimeCoef;
		static const int SCLLineIndex = 0;
		static const int SDALineIndex = 1;


		std::shared_ptr<ObservableCollection<InterfaceItem>> I2C_BitsObserv = std::make_shared<ObservableCollection<InterfaceItem>>();
		std::shared_ptr<ObservableCollection<InterfaceItem>> I2C_PartsObserv = std::make_shared<ObservableCollection<InterfaceItem>>();
		std::shared_ptr<Collection<int>> SearchCollection = std::make_shared<Collection<int>>();
		int MatchingItemsCounter = 0;
		std::vector<InterfaceLine> InterfaceLines;
		std::vector<int> datalinesnumbers;
		std::vector<int> alllinesnumbers;


	public:
		const std::string &getName() const;
		void setName(const std::string &value);
		const std::string &getType() const;
		const std::vector<int> &getDataLinesNumbers() const;
		const std::vector<int> &getAllLinesNumbers() const;
		std::shared_ptr<ObservableCollection<InterfaceItem>> getInterfaceParts() const;
		std::shared_ptr<ObservableCollection<InterfaceItem>> getInterfaceBits() const;
		const int &getMatchedItemsCount() const;
		const int &getCurrentMatchedItemIndex() const;
		const std::vector<bool> &getInterfaceVisibility() const;
		void setInterfaceVisibility(const std::vector<bool> &value);

		void Initialize(std::vector<InterfaceLine> &InitParams);

		void Analyze(std::vector<PointCollection> &SignalPoints, double TimeRes);

	private:
		bool WaitForLevelChange(const std::shared_ptr<PointCollection> &InputSignal, int &StartPoint, int CurrentLevel);

	public:
		void SearchItem(const std::string &ItemToSearch);

		void NextMatchItem();

		void PreviousMatchItem();
	};
}
