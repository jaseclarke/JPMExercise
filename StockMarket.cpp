#include "StockMarket.hpp"

double StockMarket::GBCEIndex(time_t endTime /*= std::time(nullptr)*/) const
{
	std::vector<double> prices;
	for (auto stock : stocks_) {
		prices.push_back(VWSP(stock.second.Symbol(), endTime));
	}
	return GeometricMean(prices);
}

double StockMarket::VWSP(const std::string& symbol, time_t endTime /*= std::time(nullptr)*/) const
{
	const time_t FifteenMinutes = 60 * 15;
	time_t earliestTrade = (endTime > FifteenMinutes) ? endTime - FifteenMinutes : 0;
	double weigthedPriceSum = 0;
	int quantitySum = 0;
	for (auto trade : GetStockTradesSince(symbol, earliestTrade))
	{
		weigthedPriceSum += (trade.Price() * trade.Quantity());
		quantitySum += trade.Quantity();
	}
	return quantitySum > 0 ? (weigthedPriceSum / static_cast<double>(quantitySum)) : 0;
}

// Just handling zeros here by ignoring them.. should clarify required behaviour
double StockMarket::GeometricMean(const std::vector<double>& values)
{
	double logSum = 0;
	size_t count = 0;
	for (auto v : values)
	{
		if (v > 0)
		{
			logSum += std::log(v);
			++count;
		}
	}
	return (count > 0) ? std::exp(logSum / count) : 0;
}

void StockMarket::RecordTrade(const Trade& trade)
{
	if (!KnownStock(trade.Symbol()))
	{
		throw std::exception("Unknown Stock");
	}

	if (trade.Time() >= LastTradeTime())
	{
		trades_.push_back(trade);
	}
	else
	{
		for (auto it = trades_.begin(); it != trades_.end(); ++it)
		{
			if (it->Time() >= trade.Time())
			{
				trades_.insert(it, trade);
				break;
			}
		}
	}
}

void StockMarket::RegisterStock(const Stock& stock)
{
	stocks_[stock.Symbol()] = stock;
}

std::vector<Trade> StockMarket::GetStockTradesSince(const std::string& symbol, time_t earliestTime) const
{
	std::vector<Trade> trades;

	// Trades are in ascending time order so start at the end and 
	// work backwards until a trade is before the earliest time (or
    // we get to the end of the list)
	for (auto ti = trades_.rbegin(); ti != trades_.rend(); ++ti)
	{
		if (ti->Time() < earliestTime) break;

		if (ti->Symbol() == symbol)
		{
			trades.push_back(*ti);
		}
	}
	return trades;
}

bool StockMarket::KnownStock(const std::string& symbol) const
{
	return stocks_.count(symbol) > 0;
}

time_t StockMarket::LastTradeTime() const
{
	return trades_.empty() ? 0 : trades_[trades_.size() - 1].Time();
}

std::optional<Stock> StockMarket::GetStock(const std::string & symbol) const
{
	return KnownStock(symbol)? stocks_.at(symbol) : std::optional<Stock>();
}

void StockMarket::ClearTrades()
{
	trades_.clear();
}

void StockMarket::ClearStocks()
{
	stocks_.clear();
}
