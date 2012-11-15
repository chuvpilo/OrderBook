//==========================================================================
// Name        : OrderBook.cpp
// Author      : Gleb Chuvpilo
// Version     : 1.0
// Copyright   : (c) Gleb Chuvpilo, 2012
// Description : Order Book
//==========================================================================

#include "OrderBook.h"

// Define statics

trading::OrderBook::BidsMap trading::OrderBook::bidsMap_ = trading::OrderBook::BidsMap();
trading::OrderBook::AsksMap trading::OrderBook::asksMap_ = trading::OrderBook::AsksMap();

trading::OrderBook::BidsHash trading::OrderBook::bidsHash_ = trading::OrderBook::BidsHash();
trading::OrderBook::AsksHash trading::OrderBook::asksHash_ = trading::OrderBook::AsksHash();

trading::MarketOrder::Size trading::OrderBook::openBids_ = 0;
trading::MarketOrder::Size trading::OrderBook::openAsks_ = 0;


double trading::OrderBook::pretendExecuteMarketOrder(const trading::OrderSide& side, const trading::MarketOrder::Size& targetSize) {

	if (targetSize < 1) { // sanity check
		throw trading::BadOrderSize();
	}

	assert(targetSize >= 1); // in case someone removes the throw above

	trading::MarketOrder::Size sizeCompleted = 0; // how much of the order size we have completed
	double amount = 0;       					  // how much we've spent/received from execution

	trading::MarketOrder order;

	switch (side) {
	case trading::buy:
		if (targetSize > openAsks_) { // not possible to execute
			return 0;
		} else { // enough open interest to execute
			for (MapIter it = asksMap_.begin(); it != asksMap_.end(); it++) {
				order = it->second;
				FILE_LOG(logDEBUG) << "Using order " << order.toString() << " to execute";
				double price = static_cast<double>(order.price)/100;
				trading::MarketOrder::Size sizeFromCurrentOrder = std::min(order.size,targetSize - sizeCompleted);

				sizeCompleted += sizeFromCurrentOrder;
				amount += sizeFromCurrentOrder * price;

				if (sizeCompleted == targetSize) {
					return amount;
				}
			}
		}
		break;
	case trading::sell:
		if (targetSize > openBids_) { // not possible to execute
			return 0;
		} else { // enough open interest to execute
			for (MapIter it = bidsMap_.begin(); it != bidsMap_.end(); it++) {
				order = it->second;
				FILE_LOG(logDEBUG) << "Using order " << order.toString() << " to execute";
				double price = static_cast<double>(order.price)/100;
				trading::MarketOrder::Size sizeFromCurrentOrder = std::min(order.size,targetSize - sizeCompleted);

				sizeCompleted += sizeFromCurrentOrder;
				amount += sizeFromCurrentOrder * price;

				if (sizeCompleted == targetSize) {
					return amount;
				}
			}
		}
		break;
	default:
		throw trading::BadOrderSide();
	}

	// should never get here
	assert(false);
	return 0;
}

std::string trading::OrderBook::printBook() {
	std::stringstream oss;
	trading::MarketOrder order;

	oss << std::endl ;
	oss << "Order book:" << std::endl;
	oss << "Bids\t\tAsks" << std::endl;


	for (MapReverseIter it = asksMap_.rbegin(); it != asksMap_.rend(); it++) {
		order = it->second;
		oss << "\t\t" << static_cast<double>(order.price) / 100 << " x " << order.size << "\t// " << order.toString() << std::endl;
	}

	for (MapIter it = bidsMap_.begin(); it != bidsMap_.end(); it++) {
		order = it->second;
		oss << "" << static_cast<double>(order.price) / 100 << " x " << order.size << "\t\t\t// " << order.toString() << std::endl;
	}

	oss << std::endl;
	oss << "Open bids = " << openBids_ << std::endl;
	oss << "Open asks = " << openAsks_ << std::endl;

	return oss.str();
}

