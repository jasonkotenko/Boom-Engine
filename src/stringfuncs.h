/*
	String helper functions
*/

#ifndef _STRINGFUNCS_H
#define _STRINGFUNCS_H

#include <string>
#include <vector>

using namespace std;

void split(const string& str,
           vector<string>& tokens,
           const string& delimiters);

#endif
