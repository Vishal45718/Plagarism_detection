#include "preprocessing.h"
#include "utils.h"
#include<sstream>
#include<fstream>
#include<unordered_set>
#include<algorithm>

using namespace std;
std::vector<std:: string> preprocess(const std:: string & text)
{
    vector<string> tokens;
    unordered_set<string> stopwords = load_stopwords("stopwords.txt");
    
    stringstream ss(text);
    string word;

    while(ss >> word)
    {
        transform(word.begin(), word.end() , word.begin(), ::tolower);
        word.erase(remove_if(word.begin(), word.end(), ::ispunct), word.end());
        if(!stopwords.count(word) && !word.empty())
        {
            tokens.push_back(word);
        }
    }
    return tokens;
}