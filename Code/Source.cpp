#include <windows.h>
#include <windowsx.h>
#include <string>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include "resource.h" 

int Width;
int Height;
int EachWidth;
int EachHeight;
int DiffNum;
const int WindowWidth = 500*1.2;
const int WindowHeight = 500*1.2;
const char* szAppName = TEXT("Rubiks's cube Wall");

bool IsEnter = false; //玩家是否正常地输入了各类信息. 

LRESULT CALLBACK EditWndProc(HWND hwnd,UINT Message,WPARAM wParam,LPARAM lParam);
LRESULT CALLBACK WndProc(HWND hwnd,UINT Message,WPARAM wParam,LPARAM lParam);

void SetColor(void);
bool IsInRect(int,int,RECT*);
void MakeDifferent(COLORREF*,POINT*,int);
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
	EditClass.hCursor = LoadCursor(0,IDC_ARROW);
	
	EditClass.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	EditClass.lpszClassName = "GETNUM";
	EditClass.hIcon	= LoadIcon(0,IDI_APPLICATION); /* Load a standard icon */
	EditClass.hIconSm = LoadIcon(0,IDI_APPLICATION); /* use the name "A" to use the project icon */
	
	memset(&WndClass,0,sizeof(WndClass));
	WndClass.style = CS_VREDRAW|CS_HREDRAW;
	WndClass.cbSize = sizeof(WNDCLASSEX);
	WndClass.lpfnWndProc = WndProc;
	WndClass.hInstance = hInstance;
	WndClass.hCursor = LoadCursor(0,IDC_ARROW);
	
	WndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	WndClass.lpszClassName = szAppName;
	WndClass.lpszMenuName = 0;
	WndClass.hIcon = LoadIcon(0,IDI_APPLICATION); /* Load a standard icon */
	WndClass.hIconSm = LoadIcon(0,IDI_APPLICATION); /* use the name "A" to use the project icon */
	
	if(!RegisterClassEx(&EditClass))
	{
		MessageBox(0,"Window Registration Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
		return 0;
	}
	
	if(!RegisterClassEx(&WndClass))
	{
		MessageBox(0,"Window Registration Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
		return 0;
	}
	
	EditHwnd = CreateWindowEx(WS_EX_CLIENTEDGE,"GETNUM",TEXT("请先输入数据:"),WS_VISIBLE,
		CW_USEDEFAULT, /* x */
		CW_USEDEFAULT, /* y */
		400, /* width */
		400, /* height */
		0,0,0,0);
		
	if(EditHwnd == 0)
	{
		MessageBox(0,"Window Creation Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
		return 0;
	}
	
	while(GetMessage(&msg,0,0,0))
	{
		TranslateMessage(&msg); /* Translate key codes to chars if present */
		DispatchMessage(&msg); /* Send it to WndProc */
	}
	
	if(!IsEnter)
	{
		return msg.wParam;
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
		0,0,hInstance,0);
		
	SetWindowLong(hwnd,GWL_WNDPROC,(LONG)WndProc);
	UpdateWindow(hwnd);
	
	if(hwnd == 0)
	{
		MessageBox(0,"Window Creation Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
		return 0;
	}

	while(GetMessage(&msg,0,0,0))
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
	static HMENU hMenu;
	
	static POINT* Different;	//不同的 
	static COLORREF *TmpRgb;
	static int Count;
	
	bool IsDifferent = false;
	
	static char szBuffer[45];
	static int Time = 0;
	
	switch(Message)
	{
		case WM_TIMER:
		{
			Time++;
			wsprintf(szBuffer,TEXT("魔方墙找茬器 剩余:%d 时间:%ds"),Count,Time);
			SetWindowText(hwnd,szBuffer);
			if(Count == 0)
			{
				wsprintf(szBuffer,TEXT("恭喜您，在%d秒内找完全部不同方块!"),Time);
				KillTimer(hwnd,0);
				delete [] Different;
				delete [] TmpRgb;
				MessageBox(hwnd,szBuffer,TEXT("提示"),MB_OK);
				DestroyWindow(hwnd);
			}
			break;
		}
		
		case WM_CREATE:
		{
			hMenu = CreateMenu();
			AppendMenu(hMenu,MF_SEPARATOR,0,NULL);//把分割线附加到系统菜单中
			AppendMenu(hMenu,0,IDM_HELP,TEXT("帮助"));//把菜单项help附加到系统菜单中
			AppendMenu(hMenu,0,IDM_RESTART,TEXT("重新开始"));
			AppendMenu(hMenu,0,IDM_ABOUT,TEXT("关于"));
			SetMenu(hwnd,hMenu);
			
			Clientx = ((LPCREATESTRUCT)lParam)->cx;
			Clienty = ((LPCREATESTRUCT)lParam)->cy;
			EachWidth = round(Clientx/2/Width);
			EachHeight = round(Clienty/Height);
			MoveWindow(hwnd,100,100,2*(::Width*::EachWidth),(::Height+1)*::EachHeight,true);
			Different = new POINT[DiffNum];
			TmpRgb = new COLORREF[DiffNum];
			MakeDifferent(TmpRgb,Different,DiffNum);
			Count = DiffNum;
			SetTimer(hwnd,0,1000,0);
			break;
		}
		case WM_COMMAND:
		{
			if(lParam == 0)
			{
				switch(LOWORD(wParam))
				{
					case IDM_HELP:MessageBox(hwnd,TEXT("点击屏幕右侧不同的方块进行游戏."),TEXT("帮助"),MB_ICONINFORMATION);break; 
					case IDM_RESTART:/*TODO:添加重新开始游戏的代码.*/break;
					case IDM_ABOUT:MessageBox(hwnd,TEXT("魔方墙找茬器,用于训练3D眼.\n\n作者:Vmpy \nGithub:www.github.com/vmpy"),TEXT("帮助"),MB_ICONINFORMATION);break; 
				}
			}
			break; 
		}
		case WM_SIZE:
		{
			Clientx = GET_X_LPARAM(lParam);
			Clienty = GET_Y_LPARAM(lParam);
			EachWidth = round(Clientx/2/Width);
			EachHeight = round(Clienty/Height);
			break;
		}
		
		case WM_PAINT:
		{
			hdc = BeginPaint(hwnd,&ps);
			//绘制左边区域 
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
			//绘制右边区域 
			for(int x = 0;x < Width;x++)
			{
				for(int y = 0;y < Height;y++)
				{
					rect.left = x*::EachWidth + Clientx/2;
					rect.top = y*::EachHeight;
					rect.right = (x+1)*::EachWidth + Clientx/2;
					rect.bottom = (y+1)*::EachHeight;
					for(int i = 0;i < DiffNum;i++)
					{
						if(Different[i].x != -1 && Different[i].y != -1 && Different[i].x == x && Different[i].y == y)
						{
							hBrush = CreateSolidBrush(TmpRgb[i]);
							IsDifferent = true;
						}
						
					}
					if(!IsDifferent)
					{
						hBrush = CreateSolidBrush(::Map[x][y]);
						IsDifferent = false;
					}
					else
					{
						IsDifferent = false;
					}
					FillRect(hdc,&rect,hBrush);
					DeleteObject(hBrush);
				}
			}
			//画出分割线，便于观察. 
			hPen = CreatePen(PS_SOLID,2,RGB(0,0,0));
			SelectObject(hdc,hPen);
			MoveToEx(hdc,Clientx/2,0,0);
			LineTo(hdc,Clientx/2,Clienty); 
			EndPaint(hwnd,&ps);
			break; 
		}
		
		case WM_LBUTTONUP:
		{
			int xPos = GET_X_LPARAM(lParam);
			int yPos = GET_Y_LPARAM(lParam);
			for(int x = 0;x < Width;x++)
			{
				for(int y = 0;y < Height;y++)
				{
					rect.left = x*::EachWidth + Clientx/2;
					rect.top = y*::EachHeight;
					rect.right = (x+1)*::EachWidth + Clientx/2;
					rect.bottom = (y+1)*::EachHeight;
					
					if(IsInRect(xPos,yPos,&rect))
					{
						for(int i = 0;i < DiffNum;i++)
						{
							//检测是否击中了目标位置(魔方墙中的不同颜色方块). 
							if(Different[i].x == x && Different[i].y == y)
							{
								Different[i].x = -1;
								Different[i].y = -1;
								Count--;
								InvalidateRect(hwnd,0,true);
								return 0;
							}
						}
					}
				}
			}
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

/**
*信息输入窗口窗口过程. 
*/
LRESULT CALLBACK EditWndProc(HWND hwnd,UINT Message,WPARAM wParam,LPARAM lParam)
{
	static HWND EditDiffNum;
	static HWND EditWidth;
	static HWND EditHeight;
	static HWND ButtonOK;
	static char Width[32];
	static char Height[32];
	static char DiffNum[32];
	static HFONT hFont;
	
	switch(Message)
	{
		case WM_CREATE:
		{
			hFont = CreateFont(18,8,0,0,FW_THIN,false,false,false,GB2312_CHARSET,OUT_CHARACTER_PRECIS,
            CLIP_CHARACTER_PRECIS,DEFAULT_QUALITY,FF_MODERN,TEXT("宋体"));
            
			EditDiffNum = EditHeight = CreateWindowEx(0,TEXT("EDIT"),TEXT("请输入不同方块数目:"),WS_CHILD|ES_NUMBER|ES_LEFT|WS_BORDER|WS_VISIBLE,((LPCREATESTRUCT)lParam)->cx / 3,((LPCREATESTRUCT)lParam)->cy / 3 - 50,180,25,hwnd,(HMENU)-1,0,0);
			EditHeight = CreateWindowEx(0,TEXT("EDIT"),TEXT("请输入高度:"),WS_CHILD|ES_NUMBER|ES_LEFT|WS_BORDER|WS_VISIBLE,((LPCREATESTRUCT)lParam)->cx / 3,((LPCREATESTRUCT)lParam)->cy / 3,150,25,hwnd,(HMENU)0,0,0); 
			EditWidth = CreateWindowEx(0,TEXT("EDIT"),TEXT("请输入宽度:"),WS_CHILD|ES_NUMBER|ES_LEFT|WS_BORDER|WS_VISIBLE,((LPCREATESTRUCT)lParam)->cx / 3,((LPCREATESTRUCT)lParam)->cy / 3 + 50,150,25,hwnd,(HMENU)1,0,0); 
			ButtonOK = CreateWindowEx(0,TEXT("BUTTON"),"OK",WS_CHILD|BS_PUSHBUTTON|BS_FLAT|WS_VISIBLE,((LPCREATESTRUCT)lParam)->cx / 3,((LPCREATESTRUCT)lParam)->cy / 3 + 100,50,25,hwnd,(HMENU)2,0,0);
			SendMessage(EditDiffNum,WM_SETFONT,(WPARAM)hFont,0);
			SendMessage(EditHeight,WM_SETFONT,(WPARAM)hFont,0);
			SendMessage(EditWidth,WM_SETFONT,(WPARAM)hFont,0);
			SendMessage(ButtonOK,WM_SETFONT,(WPARAM)hFont,0);
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
				SendMessage(EditDiffNum,WM_GETTEXT,32,(LPARAM)DiffNum);
				
				//转换字符串与数字. 
				std::string Tmpx = Width;
				std::string Tmpy = Height;
				std::string TmpNum = DiffNum;
				std::stringstream Tmp;
				std::stringstream Tmp2;
				std::stringstream Tmp3;
				Tmp << Tmpx;
				Tmp >> ::Width;
				Tmp2 << Tmpy;
				Tmp2 >> ::Height;
				Tmp3 << TmpNum;
				Tmp3 >> ::DiffNum;
				
				if(::DiffNum <= 0 || ::DiffNum > ::Width*::Height || ::Width <= 0 || ::Height <= 0 || WindowWidth/2/::Width == 0 || WindowHeight/::Height == 0)
				{
					MessageBox(hwnd,TEXT("输入有误，请重新输入:"),TEXT("提示:"),MB_OK);
					SendMessage(EditHeight,WM_SETTEXT,0,(LPARAM)("请重新输入高度:"));
					SendMessage(EditWidth,WM_SETTEXT,0,(LPARAM)("请重新输入宽度:"));
					break;
				}
				//销毁窗口.
				IsEnter = true;
				DestroyWindow(EditDiffNum);
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

/**
*为每一个方块取一个随机颜色值. 
*/
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

/**
*检测一个点是否在一个矩形中.
*@param x:点的x坐标.
*@param y:点的y坐标.
*@return:bool. 
*/
bool IsInRect(int x,int y,RECT *rect)
{
	if(x > rect->left && x < rect->right && y > rect->top && y < rect->bottom)
	{
		return true;
	}
	return false;
}

/**
*制造i个不同区域(坐标与颜色值)
*@param TmpRgb:不同方块区域的颜色值(随机设置).
*@param Point:不同方块点坐标.
*@param i:不同方块的数量. 
*/
void MakeDifferent(COLORREF* TmpRgb,POINT* Point,int i)
{
	for(int index = 0;index < i;index++)
	{
		do{
			Point[index].x = rand()%Width;
			Point[index].y = rand()%Height;
		}while(Point[index].x == Point[index-1].x && Point[index].y == Point[index-1].y && index != 0);
		
		do{
			TmpRgb[index] = RGB(rand()%256,rand()%256,rand()%256);
		}while(TmpRgb[index] == ::Map[Point[index].x][Point[index].y]);
	}
}
