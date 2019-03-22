/*!
 *  Copyright (c) 2014-2019 by Contributors
 * \file socket.cc
 * \brief this file aims to provide a wrapper of sockets implementation
 * \author Tianqi Chen, Chen Qin
 */

#include "socket.h"

namespace rabit {
namespace utils {
/*!
 * \brief a wrapper of TCP socket that hopefully be cross platform
 */
void TCPSocket::SetKeepAlive(bool keepalive) {
    int opt = static_cast<int>(keepalive);
    if (setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE,
                   reinterpret_cast<char*>(&opt), sizeof(opt)) < 0) {
        Socket::Error("SetKeepAlive");
    }
}

/*!
* \brief create the socket, call this before using socket
* \param af domain
*/
void TCPSocket::Create(int af) {
    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd == INVALID_SOCKET) {
        Socket::Error("Create");
    }

    // TODO (chenqin): improve conn in time-wait state handling
    // int optval = 1;
    // if( setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof (optval)) < 0){
    //    Socket::Error("Reuse Port");
    // }
}

/*!
* \brief perform listen of the socket
* \param backlog backlog parameter
*/
void TCPSocket::Listen(int backlog) {
    listen(sockfd, backlog);
}

/*! \brief get a new connection */
TCPSocket TCPSocket::Accept(void) {
    SOCKET newfd = accept(sockfd, NULL, NULL);
    if (newfd == INVALID_SOCKET) {
        Socket::Error("Accept");
    }
    return TCPSocket(newfd);
}
/*!
 * \brief decide whether the socket is at OOB mark
 * \return 1 if at mark, 0 if not, -1 if an error occured
 */
int TCPSocket::AtMark(void) const {
#ifdef _WIN32
    unsigned long atmark;  // NOLINT(*)
if (ioctlsocket(sockfd, SIOCATMARK, &atmark) != NO_ERROR) return -1;
#else
    int atmark;
    if (ioctl(sockfd, SIOCATMARK, &atmark) == -1) return -1;
#endif  // _WIN32
    return static_cast<int>(atmark);
}
/*!
 * \brief connect to an address
 * \param addr the address to connect to
 * \return whether connect is successful
 */
bool TCPSocket::Connect(const SockAddr &addr) {
    return connect(sockfd, reinterpret_cast<const sockaddr*>(&addr.addr),
                   sizeof(addr.addr)) == 0;
}
/*!
 * \brief send data using the socket
 * \param buf the pointer to the buffer
 * \param len the size of the buffer
 * \param flags extra flags
 * \return size of data actually sent
 *         return -1 if error occurs
 */
ssize_t TCPSocket::Send(const void *buf_, size_t len, int flag) {
    const char *buf = reinterpret_cast<const char*>(buf_);
    return send(sockfd, buf, static_cast<sock_size_t>(len), flag);
}
/*!
 * \brief receive data using the socket
 * \param buf_ the pointer to the buffer
 * \param len the size of the buffer
 * \param flags extra flags
 * \return size of data actually received
 *         return -1 if error occurs
 */
ssize_t TCPSocket::Recv(void *buf_, size_t len, int flags) {
    char *buf = reinterpret_cast<char*>(buf_);
    return recv(sockfd, buf, static_cast<sock_size_t>(len), flags);
}
/*!
 * \brief peform block write that will attempt to send all data out
 *    can still return smaller than request when error occurs
 * \param buf the pointer to the buffer
 * \param len the size of the buffer
 * \return size of data actually sent
 */
size_t TCPSocket::SendAll(const void *buf_, size_t len) {
    const char *buf = reinterpret_cast<const char*>(buf_);
    size_t ndone = 0;
    while (ndone <  len) {
        ssize_t ret = send(sockfd, buf, static_cast<ssize_t>(len - ndone), 0);
        if (ret == -1) {
            if (LastErrorWouldBlock()) return ndone;
            Socket::Error("SendAll");
        }
        buf += ret;
        ndone += ret;
    }
    return ndone;
}
/*!
 * \brief peforma block read that will attempt to read all data
 *    can still return smaller than request when error occurs
 * \param buf_ the buffer pointer
 * \param len length of data to recv
 * \return size of data actually sent
 */
size_t TCPSocket::RecvAll(void *buf_, size_t len) {
    char *buf = reinterpret_cast<char*>(buf_);
    size_t ndone = 0;
    while (ndone <  len) {
        ssize_t ret = recv(sockfd, buf,
                           static_cast<sock_size_t>(len - ndone), MSG_WAITALL);
        if (ret == -1) {
            if (LastErrorWouldBlock()) return ndone;
            Socket::Error("RecvAll");
        }
        if (ret == 0) return ndone;
        buf += ret;
        ndone += ret;
    }
    return ndone;
}
/*!
 * \brief send a string over network
 * \param str the string to be sent
 */
void TCPSocket::SendStr(const std::string &str) {
    int len = static_cast<int>(str.length());
    utils::Assert(this->SendAll(&len, sizeof(len)) == sizeof(len),
                  "error during send SendStr");
    if (len != 0) {
        utils::Assert(this->SendAll(str.c_str(), str.length()) == str.length(),
                      "error during send SendStr");
    }
}
/*!
 * \brief recv a string from network
 * \param out_str the string to receive
 */
void TCPSocket::RecvStr(std::string *out_str) {
    int len;
    utils::Assert(this->RecvAll(&len, sizeof(len)) == sizeof(len),
                  "error during send RecvStr");
    out_str->resize(len);
    if (len != 0) {
        utils::Assert(this->RecvAll(&(*out_str)[0], len) == out_str->length(),
                      "error during send SendStr");
    }
}
}  // namespace utils
}  // namespace rabit
