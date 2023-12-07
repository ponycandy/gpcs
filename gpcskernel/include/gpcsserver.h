#pragma once

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include "session.h"
#include <map>
#include "funcdummy.h"
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
namespace gpcs
{
	class session;
	class gpcsserver
	{
	public:
		gpcsserver(int portnum);
		void readcoreconfig();
		void setupserver();
		void start_accept();
		void handle_accept(session* new_session, const boost::system::error_code& error);
		void run();
		void registernode(std::string nodename, int ID, session* secinput);
		void subscribe(std::string topicname, int ID,session* secinput);
		void publish(std::string topicname, int ID, session* secinput);
		int tryconnection_publisher(std::string topicname, int ID);
		int tryconnection_subscriber(std::string topicname, int ID);
		int tryreconnection_subscriber(std::string topicname, int ID);
		void connect(std::string publisher, std::string subscriber,std::string topicname);
		int prime_portnum;
		int temp_connection;
		int temp_connection_maxsize;
	private:
		int coreportnum;
		
		int nodenum;

		bool socketsignal = false;
		boost::mutex socketmutex;
		boost::condition_variable socketcondition;

		boost::asio::io_service io_service_;
		boost::asio::ip::tcp::acceptor acceptor_;
		std::vector<session*> nodegroup;//��node��ŵ�nodesocket��ӳ��
		std::map<std::string,int> nodenamelist;//��nodename��node��ŵ�ӳ��
		std::map<std::string, int> nodename_2_count_list;//��nodename��nodename�Ѿ�ע�������ӳ��
		std::map<std::string, int> topicname_2_count_list;//����publisher�����ߴ���
		std::map<int,std::string> nodenamelist_inverse;//��node��ŵ�nodename��ӳ��
		std::map<std::string, FuncDummy*> subscribermap;
		//��һ�����룬�������֣��ڶ������룬���Ļ���Ľڵ�wrapper
		//��һ�����룬�������֣��ڶ������룬��������Ľڵ�wrapper
		std::map<std::string, FuncDummy*> publishermap;
		std::string temp_subscriber_name;
		std::string temp_publisher_name;
		std::string temp_topic_name;
	};
}

