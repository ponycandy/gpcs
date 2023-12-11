#pragma once
#include <iostream>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/asio.hpp>
#include <queue>
#include <mutex>
#include <condition_variable>
#include "gpcsnode.h"
namespace gpcs
{
	class session
	{
    public:
        session(boost::asio::io_context& io_context)
            : socket_(io_context)
        {
        }

        boost::asio::ip::tcp::socket& socket()
        {
            return socket_;
        }

        void start();
        int execute(std::string cmd);
        //void initialize(gpcsserver* server,int order);
        void handle_read(const boost::system::error_code& error,
            std::size_t bytes_transferred);
        void handle_write(const boost::system::error_code& error, std::size_t bytes_transferred);
        
        std::queue<std::string> dataQueue;
        bool IsSubscriber=false;
        bool IsPublisher = false;
        bool Isvalid = true;
        int capacity_ = 10;
        std::mutex queueMutex;
        std::condition_variable queueCondition;
        std::string related_topicname;
    private:
        boost::asio::ip::tcp::socket socket_;
        std::string nodename;
        
        int ID;
        
        char Databuffer_charvec[USE_MAX_DATA_LENGTH];
	};
}

