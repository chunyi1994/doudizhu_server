#ifndef LOGIN
#define LOGIN
#include <fstream>
#include <map>
#include "utils.h"
namespace doudizhu {

const char* DATABASE_LOGIN_PATH = "database/userinfo/";

static bool login_aux(const std::string& username, const std::string& password) {
    std::ifstream ifs(DATABASE_LOGIN_PATH + username);
    if (!ifs) {
        utils::log("不存在的用户名");
        return false;
    }
    std::string line;
    size_t pos;
    std::map<std::string, std::string> infomap;
    std::string key, value;
    while (std::getline(ifs, line)) {
        pos = line.find(":");
        if (pos == std::string::npos) continue;
        key = line.substr(0, pos);
        value = line.substr(pos + 1, line.size() - pos - 1);
        infomap[key] = value;
    }
    utils::log("正确的密码是：" + infomap["password"]);
    if (password != infomap["password"]) return false;
    utils::log("验证成功");
    return true;
}

static bool register_aux(const std::string& username, const std::string& password) {
    std::ifstream ifs(DATABASE_LOGIN_PATH + username);
    if (ifs) {
        utils::log("用户名已存在");
        return false;
    }
    std::ofstream ofs(DATABASE_LOGIN_PATH + username);
    if (!ofs) {
        utils::log("写入文件错误");
        return false;
    }
    ofs<<"password:"<<password<<"\n";
    utils::log("写入文件成功");
    return true;
}

} //namespace
#endif // LOGIN

