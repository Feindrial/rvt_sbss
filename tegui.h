#pragma once

#include <map>
#include <utility>
#include <string>
#include <unordered_map>
#include <vector>

enum tteos
{
    normal = 0,
    error = 1,
    info = 2
};
inline std::string teochar(tteos t)
{
    if (t == tteos::normal)
        return "0";
    else if (t == tteos::error)
        return "1";
    else if (t == tteos::info)
        return "2";
    else 
        return "9";
}

extern std::map<unsigned int, std::pair<std::string, std::string>> path_map;
extern std::vector<std::string> teostream;

void draw_file_info(const std::pair<std::string, std::string> &file_name, const std::unordered_map<std::string, size_t> &values);
void draw_file_gui();
void draw_st_output();