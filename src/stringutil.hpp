#pragma once

#ifndef STRING_UTIL_HEADER_H_
#define STRING_UTIL_HEADER_H_

#include <string_view>
#include <string>
#include <vector>
#include "boardstate.hpp"

Position stringToPosition(std::string_view view);
std::string positionToString(Position pos);

bool isWhitespace(char c);

bool _isIn(char c, std::string_view s);

std::string join(const std::vector<std::string_view>& viewList, std::string joiner);
std::string join(const std::vector<std::string>& viewList, std::string joiner);

std::vector<std::string_view> split(std::string_view s, std::string_view delim = " \n\t", int splitCount = -1);
std::vector<std::string> split(const std::string& s, std::string_view delim = " \n\t", int splitCount = -1);

std::string_view strip(std::string_view s, std::string_view delim = " \n\t");
std::string strip(const std::string& s, std::string_view delim = " \n\t");


#endif // STRING_UTIL_HEADER_H_
