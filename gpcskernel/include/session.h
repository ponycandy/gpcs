#pragma once
#include <iostream>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include "gpcsserver.h"
namespace gpcs
{
    class gpcsserver;
	class session
	{
    public:
        session(boost::asio::io_service& io_service)
            : socket_(io_service)
        {
        }

        boost::asio::ip::tcp::socket& socket()
        {
            return socket_;
        }

        void start();
        void execute(std::string cmd);
        int isvalid();
        void initialize(gpcsserver* server,int order);

        void handle_read(const boost::system::error_code& error,
            std::size_t bytes_transferred);

        void set_listen();//ÉèÖÃÎªÒ»°ã¼àÌý×´Ì¬
        void parse_command(std::string cmd);

    
        enum { max_length = 1024 };
        char data_[max_length];
    private:
        boost::asio::ip::tcp::socket socket_;
        
        
        std::string nodename;
        int ID;
        gpcsserver* server_;
	};
}

