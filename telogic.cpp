#include "telogic.h"

#include "tegui.h"
#include "tesql.h"

#include "aho_corasick/aho_corasick.hpp"

#include <string>
#include <unordered_map>
#include <fstream>
#include <vector>
#include <thread>
#include <future>
#include <chrono>
#include <sstream>
#include <ratio>


//#define OLD_LOGIC


const unsigned int core_count = std::thread::hardware_concurrency();
const size_t excess = 1024; //1kb excess
std::vector<std::string*> splitted;
std::vector<std::unordered_map<std::string, size_t>*> splitted_counts;
std::vector<std::pair<std::string, std::string>> file_names;
aho_corasick::trie *trie = nullptr;

__attribute__((always_inline)) size_t go_to_newline(const std::string &str, const size_t start)
{
    size_t res = 0;
    for (size_t i = start; i < str.length(); i++)
    {
        if (str[i] == '\n') [[likely]]
            return res;
        else
            res++;
    }
    return static_cast<size_t>(0);
}

__attribute__((always_inline)) void split_the_text(const std::string &&str, const size_t count)
{
    size_t split_seek = 0;
    for (unsigned int i = 0; i < core_count; i++)
    {
        size_t excess_to_newline = go_to_newline(str, split_seek + count);
        auto &&howstr = str.substr(split_seek, count + excess_to_newline);
        splitted.back()[i].assign(howstr);

        split_seek += count + excess_to_newline;
    }
}

__attribute__((always_inline)) void process(unsigned int id, unsigned int core_id) 
{          
    const auto res = trie->parse_text(splitted[id][core_id]);

    for (const auto &r: res)
    {
        if (splitted_counts[id][core_id].contains(r.get_keyword())) [[likely]]
            splitted_counts[id][core_id][r.get_keyword()]++;
        else
            splitted_counts[id][core_id][r.get_keyword()] = 1;
    }        
}

__attribute__((always_inline)) void recorrect_trie()
{
    if (!trie) [[unlikely]]
        trie = new aho_corasick::trie;
    else
    {
        delete trie;
        trie = new aho_corasick::trie;
    }

    for (const auto &e: errors)
    {
        trie->insert(e.second);
    }
}

__attribute__((always_inline)) void at_file_logic_clear_single(int id)
{
    if (splitted[id])
    {
        delete[] splitted[id];
        splitted[id] = nullptr;
    }
    splitted.erase(splitted.begin() + id);
    if (splitted_counts[id])
    {
        delete[] splitted_counts[id];
        splitted_counts[id] = nullptr;
    }
    splitted_counts.erase(splitted_counts.begin() + id);
    file_names.erase(file_names.begin() + id);    
}

__attribute__((always_inline)) void at_file_logic_clear()
{
    for (auto &sp: splitted)
    {
        if (sp)
        {
            delete[] sp;        
            sp = nullptr;
        }
    }
    splitted.clear();
    for (auto &spc: splitted_counts)
    {
        if (spc)
        {
            delete[] spc; 
            spc = nullptr;
        }
    }
    splitted_counts.clear();
    file_names.clear();
}

__attribute__((always_inline)) void at_file_logic()
{
    //CORRECTION IF CHANGED
    at_file_logic_clear();

    for (const auto p: path_map)
    {
        if (p.second.first == "" && p.second.second == "")
            continue;

        std::ifstream t(p.second.second);
        std::string str;

        t.seekg(0, std::ios::end);   
        const auto res_str = t.tellg();
        str.reserve(res_str);
        t.seekg(0, std::ios::beg);

        str.assign((std::istreambuf_iterator<char>(t)),
                    std::istreambuf_iterator<char>());

        const size_t th_str_count = str.length() / core_count;

        splitted_counts.push_back(new std::unordered_map<std::string, size_t>[core_count]);
        splitted.push_back(new std::string[core_count]);
        file_names.emplace_back(p.second.first, p.second.second);
        for (unsigned int i = 0; i < core_count; i++)
        {
            splitted.back()[i].reserve(th_str_count + excess);
        }
        split_the_text(std::move(str), th_str_count);
    }

    if (!trie) [[unlikely]]
            recorrect_trie();
}

__attribute__((always_inline)) void at_logic()
{
    std::chrono::high_resolution_clock::time_point start;
    std::chrono::high_resolution_clock::time_point finish;
    if (splitted.size() != 0)
        start = std::chrono::high_resolution_clock::now();

#if defined(OLD_LOGIC)

    for (int i = 0; i < splitted.size(); i++)
    {
        std::vector<std::thread> threads;
        for (int j = 0; j < core_count - 1; j++)
        {
            threads.emplace_back(process, i, j);
        }
        process(i, core_count - 1);
        for (unsigned int i = 0; i < core_count - 1; ++i)
        {
            threads[i].join();
        }        
    }

#else

    std::vector<std::future<void>> futures;

    for (int i = 0; i < splitted.size(); i++)
    {
        for (int j = 0; j < core_count; j++)
        {
            if (i == (splitted.size() - 1) && j == (core_count - 1)) [[unlikely]]
                break;

            futures.emplace_back(std::async(std::launch::async, process, i, j));
        }
    }
    if (splitted.size() != 0) [[likely]]
        process(splitted.size() - 1, core_count - 1);
    for (auto &f : futures)
    {
        f.get();
    }

#endif

    if (splitted.size() != 0)
    {
        finish = std::chrono::high_resolution_clock::now();
        std::ostringstream out;
        out << static_cast<std::chrono::duration<double, std::ratio<1>>>(finish - start);
        teostream.emplace_back(teochar(tteos::info) + "Execution time is: " 
                                                    + out.str()
                                                    + '\n');
    }

    //OUTPUT AND CLEAR FOR OTHER FILE
    int iiii = 0;
    for (auto &spc: splitted_counts)
    {
        for (unsigned int i = 1; i < core_count; i++)
        {
            for (const auto &kv : spc[i])
            {
                if (spc[0].contains(kv.first))  [[likely]]
                    spc[0][kv.first] += kv.second;
                else
                    spc[0][kv.first] = kv.second;            
            }
        }

        draw_file_info(file_names[iiii], spc[0]);
        for (unsigned int i = 0; i < core_count; i++)
        {
            spc[i].clear();
        }
        iiii++;
    }
}