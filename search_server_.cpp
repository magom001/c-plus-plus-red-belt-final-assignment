#include "search_server.h"
#include "iterator_range.h"
#include "profile.h"


#include <algorithm>
#include <iterator>
#include <sstream>
#include <iostream>
#include <numeric>

vector<string_view> SplitIntoWords(string_view line, TotalDuration &dest) {

    ADD_DURATION(dest);

    vector<string_view> result;
    result.reserve(1010);
    while (true) {
        size_t space = line.find(' ');
        result.push_back(line.substr(0, space));
        if (space == line.npos) {
            break;
        } else {
            line.remove_prefix(space + 1);
        }
    }
    return move(result);
}

SearchServer::SearchServer(istream &document_input) {
    UpdateDocumentBase(document_input, update);
}

void SearchServer::UpdateDocumentBase(istream &document_input, TotalDuration &dest) {
    ADD_DURATION(dest);
    InvertedIndex new_index;
    string current_document;
    for (; getline(document_input, current_document);) {
        new_index.Add(move(current_document), add);
    }

    index = move(new_index);
}


void SearchServer::AddQueriesStream(istream &query_input, ostream &search_results_output) {

    size_t docid_count_size = index.getDocsSize();
    vector<size_t> docid_count(docid_count_size, 0);
    vector<pair<size_t, int>> search_results;
    search_results.reserve(docid_count_size);

    for (string current_query; getline(query_input, current_query);) {
        search_results.clear();

        for (string_view word : SplitIntoWords(current_query, split)) {
            ADD_DURATION(lookup);
            for (const size_t &docid : index.Lookup(word)) {
                docid_count[docid] += 1;
            }
        }

        for (int i = 0; i < docid_count_size; i++) {
            ADD_DURATION(results);
            if (docid_count[i] != 0) {
                search_results.push_back({docid_count[i], -i});
                docid_count[i] = 0;
            }
        }


        partial_sort(
                make_move_iterator(begin(search_results)),
                make_move_iterator(min(end(search_results), (begin(search_results) + 5))),
                make_move_iterator(end(search_results)),
                [](const auto &a, const auto &b) {
                    return a > b;
                }
        );


        search_results_output << current_query
                              << ':';
        for (auto[hitcount, docid] : Head(search_results, 5)) {
            search_results_output << " {"
                                  << "docid: " << -docid << ", "
                                  << "hitcount: " << hitcount << '}';
        }
        search_results_output << '\n';
    }
}

void InvertedIndex::Add(string document, TotalDuration &dest) {
    ADD_DURATION(dest);
    docs.push_back(move(document));

    for (string_view word: SplitIntoWords(docs[doc_id], split)) {
        index[move(word)].push_back(doc_id);
    }

    doc_id++;
}

const list<size_t> &InvertedIndex::Lookup(string_view word) const {
    try {
        return (index.at(word));
    } catch (...) {
        return empty_;
    }
}

const size_t InvertedIndex::GetIndexSize() const {
    return index.size();
}
