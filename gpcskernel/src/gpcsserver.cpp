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
	start_accept();//���������node
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
	//�����������������
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
		nodegroup.push_back(new_session);//���ûỰ��ָ����뵽һ������������
		//�����ӽ�������������ӣ�Ȼ�����µ�session
		new_session->start();
		//new sessionû�б����뵽�������棬��������newһ���ǲ����ƻ������е�sessio��
		new_session = new session(io_service_);
		//��������������µļ������������Ǹ���ȫһ��
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
	//����Ͳ������ˣ�һ����˵�������������ڵ�ͬʱ�����¶���
	if (nodenamelist.count(nodename) > 0) //����ͬ���ֵĽڵ�����
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
	if (subscribermap.count(topicname) > 0) //�Ѿ����ڸ�����Ķ����ߣ�������ͬһ����Ķ����߷���ͬһ���ṹ��FuncDummy����
	{
		FuncDummy* m_dum = subscribermap[topicname];
		m_dum->num++;//FuncDummy��������������ͬ���ⶩ������Ŀ
		m_dum->tuples.insert(std::make_pair(m_dum->num, m_tuple));
	}
	else
	{//�����ڸ����⣬�����µ�funcdummy
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

	if (publishermap.count(topicname) > 0) //�ⲻ�ᷢ�����µĽڵ����һ����������߷�
	{
		//����������ͬ�Ľڵ㷢��ͬһ������
		//����ͬһ���ڵ�Ӻ�׺���ȹҵ�һ���ٶ���
		publishermap.erase(topicname);//�Ƴ��ɵĽڵ�
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
		//һ����˵��ֻ��һ��publisher����������һ�ξ���
		FuncDummy* sig_dummy = publishermap[topicname];
		//��������sig_dummy��Ӧ��nodename��ID�����������ⲿ�����������ˣ�дһ��ͨ�ýӿ�
		std::string publishername = sig_dummy->tuples[1]->nodename;
		std::string subscribername = nodenamelist_inverse[ID];
		//�Ȳ���һ��publisher�ǲ��ǻ����ţ���Ȼsubscriber������publisher��ʱ��ᵼ��
//execute���Զ������Ч�ԣ����ö���Ķ���
		connect(publishername, subscribername, topicname);
		//ע��������Ⱥ�˳�������õȴ����ٷ����ź�
		//ÿ�����Ӷ�Ҫ�ȴ�����ź�
		return 1;//��ʾ�����ػ�״̬
	}
	else
	{
		//�������session���ûؼ���״̬
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
				//�����е�map���Ƴ�����ڵ�,Ҳ���Բ��Ƴ���ÿ�����¼����Ч�Ծ���
				
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
	//����������Э�����ˣ���ҪЭ�����Ͷ˺ͽ��ն˵Ķ˿ں��Լ�CS����
	//�˿ںŴ����õĳ�ʼ�˿ںſ�ʼ�������+1����ַ��Ϊ127.0.0.1
	//��ʱֻ��������򵥰汾��
	int port2use = prime_portnum;
	std::string pub_command;
	std::string sub_command;
	//Ϊ�˾���ʹ��һ���򵥵��㷨�����port��ż�����ͽ����ն�����Ϊserver����ͽ�pub������Ϊserver
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
	//����������ָ��
	nodegroup[nodenamelist[publisher]]->execute(pub_command);
	nodegroup[nodenamelist[subscriber]]->execute(sub_command);
	std::cout << publisher << " and " << subscriber << " command sent!" << std::endl;
	//�����Եȴ�ʱ��
	boost::this_thread::sleep(boost::posix_time::seconds(3));
	prime_portnum++;
}

