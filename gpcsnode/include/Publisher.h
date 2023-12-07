#pragma once
#include"gpcsnode.h"
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/string.hpp>

namespace gpcs
{
	class session;
	class Publisher
	{
	public:
		boost::mutex mtx;

		Publisher() :mtx()
		{
		};
		void lock()
		{
			mtx.lock();
		}
		void unlock()
		{
			mtx.unlock();
		}

		template <typename T>
		void publish(T& input);
		std::vector<session*> session_group;

	};

	template<typename T>
	inline void Publisher::publish(T& input)
	{
		lock();
		std::ostringstream archive_stream;
		boost::archive::text_oarchive archive(archive_stream);
		archive << input;
		std::string serialized_data = archive_stream.str();
		for (auto it : this->session_group)//if empty,nothing will be done
		{
	//		if (!it->socket().is_open()) 
	//		{
	///*			std::cerr <<"1 connection down" << std::endl;
	//			it = this->session_group.erase(*it);*/
	//			//移除一个会话
	//		}
	//		else
	//		{
			if(it->Isvalid==true)
			{ 
				it->execute(serialized_data);
			}
		}
		unlock();
	}
}

