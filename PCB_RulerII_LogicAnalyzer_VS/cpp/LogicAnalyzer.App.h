#pragma once

#include <memory>


namespace LogicAnalyzer
{
	/// <summary>
	/// Логика взаимодействия для App.xaml
	/// </summary>
	class App : public Application
	{

protected:
		std::shared_ptr<App> shared_from_this()
		{
			return std::static_pointer_cast<App>(Application::shared_from_this());
		}
	};
}
