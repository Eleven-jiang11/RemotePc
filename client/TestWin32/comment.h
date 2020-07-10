#pragma once


//cmd类型数据
#define PACKET_REQ_CMD  10001 //PACKET_REQ_CMD request cmd 表示cmd类型请求数据 客户端-->服务端
#define PACKET_RLY_CMD  10001 //PACKET_RLY_CMD reply cmd 表示cmd类型恢复数据 服务端-->客户端


//键盘类型数据
#define PACKET_REQ_KEYBOARD  20001 //PACKET_REQ_KEYBOARD request KEYBOARD 表示KEYBOARD类型请求数据 客户端-->服务端
#define PACKET_RLY_KEYBOARD  20001 //PACKET_RLY_KEYBOARD reply KEYBOARD 表示KEYBOARD	类型恢复数据 服务端-->客户端
//定义一些公共的结构体
#pragma pack(push)
#pragma pack(1)
struct MyPacket
{
	unsigned int type; //表示包类型
	unsigned int length; //表示包长度
	char data[1];  //定义一个1字节的数组
};
#pragma pack(pop)