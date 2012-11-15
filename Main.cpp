//============================================================================
// Name        : Main.cpp
// Author      : Gleb Chuvpilo
// Version     : 1.0
// Copyright   : (c) Gleb Chuvpilo, 2012
// Description : Main
//============================================================================

#include <iostream>
#include <iomanip>  // setprecision
#include <cassert>
#include <string>   // getline
#include <typeinfo>

#include "OrderBook.h"
#include "Log.h"
#include "MarketDataProvider.h"
#include "MarketOrder.h"
#include "Parser.h"
#include "Exceptions.h"
#include "Utils.h"


int main(int argc, char* argv[]) {
	try {
		std::cout << std::setiosflags(std::ios::fixed); // to show amounts as XXXX.XX

		FILE_LOG(logDEBUG) << "Starting Trading Simulator";


		// Process arguments:
		// Arguments should be either "./Pricer 200" or "./Pricer 200 feed.txt"

		unsigned long int targetSize;
		bool useFileForMarketFeed;

		switch (argc) {
		case 2:
			targetSize = std::atol(argv[1]);
			if (targetSize < 1) {
				FILE_LOG(logERROR) << "Expected a positive number greater than or equal to 1";
				abort();
			}
			useFileForMarketFeed = false;
			break;
		case 3:
			targetSize = std::atol(argv[1]);
			if (targetSize < 1) {
				FILE_LOG(logERROR) << "Expected a positive number greater than or equal to 1";
				abort();
			}
			useFileForMarketFeed = true;
			try {
				trading::MarketDataProvider::getInstance().readMarketDataFile(argv[2]);
			} catch (const trading::BadMarketDataFile& e) {
				FILE_LOG(logERROR) << "Error opening the market data file";
				abort();
			}
			break;
		default:
			FILE_LOG(logERROR) << "Error with program arguments. There are two ways to start this program:";
			FILE_LOG(logERROR) << "./Pricer 200             // 200 is the target size of market order";
			FILE_LOG(logERROR) << "./Pricer 200 feed.txt    // use feed.txt instead of standard input";
			abort();
		}

		assert(targetSize >= 1);
		FILE_LOG(logDEBUG) << "target-size = " << targetSize;

		// For the the market order
		double newAmount = 0;
		double cachedBuyAmount = 0;
		double cachedSellAmount = 0;

		// Main loop
		while (true) {

			// Infinite loop if using standard input; break loop on EOF if using a market data file
			if (useFileForMarketFeed && !trading::MarketDataProvider::getInstance().hasNextMessage()) {
				break;
			}

			// "Receive" new message
			std::string msg;
			if (useFileForMarketFeed) {
				msg = trading::MarketDataProvider::getInstance().nextMessage();
				std::cout << msg << std::endl;
			} else {
				std::getline(std::cin, msg);
				// std::cout << msg << std::endl;
			}


			// Parse message
			trading::MarketOrder order;
			unsigned long int prevTimestamp = 0;
			try {
				order = trading::Parser::parse(msg);

				if (order.timestamp < prevTimestamp) { // out of order messages
					throw trading::OutOfOrder(); // technically, this throw should be elsewhere in a real system
				}

			} catch (const trading::ParseException&) {
				FILE_LOG(logERROR) << "Skipping this message due to parsing errors: " << msg;
				continue;
			}


			// Submit the message to the order book
			try {
				trading::OrderBook::getInstance().processOrder(order);
			} catch (const trading::OrderBookException&) {
				FILE_LOG(logERROR) << "Error in order book when submitting the following order: " << order.toString();
				continue;
			}

			std::string result;
			try {
				result = trading::OrderBook::printBook();
				FILE_LOG(logDEBUG) << "Result: " << result;
			} catch (const trading::OrderBookException&) {
				FILE_LOG(logERROR) << "Error while printing book";
				continue;
			}


			// Pretend to execute a market order
			try {

				// buy at market
				newAmount = trading::OrderBook::getInstance().pretendExecuteMarketOrder(trading::buy,targetSize);
				if (newAmount != cachedBuyAmount) { // only display if newAmount changes
					cachedBuyAmount = newAmount;
					if (newAmount > 0) {
						std::cout << order.timestamp << " B " << std::setprecision(2) << newAmount << std::endl;
					} else { // newAmount = 0;
						std::cout << order.timestamp << " B NA" << std::endl;
					}
				}


				// sell at market
				newAmount = trading::OrderBook::getInstance().pretendExecuteMarketOrder(trading::sell,targetSize);
				if (newAmount != cachedSellAmount) { // only display if newAmount changes
					cachedSellAmount = newAmount;
					if (newAmount > 0) {
						std::cout << order.timestamp << " S " << std::setprecision(2) << newAmount << std::endl;
					} else { // newAmount = 0;
						std::cout << order.timestamp << " S NA" << std::endl;
					}
				}

			} catch (const trading::OrderBookException&) {
				FILE_LOG(logERROR) << "Error while pretending to execute a market order " << order.toString();
				continue;
			}

		}

		// Done
		FILE_LOG(logDEBUG) << "Simulator is stopped.";

	} catch (const trading::Exception& e) { // exception catch-all
		FILE_LOG(logERROR) << "Exception: " << typeid(e).name();
	}
	return 0;
}

