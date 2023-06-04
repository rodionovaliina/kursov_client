//C++
#include <iostream>
#include <memory>
#include <random>
#include <system_error>
#include <set>
#include <iomanip>
#include <string>
#include <string_view>
#include <limits>

//C
#include <cerrno>
#include <cstdlib>
#include <csignal>
#include <ctime>

//UNIX & POSIX
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

//Crypto++
#include <cryptopp/cryptlib.h>
#include <cryptopp/hex.h>
#include <cryptopp/files.h>
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include <cryptopp/md5.h>

static constexpr std::string_view login = "user";
static constexpr std::string_view password = "P@ssW0rd";
static constexpr int auth_error = 10001;
static constexpr int variant_error = 10002;

volatile static sig_atomic_t sig_status = 0;

std::string_view getVariant(int argc, char** argv);

void term_handler(int status)
{
    sig_status = status;
}

std::set<std::string_view> Variants = {
    "float",
    "double",
    "int16_t",
    "uint16_t",
    "int32_t",
    "uint32_t",
    "int64_t",
    "uint64_t"
};

class Worker
{
private:
    std::string_view var;
    int work_sock;
    std::string str_read();
    void auth();
    void calculate();
    template <typename T> void calc();
public:
    Worker()=delete;
    Worker(std::string_view v);
    void operator()(int sock);
};

class Listener
{
private:
    int sock;
    sockaddr_in * self_addr;
    int queueLen;

public:
    Listener(unsigned short port=33333, int qlen=10);
    ~Listener();
    void Run(Worker & worker);
};


int main(int argc, char **argv)
{
    signal(SIGQUIT, term_handler);
    signal(SIGTERM, term_handler);
    // signal(SIGINT, term_handler);
    signal(SIGHUP, term_handler);
    Worker w(getVariant(argc, argv));
    Listener server;
    server.Run(w);
}

std::string_view getVariant(int argc, char** argv)
{
    int opt;
    std::string_view variant = "";
    while( (opt = getopt(argc, argv, "k:h")) != -1) {
        switch (opt) {
        case 'k' :
            variant = std::string_view(optarg);
            break;
        case 'h' :
        case '?' :
        case ':' :
            std::cerr << "Usage: server -k variant\n";
            std::cerr << "where variant is:\n";
            for (auto v: Variants) {
                std::cerr << '\t' << v << std::endl;
            }
            exit(1);
        }
    }
    if (Variants.find(variant) == Variants.end())
        throw std::system_error(variant_error, std::system_category(), "Unknown variant");
    return variant;
}

Listener::Listener(unsigned short port, int qlen):sock(socket(AF_INET, SOCK_STREAM, 0)), self_addr(new sockaddr_in), queueLen(qlen)
{
    // check error for socket creation
    if (sock == -1)
        throw std::system_error(errno, std::generic_category());
    int on = 1;
    int rc = setsockopt (sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof on);
    if ( rc == -1 )
        throw std::system_error(errno, std::generic_category());
    // fill self address
    self_addr->sin_family = AF_INET;
    self_addr->sin_port = htons(port);
    self_addr->sin_addr.s_addr = 0;
    // bind socket for self address
    if (bind(sock, reinterpret_cast<const sockaddr*>(self_addr), sizeof (sockaddr_in)) == -1)
        throw std::system_error(errno, std::generic_category());
}

Listener::~Listener()
{
    close(sock);
    delete self_addr;
}
void Listener::Run(Worker & worker)
{
    if (listen(sock, queueLen) == -1)
        throw std::system_error(errno, std::generic_category());
    while(sig_status == 0) {
        sockaddr_in * foreign_addr = new sockaddr_in;
        socklen_t socklen = sizeof (sockaddr_in);
        int work_sock = accept(sock, reinterpret_cast<sockaddr*>(foreign_addr), &socklen);
        if (work_sock == -1)
            throw std::system_error(errno, std::generic_category());
        std::string ip_addr(inet_ntoa(foreign_addr->sin_addr));
        std::clog << "log: Connection established with " << ip_addr <<std::endl;
        try {
            worker(work_sock);
        } catch (std::system_error &e) {
            std::cerr << e.what() << "\nConnection with " << ip_addr << " aborted\n";
        }
        close(work_sock);
        delete foreign_addr;
    }
}

Worker::Worker(std::string_view v):var(v), work_sock(-1)
{
    std::clog << "log: Select variant " << var << std::endl;
}

void Worker::operator()(int sock)
{
    work_sock = sock;
    auth();
    calculate();
}

template <typename T> void Worker::calc()
{
    uint32_t num_vectors, vector_len;
    int rc;
    std::clog << "log: Start calculate with type " << typeid(T).name() << std::endl;
    rc = recv(work_sock, (void *) &num_vectors, sizeof num_vectors, 0);
    if (rc == -1)
        throw std::system_error(errno, std::generic_category());
    std::clog << "log: Numbers of vectors " << num_vectors << std::endl;
    for (uint32_t i = 0; i < num_vectors; ++i) {
        rc = recv(work_sock, (void *) &vector_len, sizeof vector_len, 0);
        if (rc == -1)
            throw std::system_error(errno, std::generic_category());
        std::clog << "log: Vector " << i <<" size " << vector_len << std::endl;
        T* data = new T[vector_len];
        rc = recv(work_sock, (void *) data, sizeof(T) * vector_len, 0);
        if (rc == -1)
            throw std::system_error(errno, std::generic_category());
        T sum = 0;

        for (uint32_t i = 0; i < vector_len; ++i) {
            sum += data[i];
            if (std::is_integral<T>::value) {               // if interer types
                if (std::is_signed<T>::value) {             // if signed
                    if (data[i] > 0 and sum < 0 and (sum - data[i]) > 0) { //if overflow
                        sum = std::numeric_limits<T>::max();
                        break;
                    } else if (data[i] < 0 and sum > 0 and (sum - data[i]) < 0) {  //if undeflow
                        sum = std::numeric_limits<T>::min();
                        break;
                    }
                } else if (sum < data[i]) {                 // if unsigned, if overflow
                    sum = std::numeric_limits<T>::max();
                    break;
                }

            }

        }
        delete[] data;
        rc = send(work_sock, (void*)&sum, sizeof(T), 0);
        if (rc == -1)
            throw std::system_error(errno, std::generic_category());
        std::clog << "log: Sended vector summ " << sum << std::endl;
    }
}

void Worker::calculate()
{
    if (var == "float")
        Worker::calc<float>();
    else if (var == "double")
        calc<double>();
    else if (var == "uint16_t")
        calc<uint16_t>();
    else if (var == "int16_t")
        calc<int16_t>();
    else if (var == "uint32_t")
        calc<uint32_t>();
    else if (var == "int32_t")
        calc<int32_t>();
    else if (var == "uint64_t")
        calc<uint64_t>();
    else
        calc<int64_t>();
}

void Worker::auth()
{
    using namespace CryptoPP;
    int rc;
    std::mt19937_64 gen(time(nullptr));
    if (str_read() != login)
        throw std::system_error(auth_error, std::system_category(), "Unknown user");
    std::clog << "log: username ok\n";
    uint64_t rnd = gen();
    std::string salt, message;
    StringSource((const byte*)&rnd, 8, true, new HexEncoder(new StringSink(salt)));
    rc = send(work_sock, salt.c_str(), 16, 0);
    if (rc == -1)
        throw std::system_error(errno, std::generic_category());
    std::clog << "log: sending SALT " << salt << std::endl;
    Weak::MD5 hash;
    StringSource(salt + std::string(password), true, new HashFilter( hash, new HexEncoder(new StringSink(message))));
    std::clog << "log: waiting MESSAGE " << message << std::endl;
    if (str_read() != message)
        throw std::system_error(auth_error, std::system_category(), "Authentication error");
    std::clog <<"log: auth success, sending OK\n";
    rc = send(work_sock, "OK", 2, 0);
    if (rc == -1)
        throw std::system_error(errno, std::generic_category());
}

std::string Worker::str_read()
{
    size_t buflen = 1024;
    char * buf = new char[buflen];
    while ((size_t)recv(work_sock, (void*)buf, buflen, MSG_PEEK) == buflen) {
        delete[] buf;
        buflen *= 2;
        buf = new char[buflen];
    }
    int rc = recv(work_sock, (void*)buf, buflen, 0);
    if (rc == -1)
        throw std::system_error(errno, std::generic_category());
    auto res = std::string(buf, rc);
    res.resize(res.find_last_not_of("\n\r") + 1);
    delete[] buf;
    return res;
}

