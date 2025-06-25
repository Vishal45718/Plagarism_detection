#include <iostream>
#include <vector>
#include <string>
#include "preprocessing.h"
#include "similarity.h"
#include "utils.h"
using namespace std;

int main()
{
    string file1 = "data/doc1.txt";
    string file2 = "data/doc2.txt";

    string text1 = read_file(file1);
    string text2 = read_file(file2);

    vector<string> tokens1 = preprocess(text1);
    vector<string> tokens2 = preprocess(text2);

    double jaccard = jaccard_similarity(tokens1, tokens2);

    cout << "Jaccard Similarity: " << jaccard * 100 << "%" << endl;

    return 0;
}