// convert 
#include <string>
#include <iostream>
#include "convert.h"
using namespace std;

char // first character removed from str
pop (string& str) { char c = str.front(); str = str.substr(1); return c; }

string // a term after occurances of c are replaced by rep
replace (char c, string rep, string term, char cc[4])
{
    if (term.empty()) return term;
    
    string termC = term; char v;

    if (term.size()==1 && term[0]==c) return rep; // base case, a replacement

    else if (isLambda(term, cc)) // we need to make sure we don't overwrite
    {   
        string termC = term, abs; 
        while ((abs+=pop(termC)).back()!=cc[2]) if (abs.back()==c) return term;
        termC.pop_back(); // drop } ({ dropped in while)
     // here we can assume term didn't re-abstract c and is a list of terms
        string result;
        while(!(term=getNext(termC, cc)).empty()) // replace in each sub-term 
            result += replace(c, rep, term, cc);
        return abs + result + cc[3]; // put back the {..\ and }
    }
    else if (term.size()>1) // is {...}
    {
        pop(termC); termC.pop_back(); // drop {}
        string result;
        while(!(term=getNext(termC, cc)).empty()) // replace in each sub-term
            result += replace(c, rep, term, cc);
        return cc[1] + result + cc[3];
    } 
    else /* a char not c*/ return term;

    return term; // probably shouldn't get here
}

bool // returns if there is a '\' in the outer scope, before { or }
isLambda (string str, char cc[4])
{
    if (str.front()==cc[1]) pop(str); // drop the first {

    for (char c : str) // switch (c)
    {
        if(c==cc[1]||c==cc[3]) return false;
        else if (c==cc[2]) return true;
        else continue;
    }
    return false;
}

string // returns the next valid lambda term, either {...} or 1 character
getNext (string& str, char cc[4])
{
    if (str.empty()) return str;
    // add here global error check for {} mismatch
    string result; char c;
    if ((c=pop(str))==cc[1]) // {...}
    {
        result += c;
        while (str.front()!=cc[3]) result += getNext(str, cc);
        result += pop(str);
    }
    else result = c; // a character
    return result;
}

string // main function to convert bfx to bf
convert (string str, char cc[4])
{
    if (str.empty()) return str;

    string result, term, next;
    while (!(term=getNext(str, cc)).empty()) // for the whole stack
    {
        // cout << "evaluating " << term << " off " << str << '\n';
        char c;
        if (isLambda(term, cc)) // need to make replacements
        {
            pop(term); // drop {
            if ((c=pop(term))!=cc[2]) // until we get to the body
            {
                term = cc[1] + term; // replace needs a valid term
                if (!isspace(c))
                {
                    do next = getNext(str, cc); 
                    while (next.empty()?false:isspace(next.front()));
                    term = replace(c, next, term, cc);
                }
            }
            else term.pop_back(); // were at the body, drop } from end 
            str = term + str; // put body onto stack
        }
        else if ((c=pop(term))==cc[1]) // drop parens
        {
            term.pop_back(); 
            str = term + str; 
        }        
        // else if (def.count(c)>0) str = def.at(c) + str; old replacement

        else result += c; // an undefined char is dropped (identity function)
    }
    return result;
}
