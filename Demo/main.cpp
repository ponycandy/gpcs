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
//���ȶ��崫��Ľṹ����ض������л�������
//��Ȼ��Ҳ�����Զ��������

void certaincallback(const std::string& data)
{
    MyData recievedData = gpcs::struct_load<MyData>(data);
    std::cout << "reply data: id:" << recievedData.id << "  value:" << recievedData.value
        << "    name:" << recievedData.name << std::endl;
}
//��������������ݵĻص����������ǹ̶���ʽ�������л�ֻ���ں��������������û�취
int main() {
    gpcs::gpcsnode nh;//�������
    MyData testData;//����ʾ������
    testData.id = 10;
    testData.value = 1.8;
    std::string message = "hello you!";
    strcpy(testData.name, message.c_str());

    nh.init("sub_test1");//���ӵ�kernel
    nh.subscribe("TestTopic", certaincallback);//ע��ص�����
   // gpcs::Publisher* somepuber = nh.advertise("TestTopic");//ע�ᷢ����
    while (true)
    {

     //   nh.spinonce(300);
             nh.spinonce();//��ѯ�ص�����,����������Сѭ��ʱ��?
         //   // somepuber->publish<MyData>(testData);
         // //   somepuber->publish(testData);//��ָ��Ҳ���Ե�
       // somepuber->publish(testData);
    }
    std::cout << "end" << std::endl;

    return 0;
}