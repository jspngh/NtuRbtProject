#ifndef M_SERVER
#define M_SERVER

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT 4000
#define DEFAULT_INTERFACE "wlp2s0"

#include <netinet/in.h>
#include <string>

class Server
{
    public:
        int client_sock;
        struct in_addr ip_address;
        std::string last_response;

        Server();
        ~Server();
        int openSocket();
        int closeSocket();
        int sendCommand(const char* sendbuf, int client_sock);
        std::string getLastResponse();
        void error(std::string msg);

};

#endif
