#pragma once

#include <string>

class Stock
{
public:
	enum class StockType { Common, Preferred };

	Stock(const std::string& symbol, StockType stockType, double parValue, double lastDividend, double fixedDividend = 0);

	double PERatio(double price) const;

	double DividendYield(double price) const;

	std::string Symbol() const { return symbol_; }
	double ParValue() const { return parValue_; }
	StockType Type() const { return isPreferred_?StockType::Preferred:StockType::Common; }
	double LastDividend() const { return lastDividend_; }
	double FixedDividend() const { return fixedDividend_; }

	Stock() = default;
	Stock(const Stock&) = default;
	Stock& operator=(const Stock&) = default;
private:
	double Dividend(double price) const;

	std::string symbol_;
	double parValue_;

	bool isPreferred_;
	double fixedDividend_;

	double lastDividend_;
};
