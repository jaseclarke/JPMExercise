// Stock Market Class Unit Test Suite
//
#include "UTStockMarket.hpp"
#include "StockMarket.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <cmath>

const std::vector<std::string> knownStocks{ "TEA", "POP", "ALE", "GIN", "JOE" };
const double Epsilon = 1e-6;

namespace
{
	bool DoubleEqual(double v1, double v2, double epsilon = Epsilon)
	{
		return std::fabs(v1 - v2) < epsilon;
	}
}

namespace 
{
	void RegisterStandardStocks(StockMarket& market)
	{
		market.RegisterStock(Stock("TEA", Stock::StockType::Common, 100, 0));
		market.RegisterStock(Stock("POP", Stock::StockType::Common, 100, 8));
		market.RegisterStock(Stock("ALE", Stock::StockType::Common, 60, 23));
		market.RegisterStock(Stock("GIN", Stock::StockType::Preferred, 100, 8, 0.02));
		market.RegisterStock(Stock("JOE", Stock::StockType::Common, 250, 13));
	}
}

UTStockMarket::UTStockMarket()
{
	RunAll();
}

void UTStockMarket::RunAll()
{
	TestHandleStocks();
	TestVWSPCalculation();
	TestIndexCalculation();
}

void UTStockMarket::TestHandleStocks()
{
	std::cout << "Test: TestHandleStocks" << std::endl;
	auto& market = StockMarket::GetMarket();
	market.ClearStocks();
	market.ClearTrades();

	RegisterStandardStocks(market);

	// 1. Test only registered stocks are found
	assert(market.KnownStock("TEA"));
	assert(!market.KnownStock("XXX"));

	// 2. Retrieve Stocks and check that properties are correct
	// 2a TEA 
	auto teaStock = market.GetStock("TEA");

	assert(teaStock);
	if (teaStock.has_value())
	{
		assert(teaStock->Type() == Stock::StockType::Common);
		assert(DoubleEqual(teaStock->ParValue(), 100));
		assert(DoubleEqual(teaStock->LastDividend(), 0));
	}

	// 2a Common 
	auto ginStock = market.GetStock("GIN");
	assert(ginStock);
	if (ginStock.has_value())
	{
		assert(ginStock->Type() == Stock::StockType::Preferred);
		assert(DoubleEqual(ginStock->ParValue(), 100));
		assert(DoubleEqual(ginStock->FixedDividend(), 0.02));
	}
	std::cout << "Test Complete: TestHandleStocks" << std::endl;
}

void UTStockMarket::TestVWSPCalculation()
{
	std::cout << "Test: TestVWSPCalculation" << std::endl;
	auto& market = StockMarket::GetMarket();
	market.ClearStocks();
	market.ClearTrades();

	RegisterStandardStocks(market);

	// 1. Simple Calculation - all one stock - all in last 15 minutes
	// 10*TEA at 100p, 100*TEA at 200p - weighted average should be (10*100 + 100*200)/110 = 190.909 (3 dp)
	market.ClearTrades();
	market.RecordTrade(Trade("TEA", 1800, Trade::TradeType::Buy, 10, 100));
	market.RecordTrade(Trade("TEA", 1900, Trade::TradeType::Buy, 100, 200));
	assert(DoubleEqual(market.VWSP("TEA", 2000), 190.909, 0.005));

	// 2. Exclude trades more than 15 minutes ago 
	// same result as case 1
	market.ClearTrades();
	market.RecordTrade(Trade("TEA", 900, Trade::TradeType::Buy, 10000, 888));
	market.RecordTrade(Trade("TEA", 1090, Trade::TradeType::Buy, 5000, 777));
	market.RecordTrade(Trade("TEA", 1800, Trade::TradeType::Buy, 10, 100));
	market.RecordTrade(Trade("TEA", 1900, Trade::TradeType::Buy, 100, 200));
	assert(DoubleEqual(market.VWSP("TEA", 2000), 190.909, 0.005));

	// 3. Exclude trades for other stocks
	// same result as case 1
	market.ClearTrades();
	market.RecordTrade(Trade("GIN", 1800, Trade::TradeType::Buy, 10000, 888));
	market.RecordTrade(Trade("GIN", 1900, Trade::TradeType::Buy, 5000, 777));
	market.RecordTrade(Trade("TEA", 1800, Trade::TradeType::Buy, 10, 100));
	market.RecordTrade(Trade("TEA", 1900, Trade::TradeType::Buy, 100, 200));
	assert(DoubleEqual(market.VWSP("TEA", 2000), 190.909, 0.005));

	// 4. etc...

	std::cout << "Test Complete: TestVWSPCalculation" << std::endl;
}


void UTStockMarket::TestIndexCalculation()
{
	std::cout << "Test: TestIndexCalculation" << std::endl;
	auto& market = StockMarket::GetMarket();
	market.ClearStocks();
	market.ClearTrades();

	RegisterStandardStocks(market);

	// 1. Simple Calculation - one trade for each stock - all in last 15 minutes
	// geometric mean of 100,200,300,400,500 = 260.571
	market.ClearTrades();
	market.RecordTrade(Trade("TEA", 1900, Trade::TradeType::Buy, 100, 100));
	market.RecordTrade(Trade("POP", 1900, Trade::TradeType::Buy, 100, 200));
	market.RecordTrade(Trade("ALE", 1900, Trade::TradeType::Buy, 100, 300));
	market.RecordTrade(Trade("GIN", 1900, Trade::TradeType::Buy, 100, 400));
	market.RecordTrade(Trade("JOE", 1900, Trade::TradeType::Buy, 100, 500));
	assert(DoubleEqual(market.GBCEIndex(2000), 260.517, 0.005));

	// 2. Check stocks with no trades are handled - one trade for each stock except TEA in last 15 minutes
	// geometric mean of 200,300,400,500 = 330.975
	market.ClearTrades();
	market.RecordTrade(Trade("TEA", 1000, Trade::TradeType::Buy, 100, 100));  // more than 15 minutes ago
	market.RecordTrade(Trade("POP", 1900, Trade::TradeType::Buy, 100, 200));
	market.RecordTrade(Trade("ALE", 1900, Trade::TradeType::Buy, 100, 300));
	market.RecordTrade(Trade("GIN", 1900, Trade::TradeType::Buy, 100, 400));
	market.RecordTrade(Trade("JOE", 1900, Trade::TradeType::Buy, 100, 500));
	assert(DoubleEqual(market.GBCEIndex(2000), 330.975, 0.005));

	std::cout << "Test Complete: TestIndexCalculation" << std::endl;
}
