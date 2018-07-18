#include <tchar.h>
#include <Windows.h>
#include <string>
#include <cstring>
#include <sstream>
#include "Grapher.h"

using namespace std;

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 200
#define TEXTBOX_WIDTH 900
#define TEXTBOX_HEIGHT 25
#define MAX_LENGTH 512

HINSTANCE	hInst;									//主窗口句柄
TCHAR		String[MAX_LENGTH] = _T("");			//文本框内容
TCHAR		WindowName[] = _T("Function Go");
TCHAR		TextBoxName[] = _T("TextBox");

int			InsPos = _tcslen(String);				//光标位置

ATOM				CreateWindowClass(HINSTANCE hInstance);		//创建主窗口类
LRESULT CALLBACK	WindowProc(HWND, UINT, WPARAM, LPARAM);		//主窗口回调函数
HWND				CreateTextWindow(HWND hParentWnd);			//创建文本框
LRESULT CALLBACK	TextBoxWndProc(HWND, UINT, WPARAM, LPARAM); //文本框回调函数
ATOM				CreateTextBoxClass();						//创建文本框类
HWND				hTextBoxWnd;								//文本框句柄

void	PrintText(HDC hDC);			 //显示文本
void	SetCaretPos(HWND hWnd);      //设置光标位置
void	UpdateWindowEx(HWND hWnd);   //更新窗口
void	ClearBox();					 //清空输入
std::string TCHAR2STRING(TCHAR *STR);

string FunctionName;
float xLdomin = 0, xRdomin = 0, yLdomin = 0, yRdomin = 0;
int dimension = 0;

enum INPUTSTATE
{
	IFUNCTION = 0,
	IDIMENSION,
	IDOMINX,
	IDOMINY,
	DRAW
};
INPUTSTATE inputState = IFUNCTION;


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	CreateWindowClass(hInstance);

	HWND hWnd = CreateWindow(
		WindowName,
		L"Function Go",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if (inputState == DRAW)
		{
			MessageBox(0, L"开始绘图", L" ", MB_ICONASTERISK);
			inputState = IFUNCTION;
			Grapher(FunctionName, dimension, xLdomin, xRdomin, yLdomin, yRdomin);
			MessageBox(0, L"你可以再次输入函数表达式，单击灰色区域结束", L" ", MB_ICONASTERISK);
		}
	}

	return (int)msg.wParam;
}

ATOM CreateWindowClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WindowProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = 0;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(13);
	wcex.lpszMenuName = 0;
	wcex.lpszClassName = WindowName;
	wcex.hIconSm = 0;

	return RegisterClassExW(&wcex);
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{
		CreateTextBoxClass();
		hTextBoxWnd = CreateTextWindow(hWnd);

		MessageBox(0, L"请输入函数表达式，单击灰色区域结束", L" ", MB_ICONASTERISK);
	}
	break;

	case WM_ACTIVATE:
		SetFocus(hTextBoxWnd);
		break;

	case WM_SETCURSOR:
	{
		static HCURSOR hCursor = ::LoadCursor(NULL, IDC_ARROW);
		SetCursor(hCursor);
	}
	break;

	case WM_LBUTTONDOWN:
	{
		switch (inputState)
		{
		case IFUNCTION:
		{
			FunctionName = TCHAR2STRING(String);
			inputState = IDIMENSION;
			MessageBox(0, L"已输入", L" ", MB_ICONASTERISK);
			ClearBox();
			MessageBox(0, L"请输入函数自变量个数，单击灰色区域结束", L" ", MB_ICONASTERISK);
		}
		break;

		case IDIMENSION:
		{
			stringstream ss;
			string sIn = TCHAR2STRING(String);
			ss << sIn;
			ss >> dimension;
			dimension++;
			inputState = IDOMINX;
			MessageBox(0, L"已输入", L" ", MB_ICONASTERISK);
			ClearBox();
			MessageBox(0, L"请输入第一自变量定义域，单击灰色区域结束\n示例：\n-10 10", L" ", MB_ICONASTERISK);
		}
		break;

		case IDOMINX:
		{
			stringstream ss;
			string sIn = TCHAR2STRING(String);
			ss << sIn;
			ss >> xLdomin >> xRdomin;
			MessageBox(0, L"已输入", L" ", MB_ICONASTERISK);
			ClearBox();

			switch (dimension)
			{
			case 2:
				inputState = DRAW;
				break;
			case 3:
				inputState = IDOMINY;
				MessageBox(0, L"请输入第二自变量定义域，单击灰色区域结束\n示例：\n-10 10", L" ", MB_ICONASTERISK);
				break;
			default:
				inputState = DRAW;
				break;
			}

		}
		break;

		case IDOMINY:
		{
			stringstream ss;
			string sIn = TCHAR2STRING(String);
			ss << sIn;
			ss >> yLdomin >> yRdomin;
			MessageBox(0, L"已输入", L" ", MB_ICONASTERISK);
			ClearBox();
			inputState = DRAW;
		}
		break;

		default:
			break;
		}
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

LRESULT CALLBACK TextBoxWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
	{
		static PAINTSTRUCT ps;
		static RECT rect;
		HDC hDC = ::BeginPaint(hWnd, &ps);
		::GetClientRect(hWnd, &rect);
		::DrawEdge(hDC, &rect, EDGE_SUNKEN, BF_RECT);
		PrintText(hDC);
		::EndPaint(hWnd, &ps);
	}
	break;

	case WM_SETFOCUS:
	{
		::CreateCaret(hWnd, (HBITMAP)NULL, 1, TEXTBOX_HEIGHT - 5);
		SetCaretPos(hWnd);
		::ShowCaret(hWnd);
	}
	break;

	case WM_KILLFOCUS:
		::HideCaret(hWnd);
		::DestroyCaret();
		break;

	case WM_SETCURSOR:
	{
		static HCURSOR hCursor = ::LoadCursor(NULL, IDC_IBEAM);
		::SetCursor(hCursor);
	}
	break;

	case WM_CHAR:
	{
		TCHAR code = (TCHAR)wParam;
		int len = ::_tcslen(String);
		if (code < (TCHAR)' ' || len >= MAX_LENGTH)
			return 0;

		::MoveMemory(String + InsPos + 1, String + InsPos, (len - InsPos + 1) * sizeof(TCHAR));
		String[InsPos++] = code;
		UpdateWindowEx(hWnd);
		SetCaretPos(hWnd);
	}
	break;

	case WM_KEYDOWN:
	{
		TCHAR code = (TCHAR)wParam;
		switch (code)
		{
		case VK_LEFT:
			if (InsPos > 0)
				InsPos--;
			break;

		case VK_RIGHT:
			if (InsPos < (int)::_tcslen(String))
				InsPos++;
			break;

		case VK_HOME:
			InsPos = 0;
			break;

		case VK_END:
			InsPos = ::_tcslen(String);
			break;

		case VK_BACK:
			if (InsPos > 0)
			{
				::MoveMemory(String + InsPos - 1, String + InsPos, (::_tcslen(String) - InsPos + 1) * sizeof(TCHAR));
				InsPos--;
				UpdateWindowEx(hWnd);
			}
			break;

		case VK_DELETE:
		{
			int len = ::_tcslen(String);
			if (InsPos < len)
			{
				::MoveMemory(String + InsPos, String + InsPos + 1, (::_tcslen(String) - InsPos + 1) * sizeof(TCHAR));
				UpdateWindowEx(hWnd);
			}
		}
		break;

		default:
			break;
		}
		SetCaretPos(hWnd);
	}
	break;

	case WM_LBUTTONDOWN:
	{
		int x = LOWORD(lParam);
		HDC hDc = ::GetDC(hWnd);
		int strLen = ::_tcslen(String), strPos = 0;
		SIZE size;
		for (strPos = 0; strPos<strLen; strPos++)
		{
			::GetTextExtentPoint(hDc, String, strPos, &size);
			if (size.cx + 4 >= x)
				break;
		}
		InsPos = strPos;
		::GetTextExtentPoint(hDc, String, strPos, &size);
		::SetCaretPos(size.cx + 4, 3);
		::ReleaseDC(hWnd, hDc);
	}
	break;

	default:
		return ::DefWindowProc(hWnd, message, wParam, lParam);
	}

	return (LRESULT)0;
}

HWND CreateTextWindow(HWND hParentWnd)
{
	RECT parentWndRect;
	::GetClientRect(hParentWnd, &parentWndRect);
	int left = (parentWndRect.right - TEXTBOX_WIDTH) / 2, top = (parentWndRect.bottom - TEXTBOX_HEIGHT) / 2;

	HWND hWnd = CreateWindow(
		TextBoxName,
		NULL,
		WS_CHILDWINDOW | WS_VISIBLE,
		left,
		top,
		TEXTBOX_WIDTH,
		TEXTBOX_HEIGHT,
		hParentWnd,
		0,
		hInst,
		0);

	return hWnd;
}

void PrintText(HDC hDC)
{
	int len = ::_tcslen(String);
	::TextOut(hDC, 4, 2, String, len);
}

ATOM CreateTextBoxClass()
{
	WNDCLASSEX wc;
	::ZeroMemory(&wc, sizeof(wc));
	wc.cbSize = sizeof(wc);
	wc.style = CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
	wc.hInstance = hInst;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszClassName = TextBoxName;
	wc.lpfnWndProc = TextBoxWndProc;

	return ::RegisterClassEx(&wc);
}

void SetCaretPos(HWND hWnd)
{
	HDC hDC = ::GetDC(hWnd);
	SIZE size;
	GetTextExtentPoint(hDC, String, InsPos, &size);
	SetCaretPos(4 + size.cx, 3);
	ReleaseDC(hWnd, hDC);
}

void UpdateWindowEx(HWND hWnd)
{
	RECT rect;
	::GetClientRect(hWnd, &rect);
	::InvalidateRect(hWnd, &rect, TRUE);
	::UpdateWindow(hWnd);
}

void ClearBox()
{
	memset(String, MAX_LENGTH, sizeof(TCHAR));
	InsPos = 0;
	UpdateWindowEx(hTextBoxWnd);
}

std::string TCHAR2STRING(TCHAR *STR)
{
	int iLen = WideCharToMultiByte(CP_ACP, 0, STR, -1, NULL, 0, NULL, NULL);
	char* chRtn = new char[iLen * sizeof(char)];
	WideCharToMultiByte(CP_ACP, 0, STR, -1, chRtn, iLen, NULL, NULL);
	std::string str(chRtn);
	delete[]chRtn;
	return str;
}
