#include "LogicAnalyzer.I2CAnalyzerSettings.h"
#include "InterfaceLine.h"


namespace LogicAnalyzer
{

	std::vector<InterfaceLine> I2CAnalyzerSettings::getInterfaceLines() const
	{
		return I2CLines;
	}

	const std::string &I2CAnalyzerSettings::getName() const
	{
		return name;
	}

	const std::vector<bool> &I2CAnalyzerSettings::getI2CInterVisibility() const
	{
		return I2CVisibility;
	}

	I2CAnalyzerSettings::I2CAnalyzerSettings()
	{
		InitializeComponent();

		std::vector<std::string> Channels;
		for (int i = 0; i < 8; i++)
		{
			Channels.push_back(std::string("канал ") + std::to_string(i));
		}
		SCLcomboBox->ItemsSource = Channels;
		SDAcomboBox->ItemsSource = Channels;
	}

	void I2CAnalyzerSettings::CancelButton_Click(const std::shared_ptr<void> &sender, const std::shared_ptr<RoutedEventArgs> &e)
	{
		setDialogResult(false);
	}

	void I2CAnalyzerSettings::AcceptButton_Click(const std::shared_ptr<void> &sender, const std::shared_ptr<RoutedEventArgs> &e)
	{
		I2CLines.push_back(std::make_shared<InterfaceLine>(SCLcomboBox::SelectedIndex));
		I2CLines[0]->setType(InterfaceLine::CLKLineType);
		I2CLines.push_back(std::make_shared<InterfaceLine>(SDAcomboBox::SelectedIndex));
		I2CLines[1]->setType(InterfaceLine::DataLineType);

		name = NameTextBox->Text;

		setDialogResult(true);
	}
}
