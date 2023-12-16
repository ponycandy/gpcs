#include "session.h"

void gpcs::session::start()
{
	//启动异步读取状态,回调为handle_read
	//下面这个，每次发送一条数据就会返回
	socket_.async_read_some(boost::asio::buffer(Databuffer_charvec), boost::bind(&session::handle_read, this,
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred));
}

int gpcs::session::execute(std::string cmd)
{
	int byte = 0;
	try 
	{
		socket_.write_some(boost::asio::buffer(boost::asio::buffer(cmd)));
		byte = 0;

	}
	catch (const boost::system::system_error& e) 
	{
		std::cerr << "Error: " << e.what() << std::endl;
		byte = 1;
		Isvalid = false;
	}
	return byte;
}


void gpcs::session::handle_read(const boost::system::error_code& error, std::size_t bytes_transferred)
{
	if (!error)
	{
		std::string str(Databuffer_charvec.begin(), Databuffer_charvec.end());// Convert to std::string
		//memset(Databuffer_charvec, 0x00, USE_MAX_DATA_LENGTH);//清空buffer
		Databuffer_charvec.clear();
		Databuffer_charvec.resize(USE_MAX_DATA_LENGTH);
		queueMutex.lock();
		// Push the received data into the lock-free queue
		if (dataQueue.size() >= capacity_)
		{
			dataQueue.pop();
			// Queue is full, handle the situation accordingly
			// You can throw an exception or wait until some elements are consumed.
		}
		dataQueue.push(str);
		queueMutex.unlock();

		socket_.async_read_some(boost::asio::buffer(Databuffer_charvec), boost::bind(&session::handle_read, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
	}
}

void gpcs::session::handle_write(const boost::system::error_code& error, std::size_t bytes_transferred)
{
	if (error)
	{
		if (error == boost::asio::error::connection_reset || error == boost::asio::error::connection_refused) 
		{
			// Handle the case where the other side of the connection is down
			std::cout << "The connection was reset or refused. The other side may be down." << std::endl;
			// Add your custom notification or error handling logic here
		}
		else {
			std::cout << "Error: " << error.message() << std::endl;
			// Handle other errors
		}
	}
}

