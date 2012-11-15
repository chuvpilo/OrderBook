//============================================================================
// Name        : Utils.cpp
// Author      : Gleb Chuvpilo
// Version     : 1.0
// Copyright   : (c) Gleb Chuvpilo, 2012
// Description : Various utilities
//==========================================================================

#include <fstream>
#include <vector>

#include "Log.h"
#include "Utils.h"
#include "Exceptions.h"

namespace trading {

std::vector<std::string> readFile2Vector(const std::string& filename) {
	FILE_LOG(logDEBUG) << "Opening file " << filename.c_str();

	std::ifstream is(filename.c_str());

	if (!is) {
		throw trading::BadMarketDataFile();
	}

	std::string str;
	std::vector<std::string> lines;

	while (!is.eof()) {
		std::getline(is, str);
		lines.push_back(str);
	}

	is.close();
	FILE_LOG(logDEBUG) << "Number of lines read = " << lines.size();

	return lines;
}

}
