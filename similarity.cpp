#include"similarity.h"
#include <unordered_set>
#include <algorithm>
using namespace std;

double jaccard_similarity(const vector<string> &tokens1, const vector<string> &tokens2) {
   
    unordered_set<string> set1(tokens1.begin(), tokens1.end());
    unordered_set<string> set2(tokens2.begin(), tokens2.end());
    
    int intersection =0;

    for(const string &word : set1)
    {
        if(set2.count(word))
        {
            intersection++;
        }
    }
    
    int union_size = set1.size() + set2.size() - intersection;
    return static_cast<double>(intersection) / union_size;
}


