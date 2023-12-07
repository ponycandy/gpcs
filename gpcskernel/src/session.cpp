#include "session.h"

void gpcs::session::start()
{
    //启动异步读取状态,回调为handle_read
    //下面这个，每次发送一条数据就会返回
    set_listen();
    //下面这个，只有buffer完全填满才会返回
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
        memset(data_, 0x00, max_length);//清空data_
        
        //在这里处理所有的data_，需要确定所有的可能接口了
       // 按照自然语法,使用parse string，将输入和输出分开来
        parse_command(command);
        //重新进入异步读状态
        set_listen();
        //所有的异步重读状态由parse_command决定
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
    //首先按照空格分割指令，空格数目是可以任意的
    std::vector<std::string> cmdtokens;
    std::istringstream iss(cmd);
    std::string token;

    // Use std::istringstream to split the command
    while (iss >> token) {
        cmdtokens.push_back(token);
    }
    //按照指令执行
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
        //将本状态存入到server中
        server_->registernode(nodename,ID, this);
        return;
    }
}
