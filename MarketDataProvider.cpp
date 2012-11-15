//==========================================================================
// Name        : MarketDataProvider.cpp
// Author      : Gleb Chuvpilo
// Version     : 1.0
// Copyright   : (c) Gleb Chuvpilo, 2012
// Description : Market Data Provider
//==========================================================================

#include <iostream>
#include <fstream>
#include <iterator>
#include <algorithm> // copy

#include "Log.h"
#include "MarketDataProvider.h"
#include "Utils.h"

// Define static members
std::vector<std::string> trading::MarketDataProvider::messages_ = std::vector<std::string>();
std::vector<std::string>::iterator trading::MarketDataProvider::cur_ = trading::MarketDataProvider::messages_.begin();
std::string trading::MarketDataProvider::filename_ = std::string();

void trading::MarketDataProvider::readMarketDataFile(const std::string& filename) {
	messages_.clear();
	filename_ = filename;
	messages_ = readFile2Vector(filename_);
	FILE_LOG(logDEBUG) << "MarketDataProvider is initialized with " << messages_.size() << " messages";
	// std::copy(messages.begin(), messages.end(), std::ostream_iterator<std::string>(std::cout,"\n"));
	cur_ = trading::MarketDataProvider::messages_.begin();
}
