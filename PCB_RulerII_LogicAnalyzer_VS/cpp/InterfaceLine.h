#pragma once

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
	class InterfaceLine : public std::enable_shared_from_this<InterfaceLine>
	{
	private:
		int privateType = 0;

	public:
		static const int CLKLineType = 0;
		static const int DataLineType = 1;

	private:
		int linenum = 0;

	public:
		InterfaceLine(int LineNumber);

		const int &getType() const;
		void setType(const int &value);
		const int &getNumber() const;
	};
}
