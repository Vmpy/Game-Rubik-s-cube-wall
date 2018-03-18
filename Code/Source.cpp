#include <windows.h>
#include <windowsx.h>

const int WindowWidth = 640;
const int WindowHeight = 480;
const char* szAppName = TEXT("Rubiks's cube Wall");
LRESULT CALLBACK EditWndProc(HWND hwnd,UINT Message,WPARAM wParam,LPARAM lParam);
LRESULT CALLBACK WndProc(HWND hwnd,UINT Message,WPARAM wParam,LPARAM lParam);
LRESULT CALLBACK LeftWndProc(HWND hwnd,UINT Message,WPARAM wParam,LPARAM lParam);
LRESULT CALLBACK RightWndProc(HWND hwnd,UINT Message,WPARAM wParam,LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	WNDCLASSEX EditClass;
	WNDCLASSEX WndClass; /* A properties struct of our window */
	HWND hwnd; /* A 'HANDLE', hence the H, or a pointer to our window */
	MSG msg; /* A temporary location for all messages */
	
	HWND EditHwnd;

	memset(&EditClass,0,sizeof(EditClass));
	EditClass.cbSize = sizeof(WNDCLASSEX);
	EditClass.lpfnWndProc = EditWndProc;
	EditClass.hInstance	 = hInstance;
	EditClass.hCursor = LoadCursor(NULL,IDC_ARROW);
	
	EditClass.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	EditClass.lpszClassName = "GETNUM";
	EditClass.hIcon	= LoadIcon(NULL,IDI_APPLICATION); /* Load a standard icon */
	EditClass.hIconSm = LoadIcon(NULL,IDI_APPLICATION); /* use the name "A" to use the project icon */
	
	memset(&WndClass,0,sizeof(WndClass));
	WndClass.cbSize = sizeof(WNDCLASSEX);
	WndClass.lpfnWndProc = WndProc;
	WndClass.hInstance = hInstance;
	WndClass.hCursor = LoadCursor(NULL,IDC_ARROW);
	
	WndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	WndClass.lpszClassName = szAppName;
	WndClass.hIcon = LoadIcon(NULL,IDI_APPLICATION); /* Load a standard icon */
	WndClass.hIconSm = LoadIcon(NULL,IDI_APPLICATION); /* use the name "A" to use the project icon */
	
	if(!RegisterClassEx(&EditClass))
	{
		MessageBox(NULL,"Window Registration Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
		return 0;
	}
	if(!RegisterClassEx(&WndClass))
	{
		MessageBox(NULL,"Window Registration Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
		return 0;
	}
	
	EditHwnd = CreateWindowEx(WS_EX_CLIENTEDGE,"GETNUM",TEXT("请先输入数据:"),WS_VISIBLE,
		CW_USEDEFAULT, /* x */
		CW_USEDEFAULT, /* y */
		400, /* width */
		400, /* height */
		NULL,NULL,NULL,NULL);
		
	if(EditHwnd == NULL)
	{
		MessageBox(NULL,"Window Creation Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
		return 0;
	}
	
	while(GetMessage(&msg,NULL,0,0))
	{
		TranslateMessage(&msg); /* Translate key codes to chars if present */
		DispatchMessage(&msg); /* Send it to WndProc */
	}

	hwnd = CreateWindowEx(WS_EX_CLIENTEDGE,szAppName,"Caption",WS_VISIBLE|WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, /* x */
		CW_USEDEFAULT, /* y */
		WindowWidth, /* width */
		WindowHeight, /* height */
		NULL,NULL,hInstance,NULL);
		
	SetWindowLong(hwnd,GWL_WNDPROC,(LONG)WndProc);
	UpdateWindow(hwnd);
	
	if(hwnd == NULL)
	{
		MessageBox(NULL,"Window Creation Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
		return 0;
	}

	while(GetMessage(&msg,NULL,0,0))
	{
		TranslateMessage(&msg); /* Translate key codes to chars if present */
		DispatchMessage(&msg); /* Send it to WndProc */
	}
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd,UINT Message,WPARAM wParam,LPARAM lParam)
{
	static int Clientx;
	static int Clienty;
	static HWND LeftWindow;
	static HWND RightWindow;
	switch(Message)
	{
		case WM_CREATE:
		{
			Clientx = ((LPCREATESTRUCT)lParam)->cx;
			Clienty = ((LPCREATESTRUCT)lParam)->cy;
			LeftWindow = CreateWindowEx(NULL,szAppName,TEXT(""),WS_CHILD,CW_USEDEFAULT,CW_USEDEFAULT,Clientx/2,Clienty,hwnd,NULL,NULL,NULL);
			SetWindowLong(LeftWindow,GWL_WNDPROC,(LONG)LeftWndProc);
			RightWindow = CreateWindowEx(NULL,szAppName,TEXT(""),WS_CHILD,CW_USEDEFAULT + Clientx/2,CW_USEDEFAULT,Clientx/2,Clienty,hwnd,NULL,NULL,NULL);
			SetWindowLong(RightWindow,GWL_WNDPROC,(LONG)RightWndProc);
			break;
		} 
		
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			break;
		}
		
		default:
			return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK EditWndProc(HWND hwnd,UINT Message,WPARAM wParam,LPARAM lParam)
{
	static HWND EditWidth;
	static HWND EditHeight;
	static HWND ButtonOK;
	static char Width[32];
	static char Height[32];
	switch(Message)
	{
		case WM_CREATE:
		{
			EditHeight = CreateWindowEx(NULL,TEXT("EDIT"),TEXT("请输入高度:"),WS_CHILD|ES_NUMBER|WS_BORDER|WS_VISIBLE,((LPCREATESTRUCT)lParam)->cx / 3,((LPCREATESTRUCT)lParam)->cy / 3,100,20,hwnd,(HMENU)0,0,0); 
			EditWidth = CreateWindowEx(NULL,TEXT("EDIT"),TEXT("请输入宽度:"),WS_CHILD|ES_NUMBER|WS_BORDER|WS_VISIBLE,((LPCREATESTRUCT)lParam)->cx / 3,((LPCREATESTRUCT)lParam)->cy / 3 + 25,100,20,hwnd,(HMENU)1,0,0); 
			ButtonOK = CreateWindowEx(NULL,TEXT("BUTTON"),"OK",WS_CHILD|BS_PUSHBUTTON|WS_VISIBLE,((LPCREATESTRUCT)lParam)->cx / 3,((LPCREATESTRUCT)lParam)->cy / 3 + 100,50,25,hwnd,(HMENU)2,0,0);
			break;
		}
		
		case WM_COMMAND:
		{
			switch(HIWORD(wParam))
			{
				case EN_SETFOCUS:
				{
					SendMessage((HWND)lParam,WM_SETTEXT,0,(LPARAM)"");
					break;
				}
			}
			if(LOWORD(wParam) == 2)
			{
				SendMessage(EditHeight,WM_GETTEXT,32,(LPARAM)Height);
				SendMessage(EditWidth,WM_GETTEXT,32,(LPARAM)Width);
				DestroyWindow(EditHeight);
				DestroyWindow(EditWidth);
				DestroyWindow(ButtonOK);
				DestroyWindow(hwnd);
			}
			break;
		}
		
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			break;
		}
		
		default:
			return DefWindowProc(hwnd,Message,wParam,lParam);
	}
	return 0;
}

LRESULT CALLBACK LeftWndProc(HWND hwnd,UINT Message,WPARAM wParam,LPARAM lParam)
{
	switch(Message)
	{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			break;
		}
		
		default:
			return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK RightWndProc(HWND hwnd,UINT Message,WPARAM wParam,LPARAM lParam)
{
	switch(Message)
	{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			break;
		}
		
		default:
			return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}
