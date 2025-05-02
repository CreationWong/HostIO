/**
 * @file HostIO.h
 * @author CreationWong
 */

#ifndef HOSTIO_H
#define HOSTIO_H
#include <ShlObj.h>
#include <string>
#include <vector>
#include <fstream>
#include <optional>

/**
 * @brief Hosts IO类
 */
class HostIO {
private:
    struct Host {
        std::string ipAddress;
        std::string hostName;
    };
    std::string path = R"(C:\Windows\System32\drivers\etc\hosts)";
    std::vector<Host>  hosts;
    bool IsAdmin = IsUserAnAdmin();

    std::ifstream existingIo;
    std::ofstream outIo;

public:
    HostIO();
    ~HostIO();
    void setPath(std::string path);
    void open();
    void addHost(const std::string &host, const std::string &ip);
    void delHost(std::string host);
    void writeHost();
    void showHosts();
    void showClass(); // DEBUG!
};


#endif //HOSTIO_H
