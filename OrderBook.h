//============================================================================
// Name        : OrderBook.h
// Author      : Gleb Chuvpilo
// Version     : 1.0
// Copyright   : (c) Gleb Chuvpilo, 2012
// Description : Order Book Implementation
//============================================================================

#ifndef ORDERBOOK_H_
#define ORDERBOOK_H_

#include <map>
#include <list>
#include <tr1/unordered_map>
#include <string>
#include <sstream>
#include <algorithm>
#include <cassert>

#include "Exceptions.h"
#include "Log.h"
#include "MarketOrder.h"

namespace trading {

/**
 * Order Book (for one equity) as Meyers' Singleton
 */
class OrderBook {
public:
	// Singleton
	static OrderBook& getInstance();

	// Pretend executing a market order
	static double pretendExecuteMarketOrder(const trading::OrderSide& side, const trading::MarketOrder::Size& targetSize);

	// Print order book
	static std::string printBook();

	// Process a new market order
	static void processOrder(const trading::MarketOrder& order);

private:
	// Iterators
	typedef std::multimap<trading::MarketOrder::Price,trading::MarketOrder>::iterator MapIter;
	typedef std::multimap<trading::MarketOrder::Price,trading::MarketOrder>::reverse_iterator MapReverseIter;
	typedef std::tr1::unordered_map<trading::MarketOrder::Id,MapIter>::iterator HashmapIter;

	// Multimaps: price -> order
	typedef std::multimap<trading::MarketOrder::Price,trading::MarketOrder,std::greater<trading::MarketOrder::Price> > BidsMap;
	typedef std::multimap<trading::MarketOrder::Price,trading::MarketOrder,std::less<trading::MarketOrder::Price> > AsksMap;

	// Hashmaps: order id -> multimap iterator (for quick lookup by order ID)
	typedef std::tr1::unordered_map<trading::MarketOrder::Id,MapIter> BidsHash;
	typedef std::tr1::unordered_map<trading::MarketOrder::Id,MapIter> AsksHash;

	static BidsMap bidsMap_; // only declare (define in .cpp)
	static AsksMap asksMap_;

	static BidsHash bidsHash_; // only declare (define in .cpp)
	static AsksHash asksHash_;

	static trading::MarketOrder::Size openBids_; // open interest (bids)
	static trading::MarketOrder::Size openAsks_; // open interest (asks)

// Singleton stuff
private:
	OrderBook() { };
	OrderBook(OrderBook const&);      // Don't Implement
	void operator=(OrderBook const&); // Don't implement

};

} // end of namespace


// Definitions of inline functions

inline trading::OrderBook& trading::OrderBook::getInstance() {
	static OrderBook _instance; // Guaranteed to be destroyed. Instantiated on first use.
	return _instance;
}

inline void trading::OrderBook::processOrder(const trading::MarketOrder& order) {

	// Pair for the tree
	std::pair<trading::MarketOrder::Price,trading::MarketOrder> priceOrderPair =
			std::pair<trading::MarketOrder::Price,trading::MarketOrder>(order.price,order);

	// Pair for the hashmap
	std::pair<trading::MarketOrder::Id,MapIter> idIterPair;

	// both for the tree and the hashmap
	MapIter mapIter; // for the return value when inserting into multimap
	HashmapIter hmIter;
	std::pair<HashmapIter,bool> hashRet; // for the return value when inserting into hashmap

	switch (order.type) {
	case add: // new order

		switch (order.side) {
		case buy:
			mapIter = bidsMap_.insert(priceOrderPair); // Update the order multimap (price->order); order O(log n)
			idIterPair = std::pair<trading::MarketOrder::Id,MapIter>(order.id,mapIter); // We store this in hashmap
			hashRet = bidsHash_.insert(idIterPair); // Update the hashmap; order O(1)
			if (!hashRet.second) {
				throw DuplicateOrderId();
			}
			openBids_ += order.size;
			FILE_LOG(logDEBUG) << "Adding 'Buy' order: " << order.toString();
			break;
		case sell:
			mapIter = asksMap_.insert(priceOrderPair);
			idIterPair = std::pair<trading::MarketOrder::Id,MapIter>(order.id,mapIter);
			hashRet = asksHash_.insert(idIterPair);
			if (!hashRet.second) {
				throw DuplicateOrderId();
			}
			openAsks_ += order.size;
			FILE_LOG(logDEBUG) << "Adding 'Sell' order: " << order.toString();
			break;
		default:
			throw BadOrderSide();
		}

		break;

	case reduce: // update existing order
		if ((hmIter = bidsHash_.find(order.id)) != bidsHash_.end()) { // working with bids
			mapIter = hmIter->second;
			trading::MarketOrder& orderFromMap = mapIter->second;
			FILE_LOG(logDEBUG) << "Reducing 'Buy' order " << orderFromMap.toString();
			if (orderFromMap.size - order.size <= 0) { // need to remove order completely
				openBids_ -= orderFromMap.size; // update open interest
				bidsMap_.erase(mapIter);        // delete from map
				bidsHash_.erase(hmIter);        // delete from hashmap
				FILE_LOG(logDEBUG) << "Removed order completely: " << order.toString();
			} else { // need to update order
				orderFromMap.size -= order.size; // update in map
				openBids_ -= order.size;        // update open interest
				FILE_LOG(logDEBUG) << "Adjusted size of order; new order: " << orderFromMap.toString();
			}

		} else if ((hmIter = asksHash_.find(order.id)) != asksHash_.end()) { // working with asks
			mapIter = hmIter->second;
			trading::MarketOrder& orderFromMap = mapIter->second;
			FILE_LOG(logDEBUG) << "Reducing 'Sell' order " << orderFromMap.toString();
			if (orderFromMap.size - order.size <= 0) { // need to remove order completely;
				openAsks_ -= orderFromMap.size; // update open interest
				asksMap_.erase(mapIter);        // delete from map
				asksHash_.erase(hmIter);        // delete from hashmap
				FILE_LOG(logDEBUG) << "Removed order completely: " << order.toString();
			} else { // need to update order
				orderFromMap.size -= order.size; // update in map
				openAsks_ -= order.size;        // update open interest
				FILE_LOG(logDEBUG) << "Adjusted size of order; new order: " << orderFromMap.toString();
			}
		} else {
			throw AttempToReduceNonexistantOrder();
		}
		break;
	default:
		throw BadOrderType(); // should never be here
	}
}

#endif /* ORDERBOOK_H_ */
