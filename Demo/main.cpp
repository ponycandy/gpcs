#include <iostream>
#include <thread>
#include <boost/asio.hpp>
#include "gpcsnode.h"



struct MyData {
    int id;
    double value;
    char name[20];

    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar& id;
        ar& value;
        ar& name;
    }
};
//首先定义传输的结构，务必定义序列化函数，
//当然，也可以自动化这项工作

void certaincallback(const std::string& data)
{
    MyData recievedData = gpcs::struct_load<MyData>(data);
    std::cout << "reply data: id:" << recievedData.id << "  value:" << recievedData.value
        << "    name:" << recievedData.name << std::endl;
}
//定义接受这项数据的回调函数，这是固定格式，反序列化只能在函数体里面完成了没办法
int main() {
    gpcs::gpcsnode nh;//创建句柄
    MyData testData;//创建示例数据
    testData.id = 10;
    testData.value = 1.8;
    std::string message = "hello you!";
    strcpy(testData.name, message.c_str());

    nh.init("sub_test1");//连接到kernel
    nh.subscribe("TestTopic", certaincallback);//注册回调函数
   // gpcs::Publisher* somepuber = nh.advertise("TestTopic");//注册发布者
    while (true)
    {

     //   nh.spinonce(300);
             nh.spinonce();//轮询回调函数,必须设置最小循环时间?
         //   // somepuber->publish<MyData>(testData);
         // //   somepuber->publish(testData);//不指明也可以的
       // somepuber->publish(testData);
    }
    std::cout << "end" << std::endl;

    return 0;
}