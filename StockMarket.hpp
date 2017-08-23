#pragma once

#include "Trade.hpp"
#include "Stock.hpp"

#include <ctime>
#include <string>
#include <vector>
#include <optional>
#include <map>

class StockMarket
{
public:
	// Just provide access to a single market for the purposes of the application.
	static StockMarket& GetMarket() { static StockMarket market; return market; }

	// GBCE Index 
	// Assuming that the price used in the calculation is the VWSP of each stock in the market
	double GBCEIndex(time_t endTime = std::time(nullptr)) const;
	double VWSP(const std::string& symbol, time_t endTime = std::time(nullptr)) const;

	// Record a trade
	void RecordTrade(const Trade& trade);

	// Stock Handling
	void RegisterStock(const Stock& stock);
	bool KnownStock(const std::string& symbol) const;
	std::optional<Stock> GetStock(const std::string& symbol) const;

	// Reset functions
	void ClearTrades(); // Clear all trades
	void ClearStocks(); // Clear all stocks

	// Don't allow copy/assignment of the stock market
	StockMarket(const StockMarket&) = delete;
	StockMarket& operator=(const StockMarket&) = delete;
private:
	std::vector<Trade> GetStockTradesSince(const std::string& symbol, time_t earliestTime) const;
	time_t LastTradeTime() const;

	static double GeometricMean(const std::vector<double>& values);

	StockMarket() = default;

	std::map<std::string, Stock> stocks_;
	std::vector<Trade> trades_;
};
