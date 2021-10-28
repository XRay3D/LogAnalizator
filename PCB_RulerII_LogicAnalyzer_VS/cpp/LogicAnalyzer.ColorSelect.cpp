#include "LogicAnalyzer.ColorSelect.h"


namespace LogicAnalyzer
{

	const std::shared_ptr<Brush> &ColorSelect::getNewColor() const
	{
		return privateNewColor;
	}

	void ColorSelect::setNewColor(const std::shared_ptr<Brush> &value)
	{
		privateNewColor = value;
	}

	ColorSelect::ColorSelect(const std::shared_ptr<Brush> &Color)
	{
		InitializeComponent();

		std::shared_ptr<Collection<PropertyInfo>> ColorCollection = std::make_shared<Collection<PropertyInfo>>(Colors::typeid->GetProperties());
		std::shared_ptr<Collection<Border>> BorderCollection = std::make_shared<Collection<Border>>();

		for (int i = 0; i < ColorCollection->Count; i++)
		{
			std::shared_ptr<Border> NewBorder = std::make_shared<Border>();
			NewBorder->Width = 20;
			NewBorder->Height = 20;
			NewBorder->BorderThickness = Thickness(2);
			NewBorder->BorderBrush = Brushes::Black;
			NewBorder->Background = std::make_shared<SolidColorBrush>(static_cast<Color>(ColorConverter::ConvertFromString(ColorCollection[i]->Name)));
			NewBorder->MouseDown += ColorBorder_MouseDown;
			BorderCollection->Add(NewBorder);
		}

		ColorsItems->ItemsSource = BorderCollection;
	}

	void ColorSelect::ColorBorder_MouseDown(const std::shared_ptr<void> &sender, const std::shared_ptr<MouseButtonEventArgs> &e)
	{
		for (int i = 0; i < ColorsItems::Items->Count; i++)
		{
			(std::dynamic_pointer_cast<Border>(ColorsItems::Items[i]))->BorderBrush = Brushes::Black;
		}

		std::shared_ptr<Border> SelectedBorder = (std::dynamic_pointer_cast<Border>(sender));
		SelectedBorder->BorderBrush = Brushes::Cyan;
		setNewColor(SelectedBorder->Background);
	}

	void ColorSelect::ChooseButton_Click(const std::shared_ptr<void> &sender, const std::shared_ptr<RoutedEventArgs> &e)
	{
		setDialogResult(true);
	}

	void ColorSelect::CancelButton_Click(const std::shared_ptr<void> &sender, const std::shared_ptr<RoutedEventArgs> &e)
	{
		setDialogResult(false);
	}
}
