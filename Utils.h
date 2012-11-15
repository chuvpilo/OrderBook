//============================================================================
// Name        : Utils.h
// Author      : Gleb Chuvpilo
// Version     : 1.0
// Copyright   : (c) Gleb Chuvpilo, 2012
// Description : Various utilities
//==========================================================================

#ifndef UTILS_H_
#define UTILS_H_

#include <vector>
#include <string>
#include <fstream>
#include <sstream>



namespace trading {

// Read file to a vector of strings (each one is a line)
std::vector<std::string> readFile2Vector(const std::string& filename);

// Basic tokenizer
std::vector<std::string> tokenize(const std::string& str, const char& delimiter);

} // end of namespace


// Definitions of inline functions

inline std::vector<std::string> trading::tokenize(const std::string& str, const char& delimiter) {
	std::vector<std::string> fields;
	std::istringstream is(str);
	std::string s;
	while (std::getline(is, s, delimiter)) {
		fields.push_back(s);
	}
	return fields;
}

#endif /* UTILS_H_ */
