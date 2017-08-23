#include "Stock.hpp"

Stock::Stock(const std::string& symbol, StockType stockType, double parValue, double lastDividend, double fixedDividend /*= 0*/) 
	: symbol_(symbol), parValue_(parValue), lastDividend_(lastDividend), 
	isPreferred_(stockType == StockType::Preferred), fixedDividend_(fixedDividend)
{}

double Stock::PERatio(double price) const
{
	return price / Dividend(price);
}

double Stock::DividendYield(double price) const
{
	return isPreferred_ ? (fixedDividend_*parValue_ / price) : (lastDividend_ / price);
}

double Stock::Dividend(double price) const
{
	return isPreferred_ ? fixedDividend_*parValue_ : lastDividend_;
}

