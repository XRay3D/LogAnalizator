#pragma once

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
	/// <summary>
	/// Логика взаимодействия для Window1.xaml
	/// </summary>
	class ColorSelect : public Window
	{
	private:
		std::shared_ptr<Brush> privateNewColor;

	public:
		const std::shared_ptr<Brush> &getNewColor() const;
		void setNewColor(const std::shared_ptr<Brush> &value);


		ColorSelect(const std::shared_ptr<Brush> &Color);

	private:
		void ColorBorder_MouseDown(const std::shared_ptr<void> &sender, const std::shared_ptr<MouseButtonEventArgs> &e);

		void ChooseButton_Click(const std::shared_ptr<void> &sender, const std::shared_ptr<RoutedEventArgs> &e);

		void CancelButton_Click(const std::shared_ptr<void> &sender, const std::shared_ptr<RoutedEventArgs> &e);

protected:
		std::shared_ptr<ColorSelect> shared_from_this()
		{
			return std::static_pointer_cast<ColorSelect>(Window::shared_from_this());
		}
	};
}
