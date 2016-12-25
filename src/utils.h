#ifndef UTILS_H
#define UTILS_H
#include <string>
#include <iostream>
#include <cctype>
#include <ctime>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sstream>
#include <vector>
namespace utils{

static std::vector<std::string> split(const std::string& str, const std::string& dest) {
    size_t pos1 = 0;
    size_t pos2 = 0;
    std::vector<std::string> vec;
    bool quit = false;
    while (!quit) {
        pos2 = str.find(dest, pos1);
        if (pos2 == std::string::npos) {
            pos2 = str.length();
            quit = true;
        }
       std::string subStr = str.substr(pos1, pos2 - pos1);
        vec.push_back(std::move(subStr));
        pos1 = pos2 + dest.length();

        if (pos1 >= str.length()) {
            quit = true;
        }
    }
    return vec;
}

static bool trim(std::string& str) {
    if (str.empty()) {
        return false;
    }
    str.erase(0, str.find_first_not_of(" "));
    str.erase( str.find_last_not_of(" ") + 1);
    return true;
}

static bool trim(std::string& str, const std::string& s) {
    if (str.empty()) {
        return false;
    }
    str.erase(0, str.find_first_not_of(s));
    str.erase(str.find_last_not_of(s) + 1);
    return true;
}

static bool hasPrefix(const std::string& src, const std::string& dest) {
    if (dest.length() > src.length()) {
        return false;
    }
    if (src.substr(0, dest.length()) == dest) {
        return true;
    }
    return false;
}

static std::string getTime(){
    std::time_t t;
    std::time (&t);
    std::string tStr = std::ctime(&t);
    //去掉回车
    tStr.erase(tStr.size() - 1, 1);
    return tStr;
}

static void log(const std::string & msg) {
    std::cout<<getTime()<<":"<<msg<<"."<<std::endl;
}

static int string2int(const std::string& s) {
    std::istringstream is(s);
    int ret = 0;
    is>>ret;
    return ret;
}

static std::string int2string(int i) {
    std::stringstream is;
    is<<i;
    return is.str();
}

static bool isDigit(const std::string &str){
    if (str.length() == 0) {
        return false;
    }
    if ('0' > str[0] || '9'< str[0]) {
        if (str[0] != '+' && str[0] != '-') {
            return false;
        }
    }
    for(size_t i = 1; i < str.length(); i++){
        if('0' <=str[i] && str[i] <= '9'){
            continue;
        }else{
            return false;
        }
    }

    return true;
}
} // namespace

#endif // UTILS_H

