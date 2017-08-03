#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "stools.h"

////////////////////////////////////////////////////////////////////////////////

string trim(string& s,const string& drop)
{
    string r=s.erase(s.find_last_not_of(drop)+1);
    return r.erase(0,r.find_first_not_of(drop));
}

string rtrim(string s,const string& drop)
{
    string r=s.erase(s.find_last_not_of(drop)+1);
    return r;
}

string ltrim(string s,const string& drop)
{
    string r=s.erase(0,s.find_first_not_of(drop));
    return r;
}

vector<string> tokenize(const string& str,const string& delimiters)
{
    vector<string> tokens;
    string::size_type delimPos = 0, tokenPos = 0, pos = 0;

    if(str.length()<1)
        return tokens;

    while(1)
    {
        delimPos = str.find_first_of(delimiters, pos);
        tokenPos = str.find_first_not_of(delimiters, pos);

        if(string::npos != delimPos)
        {
            if(string::npos != tokenPos)
            {
                if(tokenPos<delimPos)
                {
                    tokens.push_back(str.substr(pos,delimPos-pos));
                }
                else
                {
                    tokens.push_back("");
                }
            }
            else
            {
                tokens.push_back("");
            }

            pos = delimPos+1;
        }
        else
        {
            if(string::npos != tokenPos)
            {
                tokens.push_back(str.substr(pos));
            }
            else
            {
                tokens.push_back("");
            }
            break;
        }
    }
    return tokens;
}
