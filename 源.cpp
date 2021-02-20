#include <windows.h>
#include <tchar.h>
#include <time.h>
#pragma comment (lib,"winmm.lib")
#pragma comment (lib,"Msimg32.lib")

#define WINDOW_WIDTH   736
#define WINDOW_HEIGHT  800
#define WINDOW_TITLE   L"滚蛋吧  （作者：范效萌）"
#define MAX 16
#define LEVELMAX 10
#define MAXTARGET 8
#define MAXINVARIANT 30
#define ELEMENTSIZE 45

int level_count = 0; //这个表示当前关卡
int complexion[MAX][MAX];
int temparr[MAX][MAX];
wchar_t ptext[10][1000];
HDC g_hdc, g_mdc, g_bufdc;
HBITMAP g_hBackGround, g_hBoll, g_hBind, g_hWall,g_renovate,g_next,g_pre,g_help,g_target;

void wind_move(int, int num,HWND hwnd);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL Res_Init(HWND hwnd);    //这个函数用来初始化每一关卡的情况
VOID Draw_Paint(HWND hwnd);  //这个函数用来绘制界面
BOOL Res_Clean(HWND);        //
void setLevel();             //这个函数用来设置每个关卡
int  check_pass();           //这个函数用来测试当前局面是否过关
void ass_ptext();            //为文本字符串指针指定文本内容

struct LEVEL {
	int bind; //绑定蛋的数量
	int boll; //全部蛋的数量
	int step; //当前走了多少步
	int target[MAXTARGET]; //目标位置的最多数量
	int invariant[MAXINVARIANT]; //固定铁壁的最多数量
}level[LEVELMAX];

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShwoCmd)
{
	WNDCLASSEX wndClass = { 0 };							//用WINDCLASSEX定义了一个窗口类
	wndClass.cbSize = sizeof(WNDCLASSEX);			//设置结构体的字节数大小
	wndClass.style = CS_HREDRAW | CS_VREDRAW;	//设置窗口的样式
	wndClass.lpfnWndProc = WndProc;					//设置指向窗口过程函数的指针
	wndClass.cbClsExtra = 0;								//窗口类的附加内存，取0就可以了
	wndClass.cbWndExtra = 0;							//窗口的附加内存，依然取0就行了
	wndClass.hInstance = hInstance;						//指定包含窗口过程的程序的实例句柄。
	wndClass.hIcon = (HICON)::LoadImage(NULL, L"icon.ico", IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);  //本地加载自定义ico图标
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);    //指定窗口类的光标句柄。
	wndClass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);  //为hbrBackground成员指定一个白色画刷句柄	
	wndClass.lpszMenuName = NULL;						//用一个以空终止的字符串，指定菜单资源的名字。
	wndClass.lpszClassName = L"ForTheDreamOfGameDevelop";

	if (!RegisterClassEx(&wndClass)) return -1;
	setLevel();ass_ptext();
	HWND hwnd = CreateWindow(L"ForTheDreamOfGameDevelop", WINDOW_TITLE,
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH,
		WINDOW_HEIGHT, NULL, NULL, hInstance, NULL);

	MoveWindow(hwnd, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, true);
	ShowWindow(hwnd, nShwoCmd);
	UpdateWindow(hwnd);

	if (!Res_Init(hwnd))
	{
		MessageBox(hwnd, L"游戏创建失败", L"消息窗口", 0);
		return false;
	}

	MSG msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	UnregisterClass(L"ForTheDreamOfGameDevelop", hInstance);
	return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)						//switch语句开始
	{
	case WM_PAINT:						// 若是客户区重绘消息
		ValidateRect(hwnd, NULL);		// 更新客户区的显示
		break;									//跳出该switch语句

	case WM_LBUTTONDOWN:  //接收鼠标点击的消息
	{
						   int j = LOWORD(lParam) / ELEMENTSIZE;
						   int i = HIWORD(lParam) / ELEMENTSIZE;
						   int k = 0;
						   
						   if (complexion[i][j] == 1)
						   {
							  for (int column = 0;column < MAX;column++) {
								  for (int row = 0;row < MAX;row++) {
									  if (complexion[column][row] > 1 && complexion[column][row] < 99)
										  complexion[column][row]--;
								  }
							  }
							  complexion[i][j] += level[level_count].bind;
							  level[level_count].step++;
							  Draw_Paint(hwnd);
						    }
						   else if (complexion[i][j] == 100) {
							   Res_Init(hwnd);   //这个用来初始化关卡
							   Draw_Paint(hwnd); //重画关卡
						   }
						   else if (complexion[i][j] == 101 && level[level_count].step>0) {
							   level[level_count].step--;
							   for (int i = 0;i < MAX;i++)
								   for (int j = 0;j < MAX;j++)
									   complexion[i][j] = temparr[i][j];
							   Draw_Paint(hwnd);
						   }
						   else if (complexion[i][j] == 102) {
							   if (check_pass()) //check_pass用来检测是否过关
							   {
								   level_count++;
								   Res_Init(hwnd);
								   TextOut(g_hdc, 50, 50, ptext[0], wcslen(ptext[0]));
							   }
						   }
						  
	}
		break;

	case WM_KEYDOWN: // 接收键盘按下消息
		if (1)
		{
			switch (wParam)
			{
			case VK_UP:
				wind_move(8, 0, hwnd);
				break;
			case VK_LEFT:
				wind_move(4, 0,hwnd);
				break;
			case VK_RIGHT:
				wind_move(6, 0,hwnd);
				break;
			case VK_DOWN:
				wind_move(2, 0,hwnd);
				break;
			}
		}

		Draw_Paint(hwnd);
		if (wParam == VK_ESCAPE)    // 如果被按下的键是ESC
			DestroyWindow(hwnd);		// 销毁窗口, 并发送一条WM_DESTROY消息
		break;									//跳出该switch语句

	case WM_DESTROY:				//若是窗口销毁消息
		PostQuitMessage(0);		//向系统表明有个线程有终止请求。用来响应WM_DESTROY消息
		break;								//跳出该switch语句

	default:									//若上述case条件都不符合，则执行该default语句
		return DefWindowProc(hwnd, message, wParam, lParam);		//调用缺省的窗口过程
	}

	return 0;
}

BOOL Res_Init(HWND hwnd)
{
	for (int i = 0; i < MAX; i++) 
		for (int j = 0; j < MAX; j++) 
			complexion[i][j] = 0;

	for (int i = 0; i < MAX; i++) //这两个嵌套循环用
	    for (int j = 0; j < MAX; j++) //于设置四面的墙壁
		    if (0 == i || 0 == j || MAX - 1 == i || MAX - 1 == j) complexion[i][j] = 99;

	for (int i = 0;level[level_count].invariant[i] != 0;i++) //这个用来设置每一关的额外墙壁
		complexion[level[level_count].invariant[i] / MAX][level[level_count].invariant[i] % MAX] = 99;
	
	for (int j = 1;j <= level[level_count].boll;j++) complexion[1][j] = 1; //这个用于设置每一关的球和位置

	for (int j = 1;j <= level[level_count].bind;j++) complexion[1][j] += j;//这用于设置每一关的特殊球数和位置

	complexion[0][0] = 100; //这个是刷新
	complexion[0][1] = 101;
	complexion[0][2] = 102; //这个是下一关
	complexion[0][3] = 103; //这个是帮助

	HBITMAP bmp;
	g_hdc   = GetDC(hwnd);
	g_mdc   = CreateCompatibleDC(g_hdc);
	g_bufdc = CreateCompatibleDC(g_hdc);
	bmp = CreateCompatibleBitmap(g_hdc, WINDOW_WIDTH, WINDOW_HEIGHT);
	SelectObject(g_mdc, bmp);
	g_hBackGround = (HBITMAP)::LoadImage(NULL, L"g_hBackGround.bmp", IMAGE_BITMAP, WINDOW_WIDTH, WINDOW_HEIGHT, LR_LOADFROMFILE);
	g_hBoll    = (HBITMAP)::LoadImage(NULL, L"boll.bmp", IMAGE_BITMAP, ELEMENTSIZE, ELEMENTSIZE, LR_LOADFROMFILE);
	g_hBind    = (HBITMAP)::LoadImage(NULL, L"蜘蛛网.bmp", IMAGE_BITMAP, ELEMENTSIZE, ELEMENTSIZE, LR_LOADFROMFILE);
	g_hWall    = (HBITMAP)::LoadImage(NULL, L"wall.bmp", IMAGE_BITMAP, ELEMENTSIZE, ELEMENTSIZE, LR_LOADFROMFILE);
	g_renovate = (HBITMAP)::LoadImage(NULL, L"刷新.bmp", IMAGE_BITMAP, ELEMENTSIZE, ELEMENTSIZE, LR_LOADFROMFILE);
	g_next     = (HBITMAP)::LoadImage(NULL, L"下一关.bmp", IMAGE_BITMAP, ELEMENTSIZE, ELEMENTSIZE, LR_LOADFROMFILE);
	g_pre      = (HBITMAP)::LoadImage(NULL, L"上一关.bmp", IMAGE_BITMAP, ELEMENTSIZE, ELEMENTSIZE, LR_LOADFROMFILE);
	g_help     = (HBITMAP)::LoadImage(NULL, L"帮助.bmp", IMAGE_BITMAP, ELEMENTSIZE, ELEMENTSIZE, LR_LOADFROMFILE);
	g_target   = (HBITMAP)::LoadImage(NULL, L"target.bmp", IMAGE_BITMAP, ELEMENTSIZE, ELEMENTSIZE, LR_LOADFROMFILE);

	HFONT hFont;
	hFont = CreateFont(20, 0, 0, 0, 100, 0, 0, 0, GB2312_CHARSET, 0, 0, 0, 0, TEXT("微软雅黑"));
	SelectObject(g_mdc, hFont);
	SetBkMode(g_hdc, TRANSPARENT);    //设置文字显示背景透明
	Draw_Paint(hwnd);
	return true;
}
VOID Draw_Paint(HWND hwnd)
{
	SelectObject(g_bufdc, g_hBackGround);
	BitBlt(g_mdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, g_bufdc, 0, 0, SRCCOPY);
	
	for (int i,j,a = 0;level[level_count].target[a] != 0;a++) //这个循环用来设置目标位置背景
	{
		 i = level[level_count].target[a] / MAX;
		 j = level[level_count].target[a] % MAX;
		 SelectObject(g_bufdc, g_target);
		 TransparentBlt(g_mdc, j * ELEMENTSIZE, i * ELEMENTSIZE, ELEMENTSIZE, ELEMENTSIZE, g_bufdc, 0, 0, ELEMENTSIZE, ELEMENTSIZE, RGB(254, 254, 254));
	}

	for (int i = 0; i < MAX; i++)
	for (int j = 0; j < MAX; j++)
	{
		if (complexion[i][j]>=1 && complexion[i][j]<99)
		{
			SelectObject(g_bufdc, g_hBoll);
			TransparentBlt(g_mdc, j * ELEMENTSIZE, i * ELEMENTSIZE, ELEMENTSIZE, ELEMENTSIZE, g_bufdc, 0, 0, ELEMENTSIZE, ELEMENTSIZE, RGB(254, 254, 254));
		}
		if (complexion[i][j] > 1 && complexion[i][j]<99)
		{
			SelectObject(g_bufdc, g_hBind);
			TransparentBlt(g_mdc, j * ELEMENTSIZE, i * ELEMENTSIZE, ELEMENTSIZE, ELEMENTSIZE, g_bufdc, 0, 0, ELEMENTSIZE, ELEMENTSIZE, RGB(254, 254, 254));
		}
		if (complexion[i][j] == 99)
		{
			SelectObject(g_bufdc, g_hWall);
			TransparentBlt(g_mdc, j * ELEMENTSIZE, i * ELEMENTSIZE, ELEMENTSIZE, ELEMENTSIZE, g_bufdc, 0, 0, ELEMENTSIZE, ELEMENTSIZE, RGB(254, 254, 254));
		}
		if (complexion[i][j] == 100)
		{
			SelectObject(g_bufdc, g_renovate);
			TransparentBlt(g_mdc, j * ELEMENTSIZE, i * ELEMENTSIZE, ELEMENTSIZE, ELEMENTSIZE, g_bufdc, 0, 0, ELEMENTSIZE, ELEMENTSIZE, RGB(254, 254, 254));
		}
		if (complexion[i][j] == 101)
		{
			SelectObject(g_bufdc, g_pre);
			TransparentBlt(g_mdc, j * ELEMENTSIZE, i * ELEMENTSIZE, ELEMENTSIZE, ELEMENTSIZE, g_bufdc, 0, 0, ELEMENTSIZE, ELEMENTSIZE, RGB(254, 254, 254));
		}
		if (complexion[i][j] == 102)
		{
			SelectObject(g_bufdc, g_next);
			TransparentBlt(g_mdc, j * ELEMENTSIZE, i * ELEMENTSIZE, ELEMENTSIZE, ELEMENTSIZE, g_bufdc, 0, 0, ELEMENTSIZE, ELEMENTSIZE, RGB(254, 254, 254));
		}
		if (complexion[i][j] == 103)
		{
			SelectObject(g_bufdc, g_help);
			TransparentBlt(g_mdc, j * ELEMENTSIZE, i * ELEMENTSIZE, ELEMENTSIZE, ELEMENTSIZE, g_bufdc, 0, 0, ELEMENTSIZE, ELEMENTSIZE, RGB(254, 254, 254));
		}
	}

	BitBlt(g_hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, g_mdc, 0, 0, SRCCOPY);
}

void wind_move(int wind, int m,HWND hwnd)
{
	level[level_count].step++;
	for (int i = 0;i < MAX;i++)
		for (int j = 0;j < MAX;j++)
			temparr[i][j] = complexion[i][j]; //这是用来悔棋的，只能一步！

	int i, j, k;
	for (k = 0; k < MAX; k++)
	{
		switch (wind)
		{
		case 2:
			for (i = MAX - 1; i >= 0; i--)
			for (j = MAX - 1; j >= 0; j--)
			{
				if (complexion[i][j] == 1 && 0 == complexion[i + 1][j])
				{
					complexion[i][j] = 0;
					complexion[i + 1][j] = 1;
				}
			}
			break;

		case 4:
			for (int i = 0; i < MAX; i++)
			for (int j = 0; j < MAX; j++)
			{
				if (complexion[i][j] == 1 && 0 == complexion[i][j - 1])
				{
					complexion[i][j] = 0;
					complexion[i][j - 1] = 1;
				}
			}
			break;

		case 6:
			for (i = MAX - 1; i >= 0; i--)
			for (j = MAX - 1; j >= 0; j--)
			{
				if (complexion[i][j] == 1 && 0 == complexion[i][j + 1])
				{
					complexion[i][j] = 0;
					complexion[i][j + 1] = 1;
				}
			}
			break;

		case 8:
			for (int i = 0; i < MAX; i++)
			for (int j = 0; j < MAX; j++)
			{
				if (complexion[i][j] == 1 && 0 == complexion[i - 1][j])
				{
					complexion[i][j] = 0;
					complexion[i - 1][j] = 1;
				}
			}
			break;
		}
		Draw_Paint(hwnd);
		for (int i = 0;i < 4000000;i++);
	}
}

int check_pass()
{
	int result = 1;
	for (int i = 0;level[level_count].target[i] != 0;i++)
	{
		if (complexion[ level[level_count].target[i] / MAX ][ level[level_count].target[i] % MAX ] > 0
			&&
			complexion[ level[level_count].target[i] / MAX ][ level[level_count].target[i] % MAX ] < 99)
			result *= 1;
		else result *= 0;
	}
	return result;
}
void setLevel()
{
	/*****第1关*****/
	for (int i = 0;i < MAXTARGET;i++) level[0].target[i] = 0;
	for (int i = 0;i < MAXINVARIANT;i++) level[0].invariant[i] = 0;
	level[0].bind = 1;
	level[0].boll = 2;
	level[0].step = 0;
	level[0].target[0] = 27;level[0].target[1] = 234;

	/*****第2关*******/
	for (int i = 0;i < MAXTARGET;i++) level[1].target[i] = 0;
	for (int i = 0;i < MAXINVARIANT;i++) level[1].invariant[i] = 0;
	level[1].bind = 2;
	level[1].boll = 3;
	level[1].step = 0;
	level[1].target[0] = 69;

	/*******第3关********/
	for (int i = 0;i < MAXTARGET;i++) level[2].target[i] = 0;
	for (int i = 0;i < MAXINVARIANT;i++) level[2].invariant[i] = 0;
	level[2].bind = 1;
	level[2].boll = 3;
	level[2].step = 0;
	level[2].target[0] = 77;

	/********第4关********/
	int a = 2;
	a++;
	for (int i = 0;i < MAXTARGET;i++)    level[a].target[i] = 0;
	for (int i = 0;i < MAXINVARIANT;i++) level[a].invariant[i] = 0;
	level[a].bind = 0;
	level[a].boll = 4;
	level[a].step = 0;
	level[a].target[0] = 17;level[a].target[1] = 33;level[a].target[2] = 49;level[a].target[3] = 65;
	level[a].invariant[0] = 238;

	/********第5关********/
	a++;
	for (int i = 0;i < MAXTARGET;i++)    level[a].target[i] = 0;
	for (int i = 0;i < MAXINVARIANT;i++) level[a].invariant[i] = 0;
	level[a].bind = 2;
	level[a].boll = 4;
	level[a].step = 0;
	level[a].target[0] = 119;level[a].target[1] = 120;level[a].target[2] = 135;level[a].target[3] = 136;

	/*********第6关**********/
	a++;
	for (int i = 0;i < MAXTARGET;i++)    level[a].target[i] = 0;
	for (int i = 0;i < MAXINVARIANT;i++) level[a].invariant[i] = 0;
	level[a].bind = 2;
	level[a].boll = 4;
	level[a].step = 0;
	level[a].target[0] = 180;level[a].target[1] = 228;level[a].target[2] = 46;level[a].target[3] = 62;

	/*********第7关**********/
	a++;
	for (int i = 0;i < MAXTARGET;i++)    level[a].target[i] = 0;
	for (int i = 0;i < MAXINVARIANT;i++) level[a].invariant[i] = 0;
	level[a].bind = 2;
	level[a].boll = 4;
	level[a].step = 0;
	level[a].target[0] = 26;level[a].target[1] = 171;level[a].target[2] = 177;level[a].target[3] = 17;
	level[a].invariant[0] = 21;level[a].invariant[1] = 81;
}

void ass_ptext()
{
}
