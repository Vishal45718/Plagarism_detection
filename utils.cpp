#include"utils.h"
#include<fstream>
#include<sstream>

using namespace std;

string read_file(const string &filename) {
    ifstream file(filename);
    ostringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

unordered_set<string> load_stopwords(const string &filename) {
    unordered_set<string> stopwords;
    string line;
    ifstream file(filename);
    while (getline(file, line)) {
        stopwords.insert(line);
    }
    return stopwords;
}