#include <windows.h>
#include <windowsx.h>
#include <string>
#include <sstream>
#include <cstdlib>
#include <ctime> 

int Width;
int Height;
int EachWidth;
int EachHeight;
const int WindowWidth = 500*1.5;
const int WindowHeight = 500*1.5;
const char* szAppName = TEXT("Rubiks's cube Wall");

LRESULT CALLBACK EditWndProc(HWND hwnd,UINT Message,WPARAM wParam,LPARAM lParam);
LRESULT CALLBACK WndProc(HWND hwnd,UINT Message,WPARAM wParam,LPARAM lParam);

void SetColor(void);

COLORREF **Map;

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	WNDCLASSEX EditClass;
	WNDCLASSEX WndClass; /* A properties struct of our window */
	HWND hwnd; /* A 'HANDLE', hence the H, or a pointer to our window */
	MSG msg; /* A temporary location for all messages */
	
	HWND EditHwnd;
	
	srand((unsigned int)time(0));
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

	::Map = new COLORREF*[Width];
	for(int i = 0;i < Width;i++)
	{
		::Map[i] = new COLORREF[Height];
	}
	SetColor();
	hwnd = CreateWindowEx(WS_EX_CLIENTEDGE,szAppName,TEXT("魔方墙找茬器"),WS_VISIBLE|WS_OVERLAPPEDWINDOW,
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
	
	for (int i = 0; i < Width;i++)
    {
        delete [] ::Map[i];
    }
    delete [] ::Map;
    
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd,UINT Message,WPARAM wParam,LPARAM lParam)
{
	static PAINTSTRUCT ps;
	static HDC hdc;
	static RECT rect;
	static int Clientx;
	static int Clienty;
	static HBRUSH hBrush;
	static HPEN hPen;
	switch(Message)
	{
		case WM_CREATE:
		{
			Clientx = ((LPCREATESTRUCT)lParam)->cx;
			Clienty = ((LPCREATESTRUCT)lParam)->cy;
			EachWidth = Clientx/2/Width;
			EachHeight = Clienty/Height;
			MoveWindow(hwnd,100,100,2*(::Width*::EachWidth),::Height*::EachHeight + 40,true);
			break;
		}
		
		case WM_PAINT:
		{
			hdc = BeginPaint(hwnd,&ps);
			for(int x = 0;x < Width;x++)
			{
				for(int y = 0;y < Height;y++)
				{
					rect.left = x*::EachWidth;
					rect.top = y*::EachHeight;
					rect.right = (x+1)*::EachWidth;
					rect.bottom = (y+1)*::EachHeight;
					hBrush = CreateSolidBrush(::Map[x][y]);
					FillRect(hdc,&rect,hBrush);
					DeleteObject(hBrush);
				}
			}
			for(int x = 0;x < Width;x++)
			{
				for(int y = 0;y < Height;y++)
				{
					rect.left = x*::EachWidth + Clientx/2;
					rect.top = y*::EachHeight;
					rect.right = (x+1)*::EachWidth + Clientx/2;
					rect.bottom = (y+1)*::EachHeight;
					hBrush = CreateSolidBrush(::Map[x][y]);
					FillRect(hdc,&rect,hBrush);
					DeleteObject(hBrush);
				}
			}
			hPen = CreatePen(PS_SOLID,2,RGB(0,0,0));
			SelectObject(hdc,hPen);
			MoveToEx(hdc,Clientx/2,0,NULL);
			LineTo(hdc,Clientx/2,Clienty); 
			EndPaint(hwnd,&ps);
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
			EditHeight = CreateWindowEx(NULL,TEXT("EDIT"),TEXT("请输入高度:"),WS_CHILD|ES_NUMBER|WS_BORDER|WS_VISIBLE,((LPCREATESTRUCT)lParam)->cx / 3,((LPCREATESTRUCT)lParam)->cy / 3,125,30,hwnd,(HMENU)0,0,0); 
			EditWidth = CreateWindowEx(NULL,TEXT("EDIT"),TEXT("请输入宽度:"),WS_CHILD|ES_NUMBER|WS_BORDER|WS_VISIBLE,((LPCREATESTRUCT)lParam)->cx / 3,((LPCREATESTRUCT)lParam)->cy / 3 + 50,125,30,hwnd,(HMENU)1,0,0); 
			ButtonOK = CreateWindowEx(NULL,TEXT("BUTTON"),"OK",WS_CHILD|BS_PUSHBUTTON|BS_FLAT|WS_VISIBLE,((LPCREATESTRUCT)lParam)->cx / 3,((LPCREATESTRUCT)lParam)->cy / 3 + 100,50,25,hwnd,(HMENU)2,0,0);
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
				std::string Tmpx = Width;
				std::string Tmpy = Height;
				std::stringstream Tmp;
				std::stringstream Tmp2;
				Tmp << Tmpx;
				Tmp >> ::Width;
				Tmp2 << Tmpy;
				Tmp2 >> ::Height;
				if(::Width <= 0 || ::Height <= 0 || WindowWidth/::Width == 0 || WindowHeight/::Height == 0)
				{
					MessageBox(hwnd,TEXT("输入有误，请重新输入:"),TEXT("提示:"),MB_OK);
					SendMessage(EditHeight,WM_SETTEXT,0,(LPARAM)("请重新输入高度:"));
					SendMessage(EditWidth,WM_SETTEXT,0,(LPARAM)("请重新输入宽度:"));
					break; 
				}
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

void SetColor(void)
{
	for(int x = 0;x < Width;x++)
	{
		for(int y = 0;y < Height;y++)
		{
			::Map[x][y] = RGB(rand()%256,rand()%256,rand()%256);
		}
	}
}
