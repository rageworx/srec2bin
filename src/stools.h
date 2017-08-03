#ifndef __STOOLS_H__
#define __STOOLS_H__

#include <string>
#include <vector>

using namespace std;

string trim(string& s,const string& drop = " ");
string rtrim(string s,const string& drop = " ");
string ltrim(string s,const string& drop = " ");
vector<string> tokenize(const string& str,const string& delimiters);

#endif // of __STOOLS_H__
