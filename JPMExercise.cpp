#include <string>
#include <cstdlib>
#include <cctype>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <numeric>

#include "Stock.hpp"
#include "Trade.hpp"
#include "StockMarket.hpp"
#include "UTStockMarket.hpp"

namespace  // Main application specific functions
{
	const std::vector<std::string> knownStocks{ "TEA", "POP", "ALE", "GIN", "JOE" };

	// Utility function to generate random trades for the given stock
	void GenerateRandomTrades(StockMarket& market, const std::string& symbol, int number, int startTime, int endTime, double basePrice)
	{
		for (int i = 0; i < number; ++i)
		{
			int quantity = (rand() % 50 + 1) * 25;
			auto buyOrSell = (rand() % 2 == 0) ? Trade::TradeType::Buy : Trade::TradeType::Sell;

			basePrice += basePrice*(rand() % 100 - 50) / (100 * 10);
			time_t tradeTime = (rand() % (endTime - startTime)) + startTime;
			market.RecordTrade(Trade(symbol, tradeTime, buyOrSell, quantity, basePrice));
		}
	}

	std::string toUpper(std::string s) {
		std::transform(s.begin(), s.end(), s.begin(),
			[](unsigned char c) { return std::toupper(c); } 
		);
		return s;
	}

	std::string csvString(const std::vector<std::string>& vec)
	{
		std::string result = std::accumulate(std::next(vec.begin()), vec.end(),
			vec[0],
			[](const std::string& a, const std::string& b) {
			return a + "," + b;
		});
		return result;
	}

	// Populates the market with the specified stocks and generates some random trades
	// (in the period to 2000 seconds)
	void PopulateMarket(StockMarket& market)
	{
		market.RegisterStock(Stock("TEA", Stock::StockType::Common, 100, 0));
		market.RegisterStock(Stock("POP", Stock::StockType::Common, 100, 8));
		market.RegisterStock(Stock("ALE", Stock::StockType::Common, 60, 23));
		market.RegisterStock(Stock("GIN", Stock::StockType::Preferred, 100, 8, 0.02));
		market.RegisterStock(Stock("JOE", Stock::StockType::Common, 250, 13));

		for (auto stockName : knownStocks)
		{
			auto stock = market.GetStock(stockName);
			if (stock)
			{
				GenerateRandomTrades(market, stockName, (rand() % 250), 100, 2000, stock->ParValue());
			}
		}
	}

	// The main application loop - just takes input via the standard console
	void RunApplication(StockMarket& market)
	{
		while (true)
		{
			std::cout << "Enter a stock symbol, Index for the index, UT to run unit tests (and then quit) or quit to end " << std::endl;
			std::cout << "Know Stock Symbols are: " << csvString(knownStocks) << std::endl;

			std::string userInput;
			std::cout << "Input : ";
			std::cin >> userInput;

			userInput = toUpper(userInput);

			if (userInput == "QUIT") break;

			if (userInput == "INDEX")
			{
				std::cout << "GBCE Index  = " << std::setiosflags(std::ios::fixed) << std::setprecision(1) << market.GBCEIndex(2000) << std::endl;
			}
			else if (userInput == "UT")
			{
				UTStockMarket unitTestRunner;
				break;
			}
			else
			{
				if (market.KnownStock(userInput))
				{
					double price;
					std::cout << "Enter a price for " << userInput << ": ";
					std::cin >> price;

					if (std::cin.fail() || price <= 0)
					{
						std::cin.clear();  // in case of failure the input stream must be reset
						std::cin.ignore(INT_MAX, '\n');

						std::cout << "Price must be a positive numeric value." << std::endl;
						continue;
					}

					std::cout << "For stock " << userInput << std::endl;
					std::cout << std::setiosflags(std::ios::fixed) << std::setprecision(1);
					std::cout << "PE Ratio        = " << market.GetStock(userInput)->PERatio(price) << std::endl;
					std::cout << std::setiosflags(std::ios::fixed) << std::setprecision(3);
					std::cout << "Dividend Yield  = " << market.GetStock(userInput)->DividendYield(price) << std::endl;
					std::cout << std::setiosflags(std::ios::fixed) << std::setprecision(1);
					std::cout << "VWSP            = " << market.VWSP(userInput,2000) << std::endl;
				}
				else
				{
					std::cout << "Unknown stock symbol " << userInput << std::endl;
				}
			}
		}
	}
}

void main()
{
	/*

	Notes:

	1. This is all set up to run off using the current time - however currently it's set to work of fixed 
	   time periods (number of seconds since the epoch) - to simplify data generation/handling
	2. Unit tests would work better with some sort of test framework... just one class (StockMarket) contains some
	   representative unit tests... UTs are driven from the application harness
	
	Some questions:

	1. The precise meaning of the Dividend value is not clear - for now just calculating in using the same method as 
	   the dividend yield calculation.
	2. It's not clear how to handle the calculation of the PE Ratio when the last dividend is zero (as per the POP example)?  The 
	   code just returns 'inf' at the moment.
	3. The calculation of the GBCE assumes that the price to be used in the calculation is the VWSP of each know stock (with trades in the last 15 minutes) 
	   is this what's wanted?
	4. The calculation of the geometric mean just ignores negative or zero entries (i.e. skips them) - this may not be what is required.
	
	*/

	std::srand(static_cast<unsigned int>(std::time(nullptr)));

	StockMarket& market = StockMarket::GetMarket();

	PopulateMarket(market);
	RunApplication(market);
}

