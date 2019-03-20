/*!
 *  Copyright (c) 2014-2019 by Contributors
 * \file socket_test.cc
 * \brief this file provide socket basic op test coverage
 * \author Chen Qin
 */
#include <limits.h>
#include <pthread.h>
#include "../../src/socket.h"
#include "gtest/gtest.h"

using namespace std;

namespace rabit {
namespace utils {
    // Test binding to local port
    TEST(TCPSOCKET, BINDING_HOST){
        TCPSocket sock, sock2;
        ASSERT_NO_THROW(sock.Create());
        ASSERT_NO_THROW(sock2.Create());
        ASSERT_FALSE(sock.IsClosed());
        ASSERT_TRUE(sock.TryBindHost(8080, 8081));
        ASSERT_TRUE(sock2.TryBindHost(8082, 8083));
        ASSERT_NO_THROW(sock.Close());
        ASSERT_NO_THROW(sock2.Close());
        ASSERT_TRUE(sock.IsClosed());
        ASSERT_TRUE(sock2.IsClosed());
    }

    // Test pass command string two way from client to server
    TEST(TCPSOCKET, PASS_COMMAND_STR){
        TCPSocket serv_socket;
        int port;
        ASSERT_NO_THROW(serv_socket.Create());
        ASSERT_FALSE(serv_socket.IsClosed());

        auto x = [&]{
            port = serv_socket.TryBindHost(8080, 8081);
            return true;
        };
        ASSERT_TRUE(x());
        ASSERT_EQ(port, 8080);
        ASSERT_NO_THROW(serv_socket.Listen());
        ASSERT_NO_THROW(serv_socket.SetKeepAlive(true));

        int pid = fork();
        if ( pid == 0){
            TCPSocket acpt = serv_socket.Accept();
            auto y = [&]{
                std::string recv;
                acpt.RecvStr(&recv);
                ASSERT_EQ(recv, "ping");
                acpt.SendStr("pong");
                exit(0);
            };
            ASSERT_EXIT(y(), ::testing::ExitedWithCode(0), "");

            ASSERT_NO_THROW(acpt.Close());
            ASSERT_TRUE(acpt.IsClosed());
            ASSERT_NO_THROW(serv_socket.Close());
            ASSERT_TRUE(serv_socket.IsClosed());
            exit(0);
        }else{
            TCPSocket client_socket;
            ASSERT_NO_THROW(client_socket.Create());
            ASSERT_GT(client_socket.Connect(SockAddr("127.0.0.1", 8080)), -1);
            auto z = [&]{
                std::string client_recv;
                client_socket.SendStr("ping");
                client_socket.RecvStr(&client_recv);
                ASSERT_EQ(client_recv, "pong");
                exit(0);
            };
            ASSERT_EXIT(z(),::testing::ExitedWithCode(0), "");

            ASSERT_NO_THROW(client_socket.Close());
            ASSERT_TRUE(client_socket.IsClosed());
        }
    }

    // Test client server inital magic verification
    TEST(TCPSOCKET, PASS_MAGIC_INT){
        TCPSocket serv_socket;
        int port;
        int kMagic = 0xff99;

        ASSERT_NO_THROW(serv_socket.Create());
        ASSERT_FALSE(serv_socket.IsClosed());

        auto x1 = [&]{
            port = serv_socket.TryBindHost(8083, 8084);
            return true;
        };
        ASSERT_TRUE(x1());
        ASSERT_EQ(port, 8083);
        ASSERT_NO_THROW(serv_socket.Listen());
        ASSERT_NO_THROW(serv_socket.SetKeepAlive(true));

        int pid = fork();
        if ( pid == 0){
            TCPSocket acpt = serv_socket.Accept();
            auto y = [&]{
                int magic;
                acpt.RecvAll(&magic, sizeof(magic));
                ASSERT_EQ(magic, kMagic);
                acpt.SendStr("pong");
                exit(0);
            };
            ASSERT_EXIT(y(), ::testing::ExitedWithCode(0), "");

            ASSERT_NO_THROW(acpt.Close());
            ASSERT_TRUE(acpt.IsClosed());
            ASSERT_NO_THROW(serv_socket.Close());
            ASSERT_TRUE(serv_socket.IsClosed());
            exit(0);
        }else{
            TCPSocket client_socket;
            ASSERT_NO_THROW(client_socket.Create());
            ASSERT_GT(client_socket.Connect(SockAddr("127.0.0.1", 8083)), -1);
            auto z = [&]{
                std::string client_recv;
                client_socket.SendAll(&kMagic, sizeof(kMagic));
                client_socket.RecvStr(&client_recv);
                ASSERT_EQ(client_recv, "pong");
                exit(0);
            };
            ASSERT_EXIT(z(),::testing::ExitedWithCode(0), "");

            ASSERT_NO_THROW(client_socket.Close());
            ASSERT_TRUE(client_socket.IsClosed());
        }
    }
}}
