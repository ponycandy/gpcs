#include "gpcsserver.h"
#include "session.h"
#include "tasktuple.h"
#include <boost/thread/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

gpcs::gpcsserver::gpcsserver(int portnum) :io_service_(),
acceptor_(io_service_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), portnum))
{
	coreportnum = portnum;
	prime_portnum = coreportnum + 1;
	start_accept();//监听连入的node
}

void gpcs::gpcsserver::readcoreconfig()
{


	return;
}

void gpcs::gpcsserver::setupserver()
{

	//  acceptor_(io_service, tcp::endpoint(tcp::v4(), port))
}

void gpcs::gpcsserver::start_accept()
{
	session* new_session = new session(io_service_);
	//下面这个是启动监听
	acceptor_.async_accept(new_session->socket(),
		boost::bind(&gpcs::gpcsserver::handle_accept, this, new_session,
			boost::asio::placeholders::error));
}

void gpcs::gpcsserver::handle_accept(session* new_session, const boost::system::error_code& error)
{
	if (!error)
	{
		std::cout << "new socket in!" << std::endl;
		new_session->initialize(this, nodegroup.size());
		nodegroup.push_back(new_session);//将该会话的指针存入到一个管理组里面
		//新连接接入后，启动该连接，然后建立新的session
		new_session->start();
		//new session没有被塞入到数组里面，这里下面new一个是不会破坏掉已有的sessio的
		new_session = new session(io_service_);
		//下面这个是启动新的监听，和上面那个完全一致
		acceptor_.async_accept(new_session->socket(),
			boost::bind(&gpcs::gpcsserver::handle_accept, this, new_session,
				boost::asio::placeholders::error));
	}
	else
	{
		delete new_session;
	}
}

void gpcs::gpcsserver::run()
{
	io_service_.run();
}

void gpcs::gpcsserver::registernode(std::string nodename, int ID, session* secinput)
{
	//这个就不上锁了，一般来说都不会有两个节点同时干这事儿的
	if (nodenamelist.count(nodename) > 0) //有相同名字的节点连入
	{
		int Cnt = nodename_2_count_list[nodename];
		Cnt++;
		nodename_2_count_list.erase(nodename);
		nodename_2_count_list.insert(std::make_pair(nodename, Cnt));
		nodename = nodename + "(" + std::to_string(Cnt) + ")";
		nodenamelist.insert(std::make_pair(nodename, ID));
		nodenamelist_inverse.insert(std::make_pair(ID, nodename));
	}
	else
	{
		nodenamelist.insert(std::make_pair(nodename, ID));
		nodenamelist_inverse.insert(std::make_pair(ID, nodename));
		nodename_2_count_list.insert(std::make_pair(nodename, 1));
	}





}

void gpcs::gpcsserver::subscribe(std::string topicname, int ID, session* secinput)
{
	TaskTuple* m_tuple = new TaskTuple;
	m_tuple->nodename = nodenamelist_inverse[ID];
	m_tuple->is_subscriber = true;
	m_tuple->is_publisher = false;
	if (subscribermap.count(topicname) > 0) //已经存在该主题的订阅者，将所有同一主题的订阅者放在同一个结构体FuncDummy下面
	{
		FuncDummy* m_dum = subscribermap[topicname];
		m_dum->num++;//FuncDummy计数器，计算相同主题订阅者数目
		m_dum->tuples.insert(std::make_pair(m_dum->num, m_tuple));
	}
	else
	{//不存在该主题，插入新的funcdummy
		FuncDummy* m_dum = new FuncDummy;
		m_dum->num = 1;
		m_dum->tuples.insert(std::make_pair(m_dum->num, m_tuple));
		subscribermap.insert(std::make_pair(topicname, m_dum));
	}
	tryconnection_publisher(topicname, ID);
}

void gpcs::gpcsserver::publish(std::string topicname, int ID, session* secinput)
{
	TaskTuple* m_tuple = new TaskTuple;
	m_tuple->nodename = nodenamelist_inverse[ID];
	m_tuple->is_subscriber = false;
	m_tuple->is_publisher = true;

	if (publishermap.count(topicname) > 0) //这不会发生，新的节点进来一定是下面的走法
	{
		//除非两个不同的节点发布同一个话题
		//或者同一个节点加后缀后先挂掉一次再订阅
		publishermap.erase(topicname);//移除旧的节点
		//
	}
	else
	{
		
		//wrong! m_dum is destroyed!!
	}
	FuncDummy* m_dum = new FuncDummy;
	m_dum->num = 1;
	m_dum->tuples.insert(std::make_pair(m_dum->num, m_tuple));
	publishermap.insert(std::make_pair(topicname, m_dum));

	temp_publisher_name = m_tuple->nodename;
	tryconnection_subscriber(topicname, ID);

}

int gpcs::gpcsserver::tryconnection_publisher(std::string topicname, int ID)
{

	if (publishermap.count(topicname) > 0)
	{
		//一般来说，只有一个publisher，所以连接一次就行
		FuncDummy* sig_dummy = publishermap[topicname];
		//接下来将sig_dummy对应的nodename和ID连接起来，这部分先留后面了，写一个通用接口
		std::string publishername = sig_dummy->tuples[1]->nodename;
		std::string subscribername = nodenamelist_inverse[ID];
		//先测试一下publisher是不是还挂着，不然subscriber连不上publisher的时候会导致
//execute会自动检查有效性，不用额外的东西
		connect(publishername, subscribername, topicname);
		//注意这里的先后顺序，先设置等待，再发送信号
		//每次连接都要等待完成信号
		return 1;//表示无需重回状态
	}
	else
	{
		//将请求的session设置回监听状态
		return 0;//
	}
}

int gpcs::gpcsserver::tryconnection_subscriber(std::string topicname, int ID)
{
	if (subscribermap.count(topicname) > 0)
	{
		FuncDummy* slot_dummy = subscribermap[topicname];
		std::string publishername = nodenamelist_inverse[ID];
		std::string subscribername;

		for (auto it = slot_dummy->tuples.begin(); it != slot_dummy->tuples.end(); ++it)
		{
			subscribername = it->second->nodename;
			if (nodegroup[nodenamelist[subscribername]]->isvalid() == 1)
			{
				connect(publishername, subscribername, topicname);
			}
			else
			{
				//从所有的map中移除这个节点,也可以不移除，每次重新检查有效性就行
				
			}
			
		}
		return 1;
	}
	else
	{
		return 0;
	}
}

void gpcs::gpcsserver::connect(std::string publisher, std::string subscriber, std::string topicname)
{
	//接下来就是协调器了，需要协调发送端和接收端的端口号以及CS分配
	//端口号从配置的初始端口号开始算起，向后+1，地址均为127.0.0.1
	//暂时只考虑这个简单版本了
	int port2use = prime_portnum;
	std::string pub_command;
	std::string sub_command;
	//为了均衡使用一个简单的算法，如果port是偶数，就将接收端设置为server否则就将pub端设置为server
	if (port2use % 2 == 0)
	{
		pub_command = "server  " + std::to_string(port2use) + " publisher " + topicname + " \n";
		sub_command = "client  " + std::to_string(port2use) + " subscriber " + topicname + " \n";
	}
	else
	{
		pub_command = "client  " + std::to_string(port2use) + " publisher " + topicname + " \n";
		sub_command = "server  " + std::to_string(port2use) + " subscriber " + topicname + " \n";
	}
	//接下来发送指令
	nodegroup[nodenamelist[publisher]]->execute(pub_command);
	nodegroup[nodenamelist[subscriber]]->execute(sub_command);
	std::cout << publisher << " and " << subscriber << " command sent!" << std::endl;
	//阻塞性等待时间
	boost::this_thread::sleep(boost::posix_time::seconds(3));
	prime_portnum++;
}

