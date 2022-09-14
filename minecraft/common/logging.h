//
// Created by Alex on 9/3/2022.
//

#ifndef MINECRAFT_LOGGING_H
#define MINECRAFT_LOGGING_H

#include <iostream>
#include <string>
#include <ctime>
#include <iomanip>

struct {

private:
    std::string getTime() {
        time_t now = time(nullptr);
        struct tm s{};
        char buf[80];
        s = *localtime(&now);
        strftime(buf, sizeof(buf), "%Y-%m-%d %X", &s);
        return buf;
    }

public:
    void log(const char *content) {
        std::cout << "[" + getTime() + "] " << content << std::endl;
    }

    void log(const std::string &content) {
        log(content.c_str());
    }

    void info(const char *source, const char *content) {
        std::cout << "[" + getTime() + "][" << source << "/INFO] " << content << std::endl;
    }

    void info(const char *content) {
        std::cout << "[" + getTime() + "][INFO] " << content << std::endl;
    }

    void info(const std::string &source, const std::string &content) {
        info(source.c_str(), content.c_str());
    }
    void info(const std::string &content) {
        info(content.c_str());
    }

    void err(const char *source, const char *content) {
        std::cout << "[" + getTime() + "][" << source << "/ERROR] " << content << std::endl;
    }

    void err(const char *content) {
        std::cout << "[" + getTime() + "][ERROR] " << content << std::endl;
    }

    void err(const std::string &source, const std::string &content) {
        info(source.c_str(), content.c_str());
    }
    void err(const std::string &content) {
        info(content.c_str());
    }



} console;

#endif //MINECRAFT_LOGGING_H
