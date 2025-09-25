#include <windows.h> 
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#define MAX_GOLD 10
#define GOLD_SIZE 20
#define GROUND_Y 250
#define MAX_LIFE 5
#define ITEM_SIZE 15

typedef struct { int x,y; BOOL active; DWORD spawnTime; BOOL isLifeItem; } Gold;
typedef enum { MENU, GAME, SLIDE } State;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

int score=0, explosions=0, highScore=0;
int playerX=150, playerY=270, level=1;
int life=MAX_LIFE;
Gold golds[MAX_GOLD];
State state=MENU;
HWND btnStart, btnRestart;

int windowWidth=320, windowHeight=300;

void SpawnGold(){
    for(int i=0;i<MAX_GOLD;i++){
        if(!golds[i].active){
            golds[i].x = rand()%(windowWidth-GOLD_SIZE);
            golds[i].y = 0;
            golds[i].active = TRUE;
            golds[i].spawnTime = GetTickCount();
            golds[i].isLifeItem = (rand()%10==0); // 10% chance เป็นไอเท็มเพิ่มพลัง
            break;
        }
    }
}

void ResetGame(){
    score=explosions=0; 
    level=1; 
    life=MAX_LIFE;
    playerX=windowWidth/2; 
    for(int i=0;i<MAX_GOLD;i++) golds[i].active=FALSE;
}

void HideAllButtons(){
    if(btnStart) ShowWindow(btnStart,SW_HIDE); 
    if(btnRestart) ShowWindow(btnRestart,SW_HIDE);
}

void CreateMainMenu(HWND hwnd){
    btnStart=CreateWindowW(L"BUTTON",L"เริ่มเกม",WS_VISIBLE|WS_CHILD,110,80,100,30,hwnd,(HMENU)1,NULL,NULL);
}

void ShowRestart(HWND hwnd){
    btnRestart=CreateWindowW(L"BUTTON",L"กลับหน้าแรก",WS_VISIBLE|WS_CHILD,100,200,120,30,hwnd,(HMENU)2,NULL,NULL);
}

COLORREF GetBGColor(){
    switch(level){
        case 1: return RGB(0,0,0);
        case 2: return RGB(128,128,128);
        case 3: return RGB(255,255,255);
        case 4: return RGB(255,0,0);
    }
    return RGB(0,0,0);
}

void UpdateLevel(){
    if(score>=0 && score<5) level=1;
    else if(score>=5 && score<10) level=2;
    else if(score>=10 && score<20) level=3;
    else if(score>=20) level=4;
}

int WINAPI wWinMain(HINSTANCE hI,HINSTANCE hP,PWSTR lp,int nS){
    srand(time(NULL));
    const wchar_t CLASS_NAME[] = L"MyGame";
    WNDCLASS wc={0};
    wc.lpfnWndProc=WndProc; wc.hInstance=hI; wc.lpszClassName=CLASS_NAME; wc.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
    RegisterClass(&wc);
    HWND hwnd=CreateWindowEx(0,CLASS_NAME,L"เกมเก็บทองๆๆๆ",WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,windowWidth,windowHeight,NULL,NULL,hI,NULL);
    if(!hwnd) return 0;
    ShowWindow(hwnd,SW_SHOWNORMAL); UpdateWindow(hwnd);
    CreateMainMenu(hwnd);
    SetTimer(hwnd,1,30,NULL);
    MSG msg={0};
    while(GetMessage(&msg,NULL,0,0)){TranslateMessage(&msg); DispatchMessage(&msg);}
    return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd,UINT msg,WPARAM w,LPARAM l){
    switch(msg){
        case WM_PAINT:{
            PAINTSTRUCT ps; HDC hdc=BeginPaint(hwnd,&ps);
            RECT r; GetClientRect(hwnd,&r);

            HFONT font=CreateFontW(20,0,0,0,FW_BOLD,0,0,0,0,0,0,0,0,L"Tahoma");
            SelectObject(hdc,font); SetBkMode(hdc,TRANSPARENT);

            if(state==MENU){
                HBRUSH bg=CreateSolidBrush(RGB(255,255,255)); FillRect(hdc,&r,bg); DeleteObject(bg);
                SetTextColor(hdc,RGB(0,0,0));
                TextOutW(hdc,50,30,L"เกมเก็บทองๆๆๆ",17);
                wchar_t footer[50]; wsprintfW(footer,L"ผู้ผลิต โดย เอ็กซ์ 2025"); TextOutW(hdc,50,260,footer,lstrlenW(footer));
            }
            else if(state==GAME){
                HBRUSH bg=CreateSolidBrush(GetBGColor()); FillRect(hdc,&r,bg); DeleteObject(bg);
                SetTextColor(hdc,RGB(255,255,255)); TextOutW(hdc,playerX,playerY,L"X",1);
                for(int i=0;i<MAX_GOLD;i++){
                    if(golds[i].active){
                        HBRUSH g;
                        if(golds[i].isLifeItem) g = CreateSolidBrush(RGB(0,255,0)); // ไอเท็มเพิ่มพลังเป็นสีเขียว
                        else g = CreateSolidBrush(RGB(255,215,0));
                        HBRUSH o=(HBRUSH)SelectObject(hdc,g); 
                        Ellipse(hdc,golds[i].x,golds[i].y,golds[i].x+GOLD_SIZE,golds[i].y+GOLD_SIZE); 
                        SelectObject(hdc,o); 
                        DeleteObject(g);
                    }
                }
                UpdateLevel();
                wchar_t info[50]; 
                wsprintfW(info,L"Score:%d Level:%d",score,level); 
                SetTextColor(hdc,RGB(255,215,0)); 
                TextOutW(hdc,10,10,info,lstrlenW(info));

                // วาดขีดพลังชีวิต
                SetTextColor(hdc,RGB(255,0,0));
                for(int i=0;i<life;i++){
                    Rectangle(hdc,10 + i*22,30,30 + i*22,50);
                }
            }
            else if(state==SLIDE){
                HBRUSH b=CreateSolidBrush(RGB(0,0,0)); FillRect(hdc,&r,b); DeleteObject(b);
                SetTextColor(hdc,RGB(255,255,255));
                TextOutW(hdc,20,100,L"เกมจบ! กลับหน้าเมนู",wcslen(L"เกมจบ! กลับหน้าเมนู"));
            }

            DeleteObject(font);
            EndPaint(hwnd,&ps);
        } break;

        case WM_COMMAND:{
            switch(LOWORD(w)){
                case 1: // เริ่มเกม
                    HideAllButtons(); 
                    state=GAME; 
                    ResetGame(); 
                    InvalidateRect(hwnd,NULL,TRUE);
                    break;

                case 2: // ปุ่มกลับหน้าแรก
                    HideAllButtons(); 
                    state=MENU; 
                    CreateMainMenu(hwnd); 
                    InvalidateRect(hwnd,NULL,TRUE);
                    break;
            }
        } break;

        case WM_TIMER:{
            if(state==GAME){
                DWORD now=GetTickCount();
                for(int i=0;i<MAX_GOLD;i++){
                    if(golds[i].active){
                        golds[i].y+=2; 
                        if(golds[i].y>GROUND_Y && now-golds[i].spawnTime>=3000){
                            if(golds[i].isLifeItem){ // ไม่เสียชีวิต ถ้าเป็นไอเท็ม
                                score++;
                            } else {
                                life--;
                            }
                            golds[i].active=FALSE;
                        }
                    }
                }
                if(rand()%10<2) SpawnGold();
                InvalidateRect(hwnd,NULL,TRUE);

                if(life<=0){ // GAME OVER
                    if(score>highScore) highScore=score;
                    HideAllButtons();
                    state=SLIDE;
                    ShowRestart(hwnd);
                    InvalidateRect(hwnd,NULL,TRUE);
                }
            }
        } break;

        case WM_LBUTTONDOWN:{
            if(state==GAME){
                int mx=LOWORD(l), my=HIWORD(l);
                for(int i=0;i<MAX_GOLD;i++){
                    if(golds[i].active && mx>=golds[i].x && mx<=golds[i].x+GOLD_SIZE && my>=golds[i].y && my<=golds[i].y+GOLD_SIZE){
                        if(golds[i].isLifeItem){
                            if(life<MAX_LIFE) life++; // เก็บไอเท็มเพิ่มพลัง
                        } else score++;
                        golds[i].active=FALSE;
                    }
                }
            }
        } break;

        case WM_DESTROY: PostQuitMessage(0); return 0;
    }
    return DefWindowProc(hwnd,msg,w,l);
}
