//============================================================================
// Name        : MarketOrder.h
// Author      : Gleb Chuvpilo
// Version     : 1.0
// Copyright   : (c) Gleb Chuvpilo, 2012
// Description : Market Order
//============================================================================

#ifndef MARKETORDER_H_
#define MARKETORDER_H_

#include <iostream>
#include <string>
#include <sstream>

namespace trading {

// A couple of useful enums
enum OrderType {add, reduce};
enum OrderSide {buy, sell};

// Market Order
struct MarketOrder {
	std::string toString() const;

	// We'll use these typedefs in the OrderBook class
	typedef unsigned long int Price; // limit price (in cents)
	typedef unsigned long int Size;  // Order size
	typedef std::string Id;          // unique order ID


	OrderType type;
	unsigned long int timestamp;  // milliseconds since midnight
	Id id;                        // unique ID
	OrderSide side;               // order side
	Price price;                  // limit price (in cents, to avoid rounding issues)
	Size size;       			  // order size (# shares)

};

} // end of namespace


// Definitions of inline functions

inline std::string trading::MarketOrder::toString() const {
	std::stringstream oss;

	if (type == add) {
		oss << "AddOrder: " << timestamp << " " << id << " " << (side == buy ? "B" : "S") <<
				" " << static_cast<double>(price) / 100 << " " << size;
	} else {
		oss << "ReduceOrder: " << timestamp << " " << id << " " << size;
	}
	return oss.str();
}

#endif /* MARKETORDER_H_ */
