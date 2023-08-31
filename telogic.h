#pragma once

#include "aho_corasick/aho_corasick.hpp"

#include <thread>
#include <string>
#include <vector>
#include <utility>
#include <unordered_map>

extern const unsigned int core_count;
extern const size_t excess;
extern std::vector<std::string*> splitted;
extern std::vector<std::unordered_map<std::string, size_t>*> splitted_counts;
extern std::vector<std::pair<std::string, std::string>> file_names;
extern aho_corasick::trie *trie;

size_t go_to_newline(const std::string &str, const size_t start);
void split_the_text(const std::string &&str, const size_t count);
void process(unsigned int id, unsigned int core_id);
void recorrect_trie();
void at_file_logic_clear_single(int id);
void at_file_logic_clear();
void at_file_logic();
void at_logic();