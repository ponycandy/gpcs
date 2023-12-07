#pragma once
namespace gpcs
{
#pragma pack(push,1)
    struct command
    {
        short sequence; //两位长度，预留位置
        short Id;   //标明指令类型
        short length; //指示指令长度，即后面data包含的字节数目
        char* data; //指令的输入内容
    };
#pragma pack(pop)

}