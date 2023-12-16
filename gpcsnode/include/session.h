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

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

namespace gpcs
{
	class session
	{
    public:
        session(boost::asio::io_context& io_context)
            : socket_(io_context)
        {
            char* configFile = std::getenv("GPCS_CONFIG_XML_PATH");
            if (configFile) {
                std::cout << "Config file path: " << configFile << std::endl;
                // Now you can use configFile to access the config.xml file
            }
            else {
                std::cerr << "Environment variable CONFIG_XML_PATH is not set" << std::endl;
            }

            boost::property_tree::ptree pt;
            try {
                boost::property_tree::read_xml(configFile, pt);
            }
            catch (boost::property_tree::xml_parser_error& e) {
                std::cerr << "Failed to load config.xml: " << e.what() << std::endl;
                return;
            }
            try {
                USE_MAX_DATA_LENGTH = pt.get<int>("parameters.databufferlength.value");
            }
            catch (boost::property_tree::ptree_bad_path& e) {
                std::cerr << "Invalid XML format: " << e.what() << std::endl;
                return;
            }
            Databuffer_charvec.resize(USE_MAX_DATA_LENGTH);
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
        int USE_MAX_DATA_LENGTH;
        std::mutex queueMutex;
        std::condition_variable queueCondition;
        std::string related_topicname;
    private:
        boost::asio::ip::tcp::socket socket_;
        std::string nodename;
        
        int ID;
        std::vector<char> Databuffer_charvec;
	};
}

