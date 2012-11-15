//============================================================================
// Name        : MarketDataProvider.h
// Author      : Gleb Chuvpilo
// Version     : 1.0
// Copyright   : (c) Gleb Chuvpilo, 2012
// Description : Market Data Provider
//==========================================================================

#ifndef MARKETDATAPROVIDER_H_
#define MARKETDATAPROVIDER_H_

#include <string>
#include <vector>
#include "Exceptions.h"

namespace trading {

/**
 * Market Data Provider as Meyers' Singleton
 */
class MarketDataProvider{
public:

	// Singleton
	static MarketDataProvider& getInstance();

	// Read trades from market data file
	static void readMarketDataFile(const std::string& filename);

	// Not EOF?
	static bool hasNextMessage();

	// Get next message from the market data file
	static const std::string& nextMessage();

private:
	static std::string filename_;
	static std::vector<std::string> messages_;
	static std::vector<std::string>::iterator cur_;

// Singleton stuff
private:
	MarketDataProvider() { }
	MarketDataProvider(MarketDataProvider const&); // Don't Implement
	void operator=(MarketDataProvider const&);     // Don't implement
};

} // end of namespace


// Definitions of inline functions

inline trading::MarketDataProvider& trading::MarketDataProvider::getInstance() {
	static MarketDataProvider _instance; // Guaranteed to be destroyed. Instantiated on first use.
	return _instance;
}


inline bool trading::MarketDataProvider::hasNextMessage() {
	return (cur_ != messages_.end());
}


inline const std::string& trading::MarketDataProvider::nextMessage() {
	if (hasNextMessage()) {
		std::vector<std::string>::iterator saved = cur_;
		cur_++;
		//std::cout << "saved = " << *saved << std::endl;
		return *saved;
	} else {
		throw(trading::OutOfBounds());
	}
}

#endif /* MARKETDATAPROVIDER_H_ */
