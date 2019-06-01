#include "../include/stringutil.hpp"

Position stringToPosition(std::string_view view) {
	if (view.size() != 2)	return { -1, -1 };
	auto lowerFirst = std::tolower(view.front());
	if (lowerFirst < 'a' || lowerFirst > 'h')	return { -1, -1 };
	auto second = view.back();
	if (second < '1' || second > '8')			return { -1, -1 };
	return { second - '1', lowerFirst - 'a' };
}

std::string positionToString(Position pos) {
	static auto validate = [](auto v) { return v >= 0 && v <= 7; };
	if (!validate(pos.first) || !validate(pos.second))	return "";
	char c1 = pos.second + 'a';
	char c2 = pos.first + '1';
	return { c1, c2 };
}

bool isWhitespace(char c) {
	return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

bool _isIn(char c, std::string_view s) {
	return s.find(c) != s.npos;
}

std::string join(const std::vector<std::string_view>& viewList, std::string joiner) {
	std::string result;

	if (!viewList.size())	return "";
	else if (viewList.size() == 1)	return std::string{ viewList[0] };

	for (size_t i = 0; i < viewList.size() - 1; ++i) {
		result += std::string{ viewList[i] } +joiner;
	}

	if (viewList.size())
		result += viewList.back().data();

	return result;
}

std::string join(const std::vector<std::string>& viewList, std::string joiner) {
	std::string result;

	if (!viewList.size())	return "";
	else if (viewList.size() == 1)	return viewList[0];

	for (size_t i = 0; i < viewList.size() - 1; ++i) {
		result += viewList[i] + joiner;
	}

	if (viewList.size())
		result += viewList.back().data();

	return result;
}

std::vector<std::string_view> split(std::string_view s, std::string_view delim, int splitCount) {
	std::vector<std::string_view> splitVec;

	while (!s.empty()) {
		bool broke = false;
		for (size_t i = 0; i < s.size(); ++i) {
			if (_isIn(s[i], delim)) {
				splitVec.push_back(s.substr(0, i));
				s.remove_prefix(i + 1);
				if (!(splitVec.size() >= splitCount && splitCount > 0))
					broke = true;
				break;
			}
		}

		if (!broke) {
			splitVec.push_back(s);
			break;
		}
	}

	return splitVec;
}

std::vector<std::string> split(const std::string& s, std::string_view delim, int splitCount) {
	std::vector<std::string> splitVec;

	size_t idx = 0;
	while (idx < s.size()) {
		bool break_ = false;
		for (size_t i = idx; i < s.size(); ++i) {
			if (_isIn(s[i], delim)) {
				splitVec.emplace_back(s.substr(idx, i - idx));
				idx = i + 1;
				if (splitCount == splitVec.size() && splitCount > 0)
					break_ = true;
				break;
			}
			if (i == s.size() - 1)	break_ = true;
		}
		if (break_)	break;
	}

	if (idx < s.size()) {
		splitVec.emplace_back(s.substr(idx));
	}

	return splitVec;
}

std::string_view strip(std::string_view s, std::string_view delim) {
	while (s.size() && _isIn(s[0], delim))
		s.remove_prefix(1);
	while (s.size() && _isIn(s.back(), delim))
		s.remove_suffix(1);
	return s;
}

std::string strip(const std::string& s, std::string_view delim) {
	int beg = 0;
	int end = s.size() - 1;
	while (beg < s.size() && _isIn(s[beg], delim))
		beg++;
	while (end < s.size() && _isIn(s[end], delim))
		end--;
	return s.substr(beg, end - beg + 1);
}

std::string interleave(const std::vector<std::string_view>& first,
					   const std::vector<std::string_view>& second,
					   const std::string& between, const std::string& end)
{
	std::string result;
	auto minSize = std::min(first.size(), second.size());
	auto maxSize = std::max(first.size(), second.size());
	auto& remainder = first.size() == maxSize ? first : second;

	for (size_t i = 0; i < minSize; ++i) {
		result += std::string{ first[i] } + between + std::string{ second[i] } + end;
	}

	for (size_t i = minSize; i < maxSize; ++i) {
		result += std::string{ remainder[i] } + end;
	}

	return result;
}

std::string interleave(const std::vector<std::string>& first,
					   const std::vector<std::string>& second,
					   const std::string& between, const std::string& end) {
	std::string result;
	auto minSize = std::min(first.size(), second.size());
	auto maxSize = std::max(first.size(), second.size());
	auto& remainder = first.size() == maxSize ? first : second;

	for (size_t i = 0; i < minSize; ++i) {
		result += first[i] + between + second[i] + end;
	}

	for (size_t i = minSize; i < maxSize; ++i) {
		result += remainder[i] + end;
	}

	return result;
}