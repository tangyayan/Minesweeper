#define _CRT_SECURE_NO_WARNINGS 1
#include <graphics.h>
#include <conio.h>
#include <math.h>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string>
#pragma comment(lib, "winmm.lib")  // 链接winmm.lib库

int boom[210][210],play[210][210],numap[210][210];
//play数组是玩家的地图，1表示已经开了，0表示还未开，2表示标雷,bomtime表示地雷的时间
const int dx[8] = {-1,1, 0,0,-1,1,-1, 1};
const int dy[8] = { 0,0,-1,1,-1,1, 1,-1};
int rg = 100, a = 30;//第一行长50，正方形边长为25
int rgx, rgy, boomm;//模式一9*9(10),模式二16*16(40),模式三30*16(99)
int diex, diey;//死了的时候的位置
bool botime[400];
struct node{int x, y;};
node bomtime[400];
IMAGE i0, i1, i2, i3, i4, i5, i6, i7, i8, _i0, _i1, _i2, _i3, _i4, _i5, _i6, _i7, _i8;

void outmap(int x, int y)//输出地图
{
	IMAGE bkground;
	initgraph(x*a+2, y*a+rg+2, EW_SHOWCONSOLE);//EW_SHOWCONSOLE可以设置窗口位置
	loadimage(&bkground, L"background.jpg", x * a + 2, y * a + rg + 2);	// 读取图片到 img 对象中
	putimage(0, 0, &bkground);

	HWND hwnd = GetHWnd();// 获取窗口句柄
	SetWindowText(hwnd, L"扫雷");// 设置窗口标题文字

	rectangle(0, 0, x * a, rg);
	int i,j;
	setlinecolor(LIGHTGRAY);
	for (j = rg; j < y * a + rg; j += a)
	{
		for (i = 0; i < x * a; i += a)
		{
			rectangle(i, j, i + a, j + a);//绘制从左上角到左下角的矩形
		}
	}
	/*
	对于第(i,j)个格子，他的左上角是((i-1)*a,(j-1)*a+rg)
	*/
}

int checkground(int x, int y, int stx, int sty)
{
	if (stx == x && sty == y)return -1;
	for (int t = 0; t <= 7; t++)
	{
		if (x == stx + dx[t] && y == sty + dy[t])return -1;
	}
	return 1;
}

void placeboom(int stx,int sty,int mo)
{
	int i, px, py, j, t=0;
	memset(boom, 0, sizeof(boom));//0表示没有雷,-1表示有雷
	srand((unsigned)time(NULL));
	for (i = 1; i <= boomm; i++)
	{
		px = rand() % rgx + 1;
		py = rand() % rgy + 1;
		if (mo == 1 && i <= 30)
		{
			t = rand() % 141 + 60;//60-200s
			while(botime[t]!=0)t = rand() % 141 + 60;//60-200s
		}
		while (boom[px][py] == -1 || checkground(px,py,stx,sty)==-1)
		{
			px = rand() % rgx + 1;
			py = rand() % rgy + 1;
		}
		boom[px][py] = -1; botime[t] = 1;
		if (mo == 1 && i <= 30)
		{
			bomtime[t].x = px; bomtime[t].y = py;
		}
	}

	for (i = 1; i <= rgx; i++)
	{
		for (j = 1; j <= rgy; j++)
		{
			if (boom[i][j] == -1)numap[i][j] = -1;
			else
			{
				int t;
				for (t = 0; t <= 7; t++)
				{
					if (boom[i + dx[t]][j + dy[t]] == -1)numap[i][j]++;
				}
			}
		}
	}
}
/*
void outtextxy(int x, int y, LPCTSTR str);
其中，(x, y) 是文本输出的起始坐标，str 是要输出的字符串。
如果你想在EasyX窗口中输出数组的值，可以将数组元素转换为字符串，然后使用 outtextxy 函数输出
void outboom(int x,int y)
{
	int i, j;
    for (j = 1; j <= y; ++j)
    {
        for (i = 1; i <= x; ++i)
        {
			// std::string value = std::to_string(boom[i][j]);
			//const char* myCString = value.c_str();
			// 将数组元素转换为字符串
			TCHAR s[10];
			_stprintf(s, _T("%d"), boom[i][j]);
			//设置字体样式
			settextstyle(17, 0, _T("Consolas"));
            // 输出字符串到窗口
            outtextxy((i - 1)* a+8, (j - 1)* a + rg+6, s);
        }
    }
}
*/

int check(int x,int y)
{
	if (x >= 1 && x <= rgx && y >= 1 && y <= rgy)return 1;
	return -1;
}

void outimage(IMAGE i, IMAGE _i,int x,int y)
{
	putimage(x, y, &_i, SRCAND);
	putimage(x, y, &i, SRCPAINT);
}

int display(int x, int y)
//单击鼠标,ins表示两种形势，ins=1说明是单击，ins=2说明是假双击（遇到雷直接炸
{
	node list[4100],put[4100]; int i, j;
	int head = 1, tail = 2, t, px, py,tot=0;
	put[++tot].x = x; put[tot].y = y;

	if (numap[x][y] == -1)
	{
		diex = x; diey = y;
		return -1;
	}
	else if (numap[x][y] == 0)
	{
		list[1].x = x; list[1].y = y; 
		while (head < tail)
		{
			px = list[head].x; py = list[head].y;
			play[px][py] = 1;
			for (t = 0; t <= 7; t++)
			{
				int nx = px + dx[t], ny = py + dy[t];
				if (check(nx, ny) == -1)continue;
				if (play[nx][ny] == 1 || play[nx][ny]==2)continue;
				if (numap[nx][ny] == -1)continue;

				play[nx][ny] = 1;
				put[++tot].x = nx; put[tot].y = ny;
				if (numap[nx][ny] == 0)
				{
					list[tail].x = nx; list[tail].y = ny;
					tail++;
				}
			}
			head++;
		}
	}
	else play[x][y] = 1;

	for (i = 1; i <= tot; i++)
	{
		// std::string value = std::to_string(boom[i][j]);
		//const char* myCString = value.c_str();
		// 将数组元素转换为字符串
		//TCHAR s[10];
		px = put[i].x; py = put[i].y;
		//_stprintf(s, _T("%d"), numap[px][py]);
		//transparentimage(NULL, (px - 1) * a, (py - 1) * a + rg, &i0, BLACK);
		
		switch (numap[px][py])
		{
		case 0:putimage((px - 1) * a, (py - 1) * a + rg, &i0, SRCPAINT); break;
		case 1:outimage(i1, _i1, (px - 1) * a, (py - 1) * a + rg); break;
		case 2:outimage(i2, _i2, (px - 1) * a, (py - 1) * a + rg); break;
		case 3:outimage(i3, _i3, (px - 1) * a, (py - 1) * a + rg); break;//紫色
		case 4:outimage(i4, _i4, (px - 1) * a, (py - 1) * a + rg); break;//深灰色
		case 5:outimage(i5, _i5, (px - 1) * a, (py - 1) * a + rg); break;
		case 6:outimage(i6, _i6, (px - 1) * a, (py - 1) * a + rg); break;
		case 7:outimage(i7, _i7, (px - 1) * a, (py - 1) * a + rg); break;
		case 8:outimage(i8, _i8, (px - 1) * a, (py - 1) * a + rg); break;
		}
		//设置字体样式
		//setbkmode(TRANSPARENT);
		//settextstyle(17, 0, _T("Consolas"));
		// 输出字符串到窗口
		//outtextxy((px - 1) * a + 9, (py - 1) * a + rg + 7, s);
		Sleep(10);
	}
	return 0;
}

int l_jiesuan(int stx,int sty)
{
	printf("stx%d %d\n", stx, sty);
	int head = 1, tail = 2,px,py,nx,ny,tot=1;
	int ch[40][40];
	IMAGE bom,lose;
	
	loadimage(&bom, L"boomnew.png",a,a);	// 读取图片到 img 对象中
	memset(ch, 0, sizeof(ch));

	node list[5000], zha[5000];
	list[head].x = stx; list[head].y = sty; ch[stx][sty] = 1;
	zha[tot].x = stx; zha[tot].y = sty;

	while (head < tail)
	{
		px = list[head].x; py = list[head].y;
		for (int t = 0; t <= 7; t++)
		{
			nx = px + dx[t]; ny = py + dy[t];
			if (check(nx, ny) == -1||ch[nx][ny]==1)continue;
			if (numap[nx][ny] == -1) { tot++; zha[tot].x = nx; zha[tot].y = ny; }
			list[tail].x = nx; list[tail].y = ny; tail++;
			ch[nx][ny] = 1;
		}
		head++;
	}

	mciSendString(L"open boom.mp3 alias music", nullptr, 0, nullptr);
	mciSendString(L"play music", nullptr, 0, nullptr);
	for (int i = 1; i <= tot; i++)
	{
		px = zha[i].x; py = zha[i].y;
		//printf("%d %d\n", px, py);
		putimage((px - 1) * a, (py - 1) * a + rg, &bom, SRCPAINT);//SRCPAINT调整导入图片与背景融合的方式
		//setrop2(R2_COPYPEN);
		Sleep(120);
	}
	mciSendString(L"close music", nullptr, 0, nullptr);
	MOUSEMSG pla; pla = GetMouseMsg();
	while(pla.uMsg != WM_LBUTTONDOWN)pla = GetMouseMsg();
	closegraph();
	
	initgraph(638,638);
	HWND hwnd = GetHWnd();
	SetWindowPos(hwnd, 0, 100 ,100, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	//SetWindowPos 函数用于设置窗口的位置为 (100, 100),
	//SWP_NOSIZE 和 SWP_NOZORDER 标志用于保持窗口的大小不变和不改变窗口的 Z 顺序。
	SetWindowText(hwnd, L"你输了");// 设置窗口标题文字
	loadimage(&lose, L"lose.png");	// 读取图片到 img 对象中
	putimage(0, 0, &lose);
	mciSendString(L"open lose.mp3 alias music", nullptr, 0, nullptr);
	mciSendString(L"play music", nullptr, 0, nullptr);
	
	while (1)
	{
		pla = GetMouseMsg();
		if (pla.uMsg == WM_LBUTTONDOWN)
		{
			if (pla.x >= 480 && pla.x <= 614 && pla.y >= 347 && pla.y <= 393)
			{
				closegraph();
				mciSendString(L"close music", nullptr, 0, nullptr);
				return 1;
			}
			if (pla.x >= 484 && pla.x <= 610 && pla.y >= 458 && pla.y <= 499)
			{
				mciSendString(L"close music", nullptr, 0, nullptr);
				closegraph();
				return -1; 
			}
		}
	}
}

int checkwin()
{
	for (int i = 1; i <= rgx; i++)
	{
		for (int j = 1; j <= rgy; j++)
		{
			if (play[i][j] == 2 && numap[i][j] != -1)return -1;
		}
	}
	return 1;
}

int w_jiesuan(int mo)
{
	IMAGE win;
	MOUSEMSG pla;
	pla = GetMouseMsg();
	while (pla.uMsg != WM_LBUTTONDOWN)pla = GetMouseMsg();
	closegraph();

	initgraph(1080, 1080);
	HWND hwnd = GetHWnd();
	SetWindowPos(hwnd, 0, 100, 100, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	//SetWindowPos 函数用于设置窗口的位置为 (100, 100),
	//SWP_NOSIZE 和 SWP_NOZORDER 标志用于保持窗口的大小不变和不改变窗口的 Z 顺序。
	SetWindowText(hwnd, L"你赢了");// 设置窗口标题文字
	loadimage(&win, L"win.jpg");	// 读取图片到 img 对象中
	putimage(0, 0, &win);
	mciSendString(L"open win.mp3 alias music", nullptr, 0, nullptr);
	mciSendString(L"play music", nullptr, 0, nullptr);

	while (1)
	{
		pla = GetMouseMsg();
		if (pla.uMsg == WM_LBUTTONDOWN)
		{
			if (pla.x >= 815 && pla.x <= 1029 && pla.y >= 657 && pla.y <= 732)
			{
				closegraph(); mciSendString(L"close music", nullptr, 0, nullptr);
				return 1;
			}
			if (pla.x >= 821 && pla.x <= 1034 && pla.y >= 825 && pla.y <= 929)
			{
				mciSendString(L"close music", nullptr, 0, nullptr);
				closegraph(); return -1; 
			}
		}
	}
}

void begin()
{
	IMAGE start; MOUSEMSG pla;
	loadimage(&start, L"start.jpg");
	initgraph(638, 638);
	HWND hwnd = GetHWnd();
	SetWindowPos(hwnd, 0, 100, 100, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	//SetWindowPos 函数用于设置窗口的位置为 (100, 100),
	//SWP_NOSIZE 和 SWP_NOZORDER 标志用于保持窗口的大小不变和不改变窗口的 Z 顺序。
	SetWindowText(hwnd, L"开始");// 设置窗口标题文字
	putimage(0, 0, &start);
	while (1)
	{
		pla = GetMouseMsg();
		if (pla.uMsg == WM_LBUTTONDOWN)
		{
			if (pla.x >= 380 && pla.x <= 613)
			{
				if (pla.y >= 397 && pla.y <= 426) { rgx = 9; rgy = 9; boomm = 10; rg = 70; break; }
				if (pla.y >= 483 && pla.y <= 512) { rgx = 16; rgy = 16; boomm = 40; rg = 120; break; }
				if (pla.y >= 565 && pla.y <= 590) { rgx = 30; rgy = 16; boomm = 99; rg = 120; break; }
			}
		}
	}
	closegraph();
}

void autoboom(int t)
{
	int px, py,nx,ny;
	px = bomtime[t].x; py = bomtime[t].y;
	if (play[px][py] == 2)return;
	IMAGE bom;
	loadimage(&bom, L"boomnew.png", a, a);	// 读取图片到 img 对象中
	putimage((px - 1) * a, (py - 1) * a + rg, &bom, SRCPAINT);//SRCPAINT调整导入图片与背景融合的方式
	for (int t = 0; t <= 7; t++)
	{
		nx = px + dx[t]; ny = py + dy[t];
		if (check(nx, ny) == -1 || numap[nx][ny] == -1)continue;
		display(nx, ny);
	}
}

int main()
{
	MOUSEMSG playm;
	IMAGE flig, epyig, backg1,backg2;
	int ch, px, py, tot, leftb,l_jie=1,w_jie=1;//tot标记第一次按雷

	loadimage(&i0, L"0.png", a, a);
	loadimage(&i1, L"1.png", a, a); loadimage(&_i1, L"01.png", a, a);
	loadimage(&i2, L"2.png", a, a); loadimage(&_i2, L"02.png", a, a);
	loadimage(&i3, L"3.png", a, a); loadimage(&_i3, L"03.png", a, a);
	loadimage(&i4, L"4.png", a, a); loadimage(&_i4, L"04.png", a, a);
	loadimage(&i5, L"5.png", a, a); loadimage(&_i5, L"05.png", a, a);
	loadimage(&i6, L"6.png", a, a); loadimage(&_i6, L"06.png", a, a);
	loadimage(&i7, L"7.png", a, a); loadimage(&_i7, L"07.png", a, a);
	loadimage(&i8, L"8.png", a, a); loadimage(&_i8, L"08.png", a, a);
	
	while (1)
	{
		//初始化
		FlushMouseMsgBuffer();
		begin();
		ch = 1; tot = 1; leftb = boomm;
		memset(botime, 0, sizeof(botime));
		memset(play, 0, sizeof(play));//0表示没开1表示开了2表示标记
		memset(numap, 0, sizeof(numap));
		outmap(rgx, rgy);
		DWORD startTime = GetTickCount(); // 获取起始时间
		//输出剩余雷数
		getimage(&backg1, (rgx / 3 * 2) * a, rg / 3, rgx * a, rg / 3 * 2);
		getimage(&backg2, 2, rg / 3, rgx*a/2, rg / 3);
		settextstyle(25, 0, _T("Consolas"));//设置字体样式
		setbkmode(TRANSPARENT);
		settextcolor(BLACK);
		outtextxy((rgx / 3 * 2) * a, rg / 3, ((std::to_wstring(leftb)) + L"/" + (std::to_wstring(boomm))).c_str());

		while (ch != -1)
		{
			DWORD currentTime = GetTickCount(), elapsedTime; // 获取当前时间
			if ((currentTime - startTime) % 1000 == 0)
			{
				elapsedTime = (currentTime - startTime) / 1000;
				if (boomm == 99 && elapsedTime <= 200 && botime[elapsedTime] == 1)
				{
					autoboom(elapsedTime);
					boomm--; leftb--;
					putimage((rgx / 3 * 2) * a, rg / 3, &backg1);
					settextstyle(25, 0, _T("Consolas"));//设置字体样式
					settextcolor(BLACK);
					outtextxy((rgx / 3 * 2) * a, rg / 3, ((std::to_wstring(leftb)) + L"/" + (std::to_wstring(boomm))).c_str());
				}
				settextstyle(25, 0, _T("Consolas"));//设置字体样式
				putimage(2, rg / 3, &backg2);
				settextcolor(BLACK);
				outtextxy(2, rg / 3, (L"Time: " + (std::to_wstring(elapsedTime))).c_str());// 输出字符串到窗口
			}

			if (MouseHit())
			{
				playm = GetMouseMsg();
				px = int(playm.x / a) + 1;
				py = int((playm.y - rg) / a) + 1;
				//对于第(i, j)个格子，他的左上角是((i - 1) * a, (j - 1) * a + rg)
				if (playm.y >= rg)
				{
					if (playm.uMsg == WM_LBUTTONDOWN)//左键按下
					{
						if (tot == 1)
						{
							if (boomm == 99) placeboom(px, py, 1); 
							else placeboom(px, py, 2);
							tot = -1;
						}//初始化地图

						if (play[px][py] == 0)ch = display(px, py);
						else
						{
							if (numap[px][py] >= 1 && numap[px][py] <= 9)
							{
								int temp = 0, nx, ny;
								for (int t = 0; t <= 7; t++)
								{
									nx = px + dx[t]; ny = py + dy[t];
									if (play[nx][ny] == 2)temp++;
								}
								if (temp == numap[px][py])
								{
									for (int t = 0; t <= 7; t++)
									{
										nx = px + dx[t]; ny = py + dy[t];
										if (play[nx][ny] == 2 || check(nx, ny) == -1)continue;
										if (numap[nx][ny] == -1) { diex = nx; diey = ny; ch = -1; break; }
										ch = display(nx, ny);
									}
								}
							}
						}
					}
					else if (playm.uMsg == WM_RBUTTONDOWN)
					{
						if (play[px][py] == 0)
						{
							getimage(&epyig, (px - 1) * a, (py - 1) * a + rg, a, a);
							loadimage(&flig, L"flag1.jpg", a, a);	// 读取图片到 img 对象中
							putimage((px - 1) * a, (py - 1) * a + rg, &flig);
							play[px][py] = 2;

							leftb--;
							putimage((rgx / 3 * 2) * a, rg / 3, &backg1);
							settextstyle(25, 0, _T("Consolas"));//设置字体样式
							setbkmode(TRANSPARENT);
							settextcolor(BLACK);
							outtextxy((rgx / 3 * 2) * a, rg / 3, ((std::to_wstring(leftb)) + L"/" + (std::to_wstring(boomm))).c_str());

						}
						else if (play[px][py] == 2)
						{
							putimage((px - 1) * a, (py - 1) * a + rg, &epyig);
							play[px][py] = 0;


							leftb++;
							putimage((rgx / 3 * 2) * a, rg / 3, &backg1);
							settextstyle(25, 0, _T("Consolas"));//设置字体样式
							setbkmode(TRANSPARENT);
							settextcolor(BLACK);
							outtextxy((rgx / 3 * 2) * a, rg / 3, ((std::to_wstring(leftb)) + L"/" + (std::to_wstring(boomm))).c_str());

						}
					}
				}
			}
			if (leftb == 0)
			{
				if (checkwin() == 1) { w_jie = w_jiesuan(1); break; }
			}
		}
		if (ch == -1)
		{
			l_jie = l_jiesuan(diex, diey);
			if (l_jie == -1)break;
		}
		if (w_jie == -1)break;
	}
	_getch();
	closegraph();
	return 0;
}