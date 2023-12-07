#pragma once
#include "ALLconfig.h"
#include <boost/asio.hpp>
#include <iostream>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include "session.h"
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include "Publisher.h"

#include <boost/thread/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace gpcs
{
	class Publisher;
	class session;

	template<typename T>
	inline T struct_load(std::string load_string)
	{
		T item;
		std::istringstream is(load_string);
		boost::archive::text_iarchive archive(is);
		archive >> item;
		return item;
	}

	class Subscriber;
	//class Publisher;*/
	class gpcsnode
	{
	public:
		gpcsnode();
		void init(std::string nodename);
		void StartRead();
		void execute(std::string cmd);
		void callCallback(const std::string& topicname, std::string input);
		void runIoContext();
		void spinonce();
		void spinonce(int ms);
		void parse_command(std::string command);
		void handleMasterRead(const boost::system::error_code& error, std::size_t bytes_transferred);

		Publisher* advertise(std::string topicname);
		Subscriber* subscribe(std::string topicname, std::function<void(std::string)> callback);

		boost::asio::io_context io_context_;
		boost::asio::ip::tcp::socket *Core_socket;
		std::map<std::string, std::function<void(std::string)>> CallbackMap;

		boost::thread *ioThread;
		
		std::vector<session*> sessiongroup;
		std::map<int , std::string> sessionindex_2_topicname_map;
		std::map<std::string, int> topicname_2_sessionindex_map;
		std::map<std::string, Publisher* > topicname_2_Publisher_map;
		int coreURL;
		//下面两个是指令相关的长度，不是数据长度
		enum { max_length = 1024 };
		char data_[max_length];

		boost::mutex session_group_mutex;
		boost::mutex callback_mutex;
		boost::condition_variable publish_advertise_condition;
		bool publish_advertise_signal;
	};
}

