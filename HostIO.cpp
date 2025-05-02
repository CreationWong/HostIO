/**
 * @author CreationWong
 */

#include "HostIO.h"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#ifdef _WIN32
#include <direct.h>
#endif

using namespace std;

/**
 * @brief 如果没有管理员权限会抛出一个错误 - Not Admin
 */
HostIO::HostIO() {
    if (!IsAdmin) {
        throw runtime_error("Not Admin");
    }
}

HostIO::~HostIO() {
    if (existingIo.is_open()) {
        existingIo.close();
    }
    if (outIo.is_open()) {
        outIo.close();
    }
}

/**
 * @brief setPath
 * @param path Path 位置
 */
void HostIO::setPath(string path) {
    this->path = move(path);
}

/**
 * @brief 打开 Hosts 文件存储到结构体中
 */
void HostIO::open() {
    existingIo.open(path.c_str(), ios::in);

    if (!existingIo.is_open()) {
        throw runtime_error("Failed to open file: " + path);
    }

    if (existingIo.peek() == ifstream::traits_type::eof()) {
        throw runtime_error("File is empty: " + path);
    }

    string line;
    while (getline(existingIo, line)) {
        if (!line.empty() && line[0] != '#') {
            istringstream iss(line);
            string ip, host;
            if (iss >> ip >> host) {
                hosts.push_back({ip, host});
            }
        }
    }
}

void HostIO::addHost(const string &host, const string &ip) {
    hosts.push_back({ip, host});
}

/**
 * @brief 删除指定域名项
 * @param host 域名
 */
void HostIO::delHost(std::string host) {
    unordered_map<string, string> existingEntries;
    string line;
    existingIo.clear();
    existingIo.seekg(0, ios::beg);

    vector<string> originalLines;
    while (getline(existingIo, line)) {
        originalLines.push_back(line);
        if (!line.empty() && line[0] != '#') {
            istringstream iss(line);
            string ip, h;
            if (iss >> ip >> h) {
                existingEntries[h] = ip;
            }
        }
    }

    existingEntries.erase(host);

    outIo.open(path.c_str(), ios::out | ios::trunc);
    if (!outIo.is_open()) {
        throw runtime_error("Failed to open file for writing: " + path);
    }

    for (const auto& origLine : originalLines) {
        if (!origLine.empty() && origLine[0] != '#') {
            istringstream iss(origLine);
            string ip, h;
            if (iss >> ip >> h) {
                if (existingEntries.find(h) != existingEntries.end()) {
                    outIo << origLine << endl;
                }
            }
        } else {
            outIo << origLine << endl;
        }
    }
}

void HostIO::writeHost() {
    unordered_map<string, string> existingEntries;
    string line;
    existingIo.clear();
    existingIo.seekg(0, ios::beg);

    vector<string> originalLines;
    while (getline(existingIo, line)) {
        originalLines.push_back(line);
        if (!line.empty() && line[0] != '#') {
            istringstream iss(line);
            string ip, host;
            if (iss >> ip >> host) {
                existingEntries[host] = ip;
            }
        }
    }

    outIo.open(path.c_str(), ios::out | ios::trunc);
    if (!outIo.is_open()) {
        throw runtime_error("Failed to open file for writing: " + path);
    }

    unordered_set<string> writtenHosts;

    for (const auto& entry : hosts) {
        string newEntry = entry.ipAddress + " " + entry.hostName;
        if (existingEntries.find(entry.hostName) == existingEntries.end() || existingEntries[entry.hostName] != entry.ipAddress) {
            outIo << newEntry << endl;
            writtenHosts.insert(entry.hostName);
        }
    }

    for (const auto& origLine : originalLines) {
        if (!origLine.empty() && origLine[0] != '#') {
            istringstream iss(origLine);
            string ip, host;
            if (iss >> ip >> host) {
                if (writtenHosts.find(host) != writtenHosts.end()) {
                    continue;
                }
            }
        }
        outIo << origLine << endl;
    }

}

void HostIO::showHosts() {
    for (const auto&[ipAddress, hostName] : hosts) {
        cout << "IP = " << ipAddress << "\t" << "Hostname = " << hostName << endl;
    }
}

/**
 * @brief DEBUG 使用！
 * 请不要在生产环境使用！
 */
void HostIO::showClass() {
    if (IsAdmin)
        printf("Run As administrator\n");
    else
        printf("Run As user\n");
    cout << this->path << endl;
    for (const auto&[ipAddress, hostName] : hosts) {
        cout << "IP = " << ipAddress << "\t" << "Hostname = " << hostName << endl;
    }
}
