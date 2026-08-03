#ifndef SERVER_HPP
#define SERVER_HPP
#include <cerrno>
#include <iomanip>
#include <fcntl.h>
#include "channel.hpp"
#include "clients.hpp"
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include<iostream>
#include<sstream>
#include<vector>
#include<map>
#include<poll.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/poll.h>
#include<sys/types.h>
#include <stdlib.h>
std::vector<std::string> splitByComma(const std::string &s);
std::string tolow(const std::string &str);
class server
{
    private:
    int port;
    std::string passwd;
    int socket_file;
    static volatile sig_atomic_t running;
    void setup_listener();
    void install_signal_handlers();
    void run_event_loop();
    void accept_client();
    void handle_client_event(size_t index);
    void remove_client(size_t index);
    static void handle_signal(int signum);
    static pollfd make_pollfd(int fd);
    void close_socks(std::vector<pollfd> &fds);
public:
    void handle_client_line(Clients &cl, const std::string &line);
    void handle_commands(size_t client_index);
    void process_client_buffers();
    void cl_registration(Clients &client, const std::string &cmd);
    void cl_ping(Clients &client);
    void cl_join(Clients &client);
    void cl_part(Clients &client);
    void cl_privmsg(Clients &client);
    void cl_topic(Clients &client);
    void cl_kick(Clients &client);
    void cl_invite(Clients &client);
    void cl_mode(Clients &client);
    bool send_msg(Clients &client);
    void flush_out_buffers();
    bool nickname_coll(const std::string &nickname) const;
    bool valid_nick(const std::string &nick) const;
    bool valid_username(const std::string &username) const;
    std::map<std::string, Channel> channels;
    std::vector<Clients> client;
    std::vector<pollfd> fds;
    server(std::string port, std::string passwd);
    int init();
};
int parsePort(const std::string &port);
bool passwordHasNoWhitespace(const std::string &passwd);
#endif
