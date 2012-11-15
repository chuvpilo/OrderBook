//============================================================================
// Name        : Parser.h
// Author      : Gleb Chuvpilo
// Version     : 1.0
// Copyright   : (c) Gleb Chuvpilo, 2012
// Description : Parser
//============================================================================

#ifndef PARSER_H_
#define PARSER_H_

#include <sstream>
#include <iostream>
#include <vector>

#include "MarketOrder.h"
#include "Log.h"
#include "Exceptions.h"
#include "Utils.h"

namespace trading {

class Parser {
public:
	// Basic Market Order Parser
	static MarketOrder parse(const std::string& msg);
};

} // end of namespace


// Definitions of inline functions

inline trading::MarketOrder trading::Parser::parse(const std::string& msg) {
	// Get fields
	std::vector<std::string> fields = tokenize(msg, ' ');

	// Basic check
	if (fields.size() != 4) {
		if (fields.size() != 6) {
			throw BadParse();
		}
	}

	MarketOrder order;

	// Parse order type
	std::string orderType = fields.at(1);
	if (orderType.compare("A") == 0) {

		// "Add order": 28800562 A c B 44.10 100
		//                     0 1 2 3     4   5

		order.timestamp = std::atol(fields.at(0).c_str());
		order.type = add;
		order.id = fields.at(2);

		// Order side
		std::string orderSide = fields.at(3);
		if (orderSide.compare("B") == 0) {
			order.side = buy;
		} else if (orderSide.compare("S") == 0) {
			order.side = sell;
		} else {
			throw BadParse();
		}

		order.price = static_cast<unsigned long int>(100 * std::atof(fields.at(4).c_str()));
		order.size = std::atol(fields.at(5).c_str());
	} else if (orderType.compare("R") == 0) {

		// "Reduce order": 28800744 R b 100
		//                        0 1 2   3

		order.timestamp = std::atol(fields.at(0).c_str());
		order.type = reduce;
		order.id = fields.at(2);
		order.size = std::atol(fields.at(3).c_str());
	} else { // bad parse
		throw BadParse();
	}

	FILE_LOG(logDEBUG) << "Parsed order: " << order.toString();
	return order;
}

#endif /* PARSER_H_ */
