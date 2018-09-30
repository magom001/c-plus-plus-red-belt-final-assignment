#include "search_server.h"
#include "iterator_range.h"
#include "profile.h"

#include <algorithm>
#include <iterator>
#include <sstream>
#include <iostream>
#include <numeric>

vector<string> SplitIntoWords(const string &line) {
    istringstream words_input(line);
    return {istream_iterator<string>(words_input), istream_iterator<string>()};
}

SearchServer::SearchServer(istream &document_input) {
    UpdateDocumentBase(document_input);
}

void SearchServer::UpdateDocumentBase(istream &document_input) {
    InvertedIndex new_index;

    for (string current_document; getline(document_input, current_document);) {
        new_index.Add(move(current_document));
    }

    index = move(new_index);
}


void SearchServer::AddQueriesStream(
        istream &query_input, ostream &search_results_output
) {
    vector<size_t> docid_count_vector(50000);
    vector<size_t> indices(50000);


    for (string current_query; getline(query_input, current_query);) {

        fill(docid_count_vector.begin(), docid_count_vector.end(), 0);

        size_t t = 0;
        iota(indices.begin(), indices.end(), t++);

        const auto &&words = SplitIntoWords(current_query);

        for (const auto &word : words) {
            for (const size_t docid : index.Lookup(word)) {
                docid_count_vector[docid]++;
            }
        }

        partial_sort(
                begin(indices),
                min(begin(indices) + 5, end(indices)),
                end(indices),
                [&docid_count_vector](const size_t a, const size_t b) {
                    int64_t lhs_docid = a;
                    int64_t rhs_docid = b;

                    return make_pair(docid_count_vector[a], -lhs_docid) > make_pair(docid_count_vector[b], -rhs_docid);

                }
        );

        vector<pair<size_t, size_t>> result(5);
        for (size_t i = 0; i < 5; ++i) {
            result[i] = make_pair(indices[i], docid_count_vector[indices[i]]);
        }


        search_results_output << current_query << ':';
        for (auto[docid, hitcount] : result) {
            if (hitcount > 0) {
                search_results_output << " {"
                                      << "docid: " << docid << ", "
                                      << "hitcount: " << hitcount << '}';
            }
        }

        search_results_output << endl;
    }
}

void InvertedIndex::Add(const string &document) {
    docs.push_back(move(document));

    const size_t docid = docs.size() - 1;
    for (const auto &word : SplitIntoWords(document)) {
        index[word].push_back(docid);
    }
}

list<size_t> InvertedIndex::Lookup(const string &word) const {
    if (auto it = index.find(word); it != index.end()) {
        return it->second;
    } else {
        return {};
    }
}
