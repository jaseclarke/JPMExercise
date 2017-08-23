#pragma once

#include <ctime>
#include <string>

class Trade
{
public:
	enum class TradeType { Buy, Sell };

	Trade(const std::string& symbol, time_t tradeTime, TradeType buyOrSell, int quantity, double price)
		: symbol_(symbol), tradeTime_(tradeTime), tradeType_(buyOrSell), quantity_(quantity), price_(price)
	{}

	std::string Symbol() const { return symbol_; }

	time_t Time() const { return tradeTime_; }
	TradeType BuyOrSellIndicator() const { return tradeType_; }

	int Quantity() const { return quantity_; }
	double Price() const { return price_; }

	// Default Copy/Assignment is ok for trades
	Trade(const Trade&) = default;
	Trade& operator=(const Trade&) = default;
private:
	std::string symbol_;
	std::time_t tradeTime_;
	TradeType tradeType_;
	int quantity_;
	double price_;
};