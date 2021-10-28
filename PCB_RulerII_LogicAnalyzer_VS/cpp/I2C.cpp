#include "I2C.h"
#include "InterfaceDataItem.h"
#include "InterfaceLine.h"
#include "LogicAnalyzer.MainWindow.h"


namespace LogicAnalyzer
{

const std::string I2C::StartItem = "S";
const std::string I2C::StartRepItem = "Sr";
const std::string I2C::AddressItem = "Адрес ";
const std::string I2C::ACKItem = "ACK";
const std::string I2C::NACKItem = "NACK";
const std::string I2C::ReadItem = "R";
const std::string I2C::WriteItem = "W";
const std::string I2C::StopItem = "P";
const std::string I2C::StartItemBackground = "Green";
const std::string I2C::StartRepItemBackground = StartItemBackground;
const std::string I2C::StopItemBackground = "Red";
const std::string I2C::DataBackground = "DarkGoldenrod";
const std::string I2C::AddrBackground = "Blue";
const double I2C::TimeCoef = 1;

	const std::string &I2C::getName() const
	{
		return privateName;
	}

	void I2C::setName(const std::string &value)
	{
		privateName = value;
	}

	const std::string &I2C::getType() const
	{
		return "I2C";
	}

	const std::vector<int> &I2C::getDataLinesNumbers() const
	{
		return datalinesnumbers;
	}

	const std::vector<int> &I2C::getAllLinesNumbers() const
	{
		return alllinesnumbers;
	}

	std::shared_ptr<ObservableCollection<InterfaceItem>> I2C::getInterfaceParts() const
	{
		return I2C_PartsObserv;
	}

	std::shared_ptr<ObservableCollection<InterfaceItem>> I2C::getInterfaceBits() const
	{
		return I2C_BitsObserv;
	}

	const int &I2C::getMatchedItemsCount() const
	{
		return SearchCollection->Count;
	}

	const int &I2C::getCurrentMatchedItemIndex() const
	{
		if (SearchCollection->Count > 0)
		{
			return SearchCollection[MatchingItemsCounter];
		}
		else
		{
			return 0;
		}
	}

	const std::vector<bool> &I2C::getInterfaceVisibility() const
	{
		return privateInterfaceVisibility;
	}

	void I2C::setInterfaceVisibility(const std::vector<bool> &value)
	{
		privateInterfaceVisibility = value;
	}

	void I2C::Initialize(std::vector<InterfaceLine> &InitParams)
	{
		InterfaceLines = InitParams;
		for (auto line : InitParams)
		{
			alllinesnumbers.push_back(line->getNumber());
			if (line->getType() == InterfaceLine::DataLineType)
			{
				datalinesnumbers.push_back(line->getNumber());
			}
		}
	}

	void I2C::Analyze(std::vector<PointCollection> &SignalPoints, double TimeRes)
	{
		std::shared_ptr<Collection<InterfaceItem>> I2C_Bits = std::make_shared<Collection<InterfaceItem>>();
		std::shared_ptr<Collection<InterfaceItem>> I2C_Parts = std::make_shared<Collection<InterfaceItem>>();

		bool AddrByte = true;
		I2C_BitsObserv->Clear();
		I2C_PartsObserv->Clear();

		for (int point = 0; point < (SignalPoints[SCLLineIndex]->Count - 1); point++)
		{
			if ((SignalPoints[SCLLineIndex][point]->Y == MainWindow::PULSE_HIGH) && (SignalPoints[SCLLineIndex][point + 1]->Y == MainWindow::PULSE_HIGH) && (SignalPoints[SDALineIndex][point]->Y > SignalPoints[SDALineIndex][point + 1]->Y))
			{
				std::shared_ptr<InterfaceItem> tempVar = std::make_shared<InterfaceItem>();
				tempVar->setItem(StartItem);
				tempVar->setStartPoint(point);
				tempVar->setBackground(StartItemBackground);
				tempVar->setWidth(StartStopItemWidth);
				tempVar->setCaptureTime(TimeRes * point * TimeCoef);
				I2C_Parts->Add(tempVar);
				I2C_Bits->Add(I2C_Parts[I2C_Parts->Count - 1]);
				point++;

				if (WaitForLevelChange(SignalPoints[SCLLineIndex], point, MainWindow::PULSE_HIGH))
				{
					break;
				}

				if (WaitForLevelChange(SignalPoints[SCLLineIndex], point, MainWindow::PULSE_LOW))
				{
					break;
				}

				while (point < SignalPoints[SCLLineIndex]->Count)
				{
					std::string I2CItem = "0";
					unsigned char I2CByte = 0;
					int I2CItemStartPoint = 0;


					for (int pulse = 0; pulse < 8; pulse++)
					{
						if (SignalPoints[SDALineIndex][point]->Y == MainWindow::PULSE_LOW)
						{
							I2CItem = "0";
							if ((pulse == 7) && (AddrByte == true))
							{
								I2CItem = WriteItem;
							}
						}

						else
						{
							I2CItem = "1";
							if ((pulse == 7) && (AddrByte == true))
							{
								I2CItem = ReadItem;
							}
							else
							{
								I2CByte |= static_cast<unsigned char>(1 << (7 - pulse));
							}
						}

						I2C_Bits->Add(std::make_shared<InterfaceItem>(I2CItem, point));
						if (pulse == 0)
						{
							I2CItemStartPoint = point;
						}

						if (pulse == 7)
						{
							std::shared_ptr<InterfaceItem> tempVar2 = std::make_shared<InterfaceItem>();
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
							tempVar2->setItem(std::string("0x") + I2CByte.ToString("X"));
							tempVar2->setStartPoint(I2CItemStartPoint);
							tempVar2->setBackground(DataBackground);
							tempVar2->setCaptureTime(TimeRes * I2CItemStartPoint * TimeCoef);
							I2C_Parts->Add(tempVar2);

							if (AddrByte == true)
							{
								I2CByte = static_cast<unsigned char>(I2CByte >> 1);
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
								I2C_Parts[I2C_Parts->Count - 1]->setItem(AddressItem + std::string("0x") + I2CByte.ToString("X") + std::string("+") + I2CItem);
								I2C_Parts[I2C_Parts->Count - 1]->setBackground(AddrBackground);
							}
						}

						if (WaitForLevelChange(SignalPoints[SCLLineIndex], point, MainWindow::PULSE_HIGH))
						{
							break;
						}

						if (WaitForLevelChange(SignalPoints[SCLLineIndex], point, MainWindow::PULSE_LOW))
						{
							break;
						}
					}

					AddrByte = false;

					if (point == SignalPoints[SCLLineIndex]->Count)
					{
						break;
					}

					I2CItem = ACKItem;
					if (SignalPoints[SDALineIndex][point]->Y == MainWindow::PULSE_HIGH)
					{
						I2CItem = NACKItem;
					}
					I2C_Bits->Add(std::make_shared<InterfaceItem>(I2CItem, point));
					I2C_Parts[I2C_Parts->Count - 1]->setItem(I2C_Parts[I2C_Parts->Count - 1]->getItem() + std::string("+") + I2CItem);

					WaitForLevelChange(SignalPoints[SCLLineIndex], point, MainWindow::PULSE_HIGH);

					I2C_Parts[I2C_Parts->Count - 1]->setWidth(point - I2C_Parts[I2C_Parts->Count - 1]->getStartPoint());

					if (WaitForLevelChange(SignalPoints[SCLLineIndex], point, MainWindow::PULSE_LOW))
					{
						break;
					}

					while (SignalPoints[SCLLineIndex][point]->Y == MainWindow::PULSE_HIGH)
					{
						point++;
						if (point >= SignalPoints[SCLLineIndex]->Count - 1)
						{
							break;
						}

						if ((SignalPoints[SCLLineIndex][point]->Y == MainWindow::PULSE_HIGH) && (SignalPoints[SCLLineIndex][point + 1]->Y == MainWindow::PULSE_HIGH) && (SignalPoints[SDALineIndex][point]->Y < SignalPoints[SDALineIndex][point + 1]->Y))
						{
							std::shared_ptr<InterfaceItem> tempVar3 = std::make_shared<InterfaceItem>();
							tempVar3->setItem(StopItem);
							tempVar3->setStartPoint(point);
							tempVar3->setBackground(StopItemBackground);
							tempVar3->setWidth(StartStopItemWidth);
							tempVar3->setCaptureTime(TimeRes * point * TimeCoef);
							I2C_Parts->Add(tempVar3);
							I2C_Bits->Add(I2C_Parts[I2C_Parts->Count - 1]);
							break;
						}

						else if ((SignalPoints[SCLLineIndex][point]->Y == MainWindow::PULSE_HIGH) && (SignalPoints[SCLLineIndex][point + 1]->Y == MainWindow::PULSE_HIGH) && (SignalPoints[SDALineIndex][point]->Y > SignalPoints[SDALineIndex][point + 1]->Y))
						{
							std::shared_ptr<InterfaceItem> tempVar4 = std::make_shared<InterfaceItem>();
							tempVar4->setItem(StartRepItem);
							tempVar4->setStartPoint(point);
							tempVar4->setBackground(StartRepItemBackground);
							tempVar4->setWidth(StartStopItemWidth);
							tempVar4->setCaptureTime(TimeRes * point * TimeCoef);
							I2C_Parts->Add(tempVar4);
							I2C_Bits->Add(I2C_Parts[I2C_Parts->Count - 1]);
							break;
						}
					}

					if (I2C_Bits[I2C_Bits->Count - 1]->getItem() == StopItem)
					{
						AddrByte = true;
						break;
					}

					else if (I2C_Bits[I2C_Bits->Count - 1]->getItem() == StartRepItem)
					{
						AddrByte = true;

						if (WaitForLevelChange(SignalPoints[SCLLineIndex], point, MainWindow::PULSE_HIGH))
						{
							break;
						}

						if (WaitForLevelChange(SignalPoints[SCLLineIndex], point, MainWindow::PULSE_LOW))
						{
							break;
						}
					}

					else
					{
						point--;
						while (SignalPoints[SCLLineIndex][point]->Y == MainWindow::PULSE_HIGH)
						{
							point--;
						}
						point++;
					}
				}
			}
		}

		I2C_BitsObserv = std::make_shared<ObservableCollection<InterfaceItem>>(I2C_Bits);
		I2C_PartsObserv = std::make_shared<ObservableCollection<InterfaceItem>>(I2C_Parts);
	}

	bool I2C::WaitForLevelChange(const std::shared_ptr<PointCollection> &InputSignal, int &StartPoint, int CurrentLevel)
	{
		bool IsSignalEnd = false;

		if (StartPoint >= InputSignal->Count)
		{
			IsSignalEnd = true;
		}
		else
		{
			while (InputSignal[StartPoint].Y == CurrentLevel)
			{
				StartPoint++;
				if (StartPoint == InputSignal->Count)
				{
					IsSignalEnd = true;
					break;
				}
			}
		}

		return IsSignalEnd;
	}

	void I2C::SearchItem(const std::string &ItemToSearch)
	{
		MatchingItemsCounter = 0;
		SearchCollection->Clear();

		for (int i = 0; i < I2C_PartsObserv->Count; i++)
		{
			if (I2C_PartsObserv[i]->getItem().find(ItemToSearch) != std::string::npos)
			{
				SearchCollection->Add(i);
			}
		}
	}

	void I2C::NextMatchItem()
	{
		MatchingItemsCounter++;
		if (MatchingItemsCounter == SearchCollection->Count)
		{
			MatchingItemsCounter = 0;
		}
	}

	void I2C::PreviousMatchItem()
	{
		MatchingItemsCounter--;
		if (MatchingItemsCounter == -1)
		{
			MatchingItemsCounter = SearchCollection->Count - 1;
		}
	}
}
