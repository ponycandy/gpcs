#pragma once
namespace gpcs
{
#pragma pack(push,1)
    struct command
    {
        short sequence; //��λ���ȣ�Ԥ��λ��
        short Id;   //����ָ������
        short length; //ָʾָ��ȣ�������data�������ֽ���Ŀ
        char* data; //ָ�����������
    };
#pragma pack(pop)

}