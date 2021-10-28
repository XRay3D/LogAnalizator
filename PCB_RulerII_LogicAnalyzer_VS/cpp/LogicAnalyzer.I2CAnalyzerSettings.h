#pragma once

#include <string>
#include <vector>
#include <memory>

//C# TO C++ CONVERTER NOTE: Forward class declarations:
namespace LogicAnalyzer { class InterfaceLine; }


namespace LogicAnalyzer
{
	/// <summary>
	/// Interaction logic for I2CAnalyzerSettings.xaml
	/// </summary>
	class I2CAnalyzerSettings : public Window
	{
	private:
		std::vector<InterfaceLine> I2CLines;
		std::vector<bool> I2CVisibility;
		std::string name;

	public:
		std::vector<InterfaceLine> getInterfaceLines() const;
		const std::string &getName() const;
		const std::vector<bool> &getI2CInterVisibility() const;

		I2CAnalyzerSettings();

	private:
		void CancelButton_Click(const std::shared_ptr<void> &sender, const std::shared_ptr<RoutedEventArgs> &e);

		void AcceptButton_Click(const std::shared_ptr<void> &sender, const std::shared_ptr<RoutedEventArgs> &e);

protected:
		std::shared_ptr<I2CAnalyzerSettings> shared_from_this()
		{
			return std::static_pointer_cast<I2CAnalyzerSettings>(Window::shared_from_this());
		}
	};
}
