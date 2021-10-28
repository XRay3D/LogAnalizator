#include "LogicAnalyzer.Properties.Settings.h"

namespace LogicAnalyzer
{
	namespace Properties
	{

std::shared_ptr<Settings> Settings::defaultInstance = (std::static_pointer_cast<Settings>(System::Configuration::ApplicationSettingsBase::Synchronized(std::make_shared<Settings>())));

		const std::shared_ptr<Settings> &Settings::getDefault()
		{
			return defaultInstance;
		}
	}
}
