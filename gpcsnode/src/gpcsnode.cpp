#include "gpcsnode.h"
#include <iostream>
#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

gpcs::gpcsnode::gpcsnode() :io_context_()
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
		return ;
	}
	try {
		coreURL = pt.get<int>("parameters.coreportnum.value");
		USE_MAX_DATA_LENGTH= pt.get<int>("parameters.databufferlength.value");
	}
	catch (boost::property_tree::ptree_bad_path& e) {
		std::cerr << "Invalid XML format: " << e.what() << std::endl;
		return ;
	}
}

void gpcs::gpcsnode::init(std::string nodename)
{

	//先尝试连接到master
	Core_socket = new boost::asio::ip::tcp::socket(io_context_);
	boost::asio::ip::tcp::resolver resolver(io_context_);
	boost::asio::ip::tcp::resolver::results_type endpoints = resolver.resolve("localhost", std::to_string(coreURL));

	// Connect to the server
	boost::asio::connect(*Core_socket, endpoints);
	//registername
	std::string cmd = "register " + nodename +" \n ";
	Core_socket->write_some(boost::asio::buffer(cmd));
	StartRead();
	boost::this_thread::sleep(boost::posix_time::seconds(2));
	//建立线程
	boost::thread Iothread(boost::bind(&gpcs::gpcsnode::runIoContext, this));

	//先这样，不知道行不行
	return;
}

void gpcs::gpcsnode::StartRead()
{
	Core_socket->async_read_some(boost::asio::buffer(data_, max_length),
		boost::bind(&gpcs::gpcsnode::handleMasterRead, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
}



void gpcs::gpcsnode::execute(std::string cmd)
{
	Core_socket->write_some(boost::asio::buffer(cmd));
}

void gpcs::gpcsnode::callCallback(const std::string& topicname, std::string input)
{
	if (CallbackMap.find(topicname) != CallbackMap.end())
	{
		CallbackMap[topicname](input);
	}
	else
	{
		std::cout << "Callback for topicname " << topicname << " not found." << std::endl;
	}
}

void gpcs::gpcsnode::runIoContext()
{
	io_context_.run();
}

void gpcs::gpcsnode::spinonce()
{
	//遍历所有的订阅型topicname，然后，查询是否可以激活的信号量，然后，这里返回类型
	bool sigstart;
	session_group_mutex.lock();
	for (auto it : sessiongroup)
	{
		if (it->IsSubscriber = true)
		{
			sigstart = true;
		}
		else
		{
			sigstart = false;
		}
		session* callbacksession = it;
		//这里其实是不需要sigstart的，可以直接使用下面的无锁队列判定
		if (sigstart)
		{
			callbacksession->queueMutex.lock();
			
			if (callbacksession->dataQueue.empty())
			{
				//空队列，啥都不执行
			}
			else 
			{
				std::string topicName= callbacksession->related_topicname;
				std::string somedatavalue = callbacksession->dataQueue.front();
				callbacksession->dataQueue.pop();
				callCallback(topicName, somedatavalue);
			}
			// Process the data,回调启动
			callbacksession->queueMutex.unlock();
		}
	}
	session_group_mutex.unlock();
}

void gpcs::gpcsnode::spinonce(int ms)
{
	spinonce();
	boost::this_thread::sleep(boost::posix_time::milliseconds(ms));
}

void gpcs::gpcsnode::parse_command(std::string cmd)
{
	//首先按照空格分割指令，空格数目是可以任意的
	std::vector<std::string> cmdtokens;
	std::istringstream iss(cmd);
	std::string token;

	// Use std::istringstream to split the command
	while (iss >> token) {
		cmdtokens.push_back(token);
	}
	//按照指令执行
	if (cmdtokens[0] == "server" || cmdtokens[0] == "client")
	{
		std::string portnum = cmdtokens[1];
		std::string charecter = cmdtokens[2];
		std::string topicname = cmdtokens[3];
		session* new_session = new session(io_context_);
		//启动监听或者连接
		if (cmdtokens[0] == "server")
		{
			boost::asio::ip::tcp::acceptor acceptor_(io_context_,
				boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), stoi(portnum)));
			acceptor_.accept(new_session->socket());
			new_session->start();
		}
		else
		{
			boost::asio::ip::tcp::resolver resolver(io_context_);
			boost::asio::ip::tcp::resolver::results_type endpoints = resolver.resolve("localhost",
				portnum);
			// Connect to the server
			boost::asio::connect(new_session->socket(), endpoints);
			new_session->start();
		}
		//这里，强制为同步执行
		new_session->related_topicname = topicname;
		if (charecter == "publisher")
		{
			new_session->IsSubscriber = false;
			new_session->IsPublisher = true;
			//要做检查，如果publisher的topicname与已有的session的话题名称不一样，就要创造一个新的
			//publisher，否则要将新的session补到已有的publisher里面
				Publisher* pub = topicname_2_Publisher_map[topicname];
				pub->lock();
				pub->session_group.push_back(new_session);
				pub->unlock();
		}
		else {
			//判定一下之前是不是已经注册过相同的话题
			//如果是的话，可以判断是重新订阅，那么就要做稍微不同的事情
			if (topicname_2_sessionindex_map.count(topicname) > 0)
			{
				for (auto it = topicname_2_sessionindex_map.begin(); it != topicname_2_sessionindex_map.end(); ++it)
				{
					sessiongroup[it->second-1]->IsPublisher= false;
					sessiongroup[it->second-1]->IsSubscriber = false;
				}
				new_session->IsSubscriber = true;
				new_session->IsPublisher = false;
			}
			else//第一次订阅
			{
				new_session->IsSubscriber = true;
				new_session->IsPublisher = false;
			}


		}
		session_group_mutex.lock();
		sessiongroup.push_back(new_session);
		session_group_mutex.unlock();
		//不是newsession执行，是主session执行！
		int index = sessiongroup.size();
		sessionindex_2_topicname_map.insert(std::make_pair(index, topicname));
		topicname_2_sessionindex_map.insert(std::make_pair(topicname, index));

		//session名称和session的序号建立连接
	 /*   boost::unique_lock<boost::mutex> lock(socketmutex);
		socketsignal = true;
		socketcondition.notify_one();*/
		//完成连接建立之后，通知调用的主进程继续，这个也不需要了
		return;
	}
}


void gpcs::gpcsnode::handleMasterRead(const boost::system::error_code& error, std::size_t bytes_transferred)
{
	if (!error)
	{
		std::string command(data_);
		memset(data_, 0x00, max_length);//清空data_
		parse_command(command);
		//在这里处理所有的data_，需要确定所有的可能接口了
		// 按照自然语法,使用parse string，将输入和输出分开来
		//重新进入异步读状态
		Core_socket->async_read_some(boost::asio::buffer(data_, max_length),
			boost::bind(&gpcs::gpcsnode::handleMasterRead, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}
	else {
		// Handle error
	}
}

gpcs::Publisher* gpcs::gpcsnode::advertise(std::string topicname)
{
	std::string cmd = "publish " + topicname+" \n";//所有指令都需要回车键
	publish_advertise_signal = false;

	Publisher* puber = new Publisher;
	topicname_2_Publisher_map.insert(std::make_pair(topicname, puber));
	//现在publisher里面的会话是空的
	execute(cmd);
	boost::this_thread::sleep(boost::posix_time::seconds(2));
	//必须再执行完成后等待一段时间，这是最简单的同步措施
	//无锁等待新的session加入
	return puber;
}

gpcs::Subscriber* gpcs::gpcsnode::subscribe(std::string topicname, std::function<void(std::string)> callback)
{
	//首先存储回调函数：
	CallbackMap[topicname] = [callback](std::string input)
	{
		callback(input);
	};
	//然后请求连接
	std::string cmd = "subscribe " + topicname+" \n";
	execute(cmd);
	boost::this_thread::sleep(boost::posix_time::seconds(2));
	//必须再执行完成后等待一段时间，这是最简单的同步措施
	//返回，回调会在连接建立后自动激活
	//只有在spin的时候，才会执行回调

	return nullptr;
}

