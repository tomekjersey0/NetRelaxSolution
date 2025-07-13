#include "Util.h"

#include <iostream>
#include <unordered_map>
#include <sstream>


std::string App::Util::strip(const std::string& str) {
    std::string buffer = str;
    strip_ip(buffer);
    return buffer;
}

void App::Util::strip_ip(std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r\f\v");
    if (std::string::npos == first) {
        str.clear();
        return;
    }
    str.erase(0, first);

    size_t last = str.find_last_not_of(" \t\n\r\f\v");
    str.erase(last + 1);
}

void App::Util::printClientMsg(const std::string& msg, const std::string& username) {
    std::ostringstream out;
    out << "{" << username << "}: ";

    for (char c : msg) {
        auto it = nonPrintToCode.find(c);
        if (it != nonPrintToCode.end()) {
            out << "[\\" << it->second << "]";
        }
        else {
            out << c;
        }
    }

    std::cout << out.str() << std::endl;
}

std::string App::Util::parseClientMsg(const std::string& msg, const std::string& username) {
    std::ostringstream out;
    out << "{" << username << "}: " << msg;
    return out.str();
}

size_t App::Util::findLongsetStringLength(const std::vector<std::string>& vec) {
    size_t longest = 0;
    for (auto& s : vec) {
        longest = std::max(s.size(), longest);
    }
    return longest;
}

