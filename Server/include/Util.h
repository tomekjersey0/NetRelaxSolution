#pragma once

#include "Server.h"

#include <string>
#include <algorithm>

namespace App {
	class Util {
	private:
		static inline const std::unordered_map<char, char> nonPrintToCode = {
			{'\n', 'n'},
			{'\t', 't'},
			{'\r', 'r'},
			{'\f', 'f'},
			{'\v', 'v'}
		};

	public:
		static std::string strip(const std::string& str);
		static void strip_ip(std::string& str);
		static void printClientMsg(const std::string& msg, const std::string& username);
		static std::string parseClientMsg(const std::string& msg, const std::string& username);
		static size_t findLongsetStringLength(const std::vector<std::string>& vec);

		template <typename MapType>
		static size_t findLongestKeyLength(const MapType& map) {
			size_t longest = 0;
			for (const auto& item : map) {
				longest = std::max(longest, item.first.size());
			}
			return longest;
		}
	};
}