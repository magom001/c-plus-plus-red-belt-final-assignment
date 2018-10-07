#include "search_server.h"
#include "iterator_range.h"
#include "profile.h"


#include <algorithm>
#include <iterator>
#include <sstream>
#include <iostream>
#include <numeric>
#include <future>

vector<string_view> SplitIntoWords(string_view line) {
    vector<string_view> result;
    result.reserve(1010);
    while (true) {
        size_t space = line.find(' ');
        auto w = line.substr(0, space);
        if (w != "") {
            result.push_back(move(w));
        }
        if (space == line.npos) {
            break;
        } else {
            line.remove_prefix(space + 1);
        }
    }
    return move(result);
}

SearchServer::SearchServer(istream &document_input) {
    UpdateDocumentBase(document_input);
}

InvertedIndex BuildIndex(istream &document_input) {
    InvertedIndex new_index;
    string current_document;
    for (; getline(document_input, current_document);) {
        new_index.Add(move(current_document));
    }
    return new_index;
}

void SearchServer::UpdateDocumentBase(istream &document_input) {
    future<InvertedIndex> new_index_fut = async(BuildIndex, ref(document_input));

    index.GetAccess().ref_to_value = move(new_index_fut.get());
}


void SearchServer::AddQueriesStream(istream &query_input, ostream &search_results_output) {

    size_t docid_count_size = index.GetAccess().ref_to_value.getDocsSize();
    vector<pair<size_t, int>> lookup_results(docid_count_size);


    for (auto i = 0; i < docid_count_size; ++i) {
        lookup_results[i].first = 0;
        lookup_results[i].second = -i;
    }
    vector<pair<size_t, int>> lookup_results_copy = lookup_results;

    for (string current_query; getline(query_input, current_query);) {

        lookup_results = lookup_results_copy;

        for (string_view word : SplitIntoWords(current_query)) {
            for (const pair<size_t, size_t> &docid : index.GetAccess().ref_to_value. Lookup(word)) {

                lookup_results[docid.first].first += docid.second;
            }
        }


        partial_sort(
                begin(lookup_results), min(end(lookup_results), (begin(lookup_results) + 5)),
                end(lookup_results),
                [](const auto &a, const auto &b) { return a > b; }
        );


        search_results_output << current_query << ':';
        for (auto[hitcount, docid] : Head(lookup_results, min(size_t(5), docid_count_size))) {
            if (hitcount != 0) {
                search_results_output << " {"
                                      << "docid: " << -docid << ", "
                                      << "hitcount: " << hitcount << '}';
            }
        }
        //search_results_output << endl;
        search_results_output << '\n';


    }
}


void InvertedIndex::Add(string document) {
    docs.push_back(move(document));
    const size_t docid = docs.size() - 1;

    for (string_view word : SplitIntoWords(docs[docid])) {
        if (auto it = find_if(index[word].begin(), index[word].end(),
                              [docid](const auto &a) { return a.first == docid; });
                it != index[word].end()) {
            (*it).second += 1;
        } else {
            index[word].push_back({docid, 1});
        }
    }
}

const vector<pair<unsigned short int, size_t>> &InvertedIndex::Lookup(string_view word) const {
    try {
        return (index.at(word));
    } catch (...) {
        return empty_;
    }
}
