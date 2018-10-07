#pragma once

#include "add_duration.h"
#include "synchronized.h"

#include <istream>
#include <ostream>
#include <set>
#include <list>
#include <vector>
#include <map>
#include <string>
#include <unordered_map>


using namespace std;

vector<string_view> SplitIntoWords(string_view line);

class InvertedIndex {
public:
    InvertedIndex() {
        docs.reserve(50000);
        index.reserve(10010);
    }

    void Add(string document);

    const size_t GetIndexSize() const;

    const vector<pair<unsigned short int, size_t>> &Lookup(string_view word) const;

    const string &GetDocument(size_t id) const {
        return docs[id];
    }

    size_t getDocsSize() {
        return docs.size();
    }

private:
    vector<pair<unsigned short int, size_t>> empty_ = {};
    unordered_map<string_view, vector<pair<unsigned short int, size_t>>> index;
    vector<string> docs;
    size_t doc_id = 0;
};

class SearchServer {
public:
    SearchServer() = default;

    explicit SearchServer(istream &document_input);

    void UpdateDocumentBase(istream &document_input);

    void AddQueriesStream(istream &query_input, ostream &search_results_output);

private:
    TotalDuration reset{"reset"};
    TotalDuration lookup{"Lookup"};
//    TotalDuration split{"AddQueries SplitWords"};
    TotalDuration psort{"AddQueries partial_sort"};
//    TotalDuration add{"InvertedIndex add"};
//    TotalDuration update{"InvertedIndex update"};

    Synchronized<InvertedIndex> index;
};
