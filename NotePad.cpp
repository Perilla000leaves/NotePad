// NotePad.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "NotePad.h"
using std::list;
list<TCHAR> ch;
list<TCHAR>::iterator it;


#define MAX_LOADSTRING 100

// 全局变量: 
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名
HANDLE hReadFile;								// 读文件的句柄
LPVOID buffer[65535] = {};						// 放置读取文件的内容的缓存
WIN32_FIND_DATA FindData;						// 全局文件属性结构体

// 此代码模块中包含的函数的前向声明: 

ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	SaveAs(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	Open(HWND, UINT, WPARAM, LPARAM);
void                save(HWND&, int, BOOL);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO:  在此放置代码。
	MSG msg;
	HACCEL hAccelTable;
	it = ch.begin();
	if (NULL != lpCmdLine)
	{
		hReadFile = CreateFile(lpCmdLine, GENERIC_ALL, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		DWORD FileSize = GetFileSize(hReadFile, NULL);
		BOOL flag = ReadFile(hReadFile, buffer, FileSize, &FileSize, NULL);
		CloseHandle(hReadFile);					//要养成用完关闭句柄的好习惯
		HANDLE hFind = FindFirstFile(lpCmdLine, &FindData);
		MessageBox(0, FindData.cFileName, 0, 0);//获得当前文件名
	}

	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_NOTEPAD, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化: 
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_NOTEPAD));

	// 主消息循环: 
	while (GetMessage(&msg, NULL, 0, 0))
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
//  函数:  MyRegisterClass()
//
//  目的:  注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_NOTEPAD));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_NOTEPAD);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   函数:  InitInstance(HINSTANCE, int)
//
//   目的:  保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // 将实例句柄存储在全局变量中

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  函数:  WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
//
RECT rect;
TCHAR szFile[65535] = {};
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_CREATE:
		GetClientRect(hWnd, &rect);
		CreateWindow(_T("Edit"), NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE, 0, 0, rect.right, rect.bottom, hWnd, HMENU(1004), hInst, NULL);
		if (buffer)
			SetDlgItemText(hWnd, IDD_MAINEDIT, (LPCSTR)buffer);
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 分析菜单选择: 
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_NEW:
			GetDlgItemText(hWnd, IDD_MAINEDIT, szFile, 65535);
			save(hWnd, IDD_MAINEDIT, FALSE);//用TRUE和FALSE表示是否需要输入文件名
			SetDlgItemText(hWnd, IDD_MAINEDIT, NULL);
			break;
		case IDM_OPEN:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_SAVEBOX), hWnd, Open);
			break;
		case IDM_SAVE:
			GetDlgItemText(hWnd, IDD_MAINEDIT, szFile, 65535);
			save(hWnd, IDD_MAINEDIT, FALSE);//用TRUE和FALSE表示是否需要输入文件名
			break;
		case IDM_SAVEAS:
			GetDlgItemText(hWnd, IDD_MAINEDIT, szFile, 65535);//在对话框跳出之前先保存记事本中的内容，这个函数得到的字符串是宽字节的，需要转换为多字节的
			DialogBox(hInst, MAKEINTRESOURCE(IDD_SAVEBOX), hWnd, SaveAs);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO:  在此添加任意绘图代码...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		break;
	case WM_CHAR:
		break;
	case WM_RBUTTONDOWN:
	{
		POINT pt;
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);
		ClientToScreen(hWnd, &pt);
		HMENU rButtonMenu = LoadMenu(hInst, MAKEINTRESOURCE(IDR_MENU2));//load意为从资源中加载
		rButtonMenu = GetSubMenu(rButtonMenu, 0);
		TrackPopupMenu(rButtonMenu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD, pt.x, pt.y, 0, hWnd, NULL);
		break;
	}
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

// “保存”框的消息处理程序。
INT_PTR CALLBACK SaveAs(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)//点击确定保存
			{
				save(hDlg, IDC_EDIT1, TRUE);
				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}
	}
	return (INT_PTR)FALSE;
}
// “打开”框的消息处理程序。
INT_PTR CALLBACK Open(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)//点击确定打开
		{
			char bufferTemp[100];
			GetDlgItemText(hDlg, IDC_EDIT1, bufferTemp, 100);
			HANDLE hFile = CreateFile(bufferTemp, GENERIC_ALL, FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			DWORD FileSize = GetFileSize(hFile, NULL);
			ReadFile(hFile, buffer, FileSize, &FileSize, NULL);
			HWND hParent = GetParent(hDlg);
			SendMessage(GetDlgItem(hParent, IDD_MAINEDIT), WM_CREATE, 0, 0);
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
	}
	return (INT_PTR)FALSE;
}
void save(HWND &hWnd, int nIDItem, BOOL flag)
{
	if (!strcmp(szFile, (const char *)buffer))//如果文本并未改动，可以省去文本写入操作，为系统节省资源
	{
		MessageBox(hWnd, _T("文本并未改动"), 0, 0);
		return;
	}
	else
	{
		if (!flag)//如果不用输入文件名
		{
			MessageBox(hWnd, _T("文本有改动！"), 0, 0);
			HANDLE hFile = CreateFile(FindData.cFileName,
				GENERIC_ALL,//访问权限
				FILE_SHARE_WRITE,//共享方式
				0,//安全属性
				CREATE_ALWAYS,//创建方式
				FILE_ATTRIBUTE_NORMAL,//文件属性
				NULL);//文件模板句柄
			BOOL flag = WriteFile(hFile, szFile, strlen(szFile), 0, 0);//将记事本中的内容写入文件
			CloseHandle(hFile);
		}
		else
		{
			TCHAR szTitleName[128] = {}; //用来做缓存的字符串
			GetDlgItemText(hWnd, nIDItem, szTitleName, 128);
			strcat(szTitleName, ".txt");
			HANDLE hFile = CreateFile(szTitleName,
				GENERIC_ALL,//访问权限
				FILE_SHARE_WRITE,//共享方式
				0,//安全属性
				CREATE_ALWAYS,//创建方式
				FILE_ATTRIBUTE_NORMAL,//文件属性
				NULL);//文件模板句柄
			BOOL flag = WriteFile(hFile, szFile, strlen(szFile), 0, 0);//将记事本中的内容写入文件
			CloseHandle(hFile);
		}
	}
}

