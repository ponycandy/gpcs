#include "session.h"

void gpcs::session::start()
{
    //�����첽��ȡ״̬,�ص�Ϊhandle_read
    //���������ÿ�η���һ�����ݾͻ᷵��
    set_listen();
    //���������ֻ��buffer��ȫ�����Ż᷵��
    //boost::asio::async_read(socket_, boost::asio::buffer(data_, max_length),
    //    boost::bind(&session::handle_read, this,
    //        boost::asio::placeholders::error,
    //        boost::asio::placeholders::bytes_transferred));
}

void gpcs::session::execute(std::string cmd)
{
    int Isvalid = 1;
    try
    {
        socket_.write_some(boost::asio::buffer(boost::asio::buffer(cmd)));
        Isvalid = 1;
    }
    catch (const boost::system::system_error& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        Isvalid = 0;
    }
    return ;
}

int gpcs::session::isvalid()
{
    std::string cmd = "check_validity";
    int Isvalid = 1;
    try
    {
        socket_.write_some(boost::asio::buffer(boost::asio::buffer(cmd)));
        Isvalid = 1;
    }
    catch (const boost::system::system_error& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        Isvalid = 0;
    }
    boost::this_thread::sleep(boost::posix_time::milliseconds(10));
    return Isvalid;
}

void gpcs::session::initialize(gpcsserver* server, int order)
{
    ID = order;
    server_ = server;
}


void gpcs::session::handle_read(const boost::system::error_code& error, std::size_t bytes_transferred)
{
    if (!error)
    {
        std::string command(data_);
        memset(data_, 0x00, max_length);//���data_
        
        //�����ﴦ�����е�data_����Ҫȷ�����еĿ��ܽӿ���
       // ������Ȼ�﷨,ʹ��parse string�������������ֿ���
        parse_command(command);
        //���½����첽��״̬
        set_listen();
        //���е��첽�ض�״̬��parse_command����
    }
}



void gpcs::session::set_listen()
{
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
        boost::bind(&session::handle_read, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
}

void gpcs::session::parse_command(std::string cmd)
{
    //���Ȱ��տո�ָ�ָ��ո���Ŀ�ǿ��������
    std::vector<std::string> cmdtokens;
    std::istringstream iss(cmd);
    std::string token;

    // Use std::istringstream to split the command
    while (iss >> token) {
        cmdtokens.push_back(token);
    }
    //����ָ��ִ��
    if (cmdtokens[0] == "subscribe")
    {
        std::string topicname = cmdtokens[1];
        server_->subscribe(topicname, ID,this);
        return;
    }
    if (cmdtokens[0] == "publish")
    {
        std::string topicname = cmdtokens[1];
        server_->publish(topicname, ID, this);
        return;
    }
    if (cmdtokens[0] == "register")
    {
        nodename = cmdtokens[1];
        //����״̬���뵽server��
        server_->registernode(nodename,ID, this);
        return;
    }
}
