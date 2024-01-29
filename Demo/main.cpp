#include <iostream>

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


void certaincallback(const std::string& data)
{
    MyData recievedData = gpcs::struct_load<MyData>(data);
    std::cout << "reply data: id:" << recievedData.id << "  value:" << recievedData.value
        << "    name:" << recievedData.name << std::endl;
}

int main() {
    gpcs::gpcsnode nh;
    MyData testData;
    testData.id = 10;
    testData.value = 1.8;
    std::string message = "hello you!";
    strcpy(testData.name, message.c_str());
	std::cout << "before nh.init  " << std::endl;
    nh.init("sub_test1");
	std::cout << "before nh.subscribe  " << std::endl;
    nh.subscribe("sometopic", certaincallback);
    while (true)
    {

     //   nh.spinonce(300);
             nh.spinonce();
         //   // somepuber->publish<MyData>(testData);
         // //   somepuber->publish(testData);//不指明也可以的
       // somepuber->publish(testData);
   // std::cout << "Let's see what's going on" << std::endl;
    }
    std::cout << "end" << std::endl;

    return 0;
}
