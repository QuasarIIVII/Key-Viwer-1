/*
Compelete at 06.13.2020 - 08:00
MAde by Quasar27
Q:\C\Dev C++\Key Input Info\KeyCount.data
*/
#include<windows.h>
#include<string>
#include<algorithm>
#include<stdlib.h>
#include<time.h>
#include<stdio.h>
#include"resource.h"

using namespace std;
using std::string;
#define round(x) ((int)( (x) < (int)(x)+0.5 ? (int)(x) : (int)(x)+1 ))
HINSTANCE hinst;
RECT rectDesktop;

typedef struct key{
	int vkey, order;
}key;
typedef union keyCount{
	UINT64 src[256];//for count
	INT8 byte[2048];//for IO
}keyCount;
keyCount keyCnt;//Key Count
void defKeyLabel();

int nkey,h=0,w=0,R=80, statH=0,statR=40, keyViewH=0, keyViewR=40;//nkey : auto
//int rsk, rek; //rs[e]k : Record Start[End] Key 
char*kcPath="C:\\Key Input Info\\KeyCount.data";
//string kcPath="Q:\\C\\Dev C++\\Key Input Info\\KeyCount.data";
char* kcPathDir;
char*keyLabel[256]={0,};
bool keyState[256], ks;//ks : Key State
char tmpstr[32];
clock_t curCk[256];
key keyset[]={{'E'},{'F'},{'J'},{'I'},{'O'}};
//key keyset[]={{'H',0},{'J',1}};
int h0e,h1s,h1e,h2s,h2e;

HANDLE hKCF; //h1andle key count file

LARGE_INTEGER li={0,0};
#ifdef Yee
LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	static HPEN hopen;
	static HBRUSH hohrush;
	switch(Message) {
		case WM_PAINT:{
			BeginPaint(hwnd,&ps);
			
			hopen=(HPEN)SelectObject(ps.hdc,GetStockObject(DC_PEN));
			hohrush=(HBRUSH)SelectObject(ps.hdc,GetStockObject(DC_BRUSH));
			
			SetDCPenColor(ps.hdc,0x000000);
			SetDCBrushColor(ps.hdc,0x000000);
			
			if(Rectangle(ps.hdc,50,50,52,52)){
				SetDCPenColor(ps.hdc,0x0000FF);
				SetDCBrushColor(ps.hdc,0x0000FF);
				Rectangle(ps.hdc,75,75,100,100);
			}
			
			DeleteObject(SelectObject(ps.hdc,hopen));
			DeleteObject(SelectObject(ps.hdc,hohrush));
			
			EndPaint(hwnd,&ps);
			break;
		}
		/* Upon destruction, tell the main thread to stop */
		case WM_DESTROY: {
			PostQuitMessage(0);
			break;
		}
		
		/* All other messages (a lot of them) are processed using default procedures */
		default:
			return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}
#endif//Yee
LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	static char strbuf[256];strbuf[0]=0;
	
	PAINTSTRUCT ps;
	static int i,c,tot=0;
	static HDC hdc, hfdc;//hfdc : DC for drawing main font
	static HPEN hopen;
	static HBRUSH hobrush;
	static HFONT hoffont, hofont;
	static RECT rect;
	static unsigned int cur=0,pcurCk=0,pcurCkR=0;//pcurCk : current clock pointer, pcurCkR : current clock pointer for Release
	static double statW=(((double)w+R)*nkey-nkey)/3;
	
	static time_t tim;
	static tm TM;
	static int sec,min,hour,lastSec;
	
	static auto color=[&hdc](COLORREF Pen, COLORREF Brush)->void{SetDCPenColor(hdc, Pen),SetDCBrushColor(hdc, Brush);};
	
	static auto detectKey=[&](){
		bool keyStateC=false;//is Key State Changed?
		char keyStr[255]="";
		
		for(i=c=0;i<256;i++){//Detect Key
			if(i==keyset[c].vkey){
				if((ks = GetAsyncKeyState(i)&0x8000)!=keyState[i]){
//				ks = GetAsyncKeyState(i)&0x8000;
					if(ks){//Hold key
						keyCnt.src[i]++;
						tot++;
						//draw key
						color(0xE8E826, 0x808080);
						RoundRect(hdc,(w+R)*keyset[c].order-keyset[c].order, h1s, (w+R)*(keyset[c].order+1)-keyset[c].order, h1e, R, R);
						SetRect(&rect,(w+R)*keyset[c].order-keyset[c].order, h1s+(h1e-h1s)/6, (w+R)*(keyset[c].order+1)-keyset[c].order, h1e);
						DrawText(hfdc,keyLabel[i],-1,&rect,DT_CENTER);
						
						SetRect(&rect,(w+R)*keyset[c].order-keyset[c].order, h1s+(h1e-h1s)/2, (w+R)*(keyset[c].order+1)-keyset[c].order, h1e);
						DrawText(hdc,itoa(keyCnt.src[i],tmpstr,10),-1,&rect,DT_CENTER);
						
						color(0xE8E826, 0xFFFFFF);
						RoundRect(hdc, (w+R)*nkey-nkey - round(statW),0,(w+R)*nkey-nkey, h0e, statR, statR);
						
						//draw Total
						RoundRect(hdc, (w+R)*nkey-nkey - round(statW),0,(w+R)*nkey-nkey, h0e, statR, statR);
						SetRect(&rect, (w+R)*nkey-nkey - round(statW),0,(w+R)*nkey-nkey, h0e);
						DrawText(hdc,"Tot",-1,&rect,DT_CENTER);
						SetRect(&rect, (w+R)*nkey-nkey - round(statW),h0e/2,(w+R)*nkey-nkey, h0e);
						DrawText(hdc,itoa(tot,tmpstr,10),-1,&rect,DT_CENTER);
						
						//process Current (+)
						curCk[pcurCk++]=clock();
						if(pcurCk==256)pcurCk=0;

						//draw Current
						RoundRect(hdc, round(statW),0,(w+R)*nkey-nkey - round(statW) , h0e, statR, statR);
						SetRect(&rect, round(statW),0,(w+R)*nkey-nkey - round(statW) , h0e);
						DrawText(hdc,"Cur",-1,&rect,DT_CENTER);
						SetRect(&rect, round(statW),h0e/2,(w+R)*nkey-nkey - round(statW) , h0e);
						DrawText(hdc,itoa(++cur,tmpstr,10),-1,&rect,DT_CENTER);
					}else{//Release key
						color(0xE8E826, 0xFFFFFF);
						RoundRect(hdc,(w+R)*keyset[c].order-keyset[c].order, h1s, (w+R)*(keyset[c].order+1)-keyset[c].order, h1e, R, R);
						SetRect(&rect,(w+R)*keyset[c].order-keyset[c].order, h1s+(h1e-h1s)/6, (w+R)*(keyset[c].order+1)-keyset[c].order, h1e);
						DrawText(hfdc,keyLabel[i],-1,&rect,DT_CENTER);
						
						SetRect(&rect,(w+R)*keyset[c].order-keyset[c].order, h1s+(h1e-h1s)/2, (w+R)*(keyset[c].order+1)-keyset[c].order, h1e);
						DrawText(hdc,itoa(keyCnt.src[i],tmpstr,10),-1,&rect,DT_CENTER);
					}
					keyState[i]=!keyState[i];
				}
				c++;
			}else{
				if((ks = GetAsyncKeyState(i)&0x8000)/*!=keyState[i]*/){
					if(ks){
						if(keyLabel[i]){
							sprintf(keyStr,"%s%s ",keyStr,keyLabel[i]);
//							strcat(keyStr,keyLabel[i]);
						}else{
							sprintf(keyStr,"%s%02X ",keyStr,i);
						}
					}else{
						
					}
					keyStateC=true;
//					keyState[i]=!keyState[i];
				}
			}
		}
		
//		if(keyStateC){
			SetRect(&rect,keyViewR/2, h2s, (w+R)*nkey-nkey-(keyViewR/2), h2e);
			RoundRect(hdc,0, h2s, (w+R)*nkey-nkey, h2e, keyViewR, keyViewR);
			DrawText(hdc,keyStr,-1,&rect,DT_LEFT|DT_WORDBREAK);
//		}
	};
	
	switch(Message) {
		case WM_TIMER:{
			switch(wParam){
				case 1:{
					detectKey();
					if(cur)if(CLOCKS_PER_SEC <= clock()-curCk[pcurCkR]){//Release Cur
						RoundRect(hdc, round(statW),0,(w+R)*nkey-nkey - round(statW) , h0e, statR, statR);
						SetRect(&rect, round(statW),0,(w+R)*nkey-nkey - round(statW) , h0e);
						DrawText(hdc,"Cur",-1,&rect,DT_CENTER);
						SetRect(&rect, round(statW),h0e/2,(w+R)*nkey-nkey - round(statW) , h0e);
						DrawText(hdc,itoa(--cur,tmpstr,10),-1,&rect,DT_CENTER);
						
						if(++pcurCkR == 256)pcurCkR=0;
					}
					break;
				}
				case 2:{
					time(&tim);
					TM=*localtime(&tim);
					
					if(TM.tm_sec^lastSec){
						RoundRect(hdc, 0,0,round(statW), h0e, statR, statR);
						SetRect(&rect, 0,0,round(statW), h0e);
						DrawText(hdc,"Time",-1,&rect,DT_CENTER);
						
						SetRect(&rect, 0,h0e/2,round(statW), h0e);
						sprintf(strbuf,"%02d:%02d:%02d",TM.tm_hour,TM.tm_min,lastSec=TM.tm_sec);
						DrawText(hdc,strbuf,-1,&rect,DT_CENTER);
					}
					
					break;
				}
			}
			break;
		}
		case WM_PAINT:{
			BeginPaint(hwnd,&ps);
			color(0xE8E826, 0xFFFFFF);
			//Draw Frame
			RoundRect(hdc, 0,0,round(statW), h0e, statR, statR);
			SetRect(&rect, 0,0,round(statW), h0e);
			DrawText(hdc,"Time",-1,&rect,DT_CENTER);
			
			RoundRect(hdc, round(statW),0,(w+R)*nkey-nkey - round(statW) , h0e, statR, statR);
			SetRect(&rect, round(statW),0,(w+R)*nkey-nkey - round(statW) , h0e);
			DrawText(hdc,"Cur",-1,&rect,DT_CENTER);
			SetRect(&rect, round(statW),h0e/2,(w+R)*nkey-nkey - round(statW) , h0e);
			DrawText(hdc,itoa(cur,tmpstr,10),-1,&rect,DT_CENTER);
			
			RoundRect(hdc, (w+R)*nkey-nkey - round(statW),0,(w+R)*nkey-nkey, h0e, statR, statR);
			SetRect(&rect, (w+R)*nkey-nkey - round(statW),0,(w+R)*nkey-nkey, h0e);
			DrawText(hdc,"Tot",-1,&rect,DT_CENTER);
			SetRect(&rect, (w+R)*nkey-nkey - round(statW),h0e/2,(w+R)*nkey-nkey, h0e);
			DrawText(hdc,itoa(tot,tmpstr,10),-1,&rect,DT_CENTER);
			
//			RoundRect(hdc,0, 0, (w+R)*nkey-nkey, statH+statR, statR, statR);
			for(i=0;i<nkey;i++){
				RoundRect(hdc,(w+R)*i-i, h1s, (w+R)*(i+1)-i, h1e, R, R);
				SetRect(&rect,(w+R)*i-i, h1s+(h1e-h1s)/6, (w+R)*(i+1)-i, h1e);
				DrawText(hfdc,keyLabel[keyset[i].vkey],-1,&rect,DT_CENTER);
			}

			detectKey();
//			RoundRect(hdc,0, h2s, (w+R)*nkey-nkey, h2e, keyViewR, keyViewR);
			
			EndPaint(hwnd,&ps);
			break;
		}
		case WM_NCHITTEST:{
			UINT nHit=DefWindowProc(hwnd,Message,wParam,lParam);
			if(nHit==HTCLIENT)nHit=HTCAPTION;
			return nHit;
		}
//		CreateRectRgn
		
		case WM_CREATE:{
			for(i=0;i<256;i++){
				keyState[i]=true;
				tot+=keyCnt.src[i];
			}
			
			SetLayeredWindowAttributes(hwnd,RGB(0xFF,0x00, 0xFF), 0x80, LWA_COLORKEY|LWA_ALPHA);
			
			//Setup DC
			hdc=GetDC(hwnd);
			hfdc=GetDC(hwnd);
			
			hopen=(HPEN)SelectObject(hdc,GetStockObject(DC_PEN));
			hobrush=(HBRUSH)SelectObject(hdc,GetStockObject(DC_BRUSH));
			hoffont=(HFONT)SelectObject(hfdc,CreateFont((h1e-h1s)/3,0,0,0,300,0,0,0,ANSI_CHARSET,false,false,false,VARIABLE_PITCH|FF_ROMAN,"Castellar"));
			hofont=(HFONT)SelectObject(hdc,CreateFont(h0e/2,0,0,0,300,0,0,0,ANSI_CHARSET,false,false,false,VARIABLE_PITCH|FF_ROMAN,TEXT("Consolas")));

			SetBkMode(hfdc,1);
			SetBkMode(hdc,1);

			SetTimer(hwnd,1,10,NULL);
			SetTimer(hwnd,2,50,NULL);
			break;
		}
		case WM_DESTROY: {
			KillTimer(hwnd,256);
			
			DeleteObject(SelectObject(hdc,hopen));
			DeleteObject(SelectObject(hdc,hobrush));
			DeleteObject(SelectObject(hfdc,hoffont));
			DeleteObject(SelectObject(hdc,hofont));
			
			ReleaseDC(hwnd,hdc);
			ReleaseDC(hwnd,hfdc);
			
			SetFilePointerEx(hKCF,li,NULL,FILE_BEGIN);
			WriteFile(hKCF,keyCnt.byte,2048,NULL,NULL);
			CloseHandle(hKCF);

			PostQuitMessage(0);
			break;
		}
		
		default:
			return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	
	DefWindowProc(hwnd, Message, wParam, lParam);
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	hinst=hInstance;
	{//setup
		int i=0,j;
		for(i=0;i<sizeof(keyset)/sizeof(key);i++)keyset[i].order=i;//keyset numbering
	}
	
	HWND hwndDesktop=GetDesktopWindow();
	GetWindowRect(hwndDesktop,&rectDesktop);
	
	nkey=sizeof(keyset)/sizeof(keyset[0]);
//	if(!nkey || !(w+R) || !(h+R)){
//		MessageBox(NULL, "Input value is wrong.","Wrong Input",MB_ICONEXCLAMATION|MB_OK);
//		return 0;
//	}
	WNDCLASSEX wc;
	HWND hwnd;
	MSG msg;

	memset(&wc,0,sizeof(wc));
	wc.cbSize		 = sizeof(WNDCLASSEX);
	wc.lpfnWndProc	 = WndProc;
	wc.hInstance	 = hInstance;
	wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);
	
	wc.hbrBackground = CreateSolidBrush(0xFF00FF);
	wc.lpszClassName = "WindowClass";
	wc.hIcon		 = LoadIcon(hinst, MAKEINTRESOURCE(IDI_ICON1));
	wc.hIconSm		 = LoadIcon(hinst, MAKEINTRESOURCE(IDI_ICON1));

	if(!RegisterClassEx(&wc)) {
		MessageBox(NULL, "Window Registration Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
		return 0;
	}
	
	//Get Key Count Data from File
	if((hKCF=CreateFile(kcPath,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL))==INVALID_HANDLE_VALUE){
		if(!CreateDirectory("C:\\Key Input Info",NULL)){
			MessageBox(NULL, "Cannot open file: \"KeyCount.data\"!","Error!",MB_ICONEXCLAMATION|MB_OK);
			return 0;
		}else{
			if((hKCF=CreateFile(kcPath,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL))==INVALID_HANDLE_VALUE){
				MessageBox(NULL, "Cannot open file: \"KeyCount.data\"!","Error!",MB_ICONEXCLAMATION|MB_OK);
				return 0;
			}
		}
	}
	
	DWORD temp;
	ReadFile(hKCF,keyCnt.byte,2048,&temp,NULL);
	if(temp!=2048){
		unsigned char default_data[2048]={0,};
		SetFilePointerEx(hKCF,li,NULL,FILE_BEGIN);
		WriteFile(hKCF,default_data,2048,&temp,NULL);
		for(int i=0;i<256;i++)keyCnt.src[i]=0;
	}
	
	sort(keyset, keyset+nkey, [](key a,key b){return a.vkey<b.vkey;});
	
	h0e=statH+statR;
	h1s=h0e?h0e-1:0;
	h1e=h1s+h+R;
	h2s=h1e?h1e-1:0;
	h2e=h2s+keyViewH+keyViewR;

	defKeyLabel();

	hwnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_LAYERED,"WindowClass","KII",WS_VISIBLE|WS_POPUP,
		0,
		rectDesktop.bottom-(h2e+1),
		(w+R)*nkey-nkey+1,
		h2e+1,
		NULL,NULL,hInstance,NULL);

	if(hwnd == NULL) {
		char buf[256];
		sprintf(buf, "Window Creation Failed! Y = %d, W = %d, H = %d",rectDesktop.bottom-(h2e+1), (w+R)*nkey-nkey+1,h2e+1);
		MessageBox(NULL,buf ,"Error!",MB_ICONEXCLAMATION|MB_OK);
		
		return 0;
	}
	
//	keyLabel[255]=(char*)calloc(5,1);
//	keyLabel[255][0]=h0e/10+0x30;
//	keyLabel[255][1]=h0e%10+0x30;
//	MessageBox(NULL,keyLabel[255],"",MB_OK);
	
	while(GetMessage(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}
void defKeyLabel(){//define keyLabel
	int charsz=sizeof(char),t,i;
//	VK_CLEAR
	keyLabel[0x01]=(char*)"BL";
	keyLabel[0x02]=(char*)"BR";
	keyLabel[0x03]=(char*)"Cancel";
	keyLabel[0x04]=(char*)"BM";
	keyLabel[0x05]=(char*)"X1";
	keyLabel[0x06]=(char*)"X2";
	keyLabel[0x08]=(char*)"Bs";
	keyLabel[0x09]=(char*)"Tab";
	keyLabel[0x0C]=(char*)"Clear";
	keyLabel[0x0D]=(char*)"Enter";
	
	keyLabel[0x10]=(char*)"";
	keyLabel[0x11]=(char*)"";
	keyLabel[0x12]=(char*)"";
	
	keyLabel[0x13]=(char*)"Pause";
	keyLabel[0x14]=(char*)"Capital";
	keyLabel[0x15]=(char*)"Hangul";
	keyLabel[0x17]=(char*)"Junja";
	keyLabel[0x18]=(char*)"Final";
	keyLabel[0x19]=(char*)"Hanja";
	
	keyLabel[0x1B]=(char*)"ESC";
	keyLabel[0x1C]=(char*)"Convert";
	keyLabel[0x1D]=(char*)"NonConvert";
	keyLabel[0x1E]=(char*)"ACCEPT";
	keyLabel[0x1F]=(char*)"ModeChange";
	
	keyLabel[0x20]=(char*)"SP";
	keyLabel[0x21]=(char*)"PgUp";
	keyLabel[0x22]=(char*)"PgDn";
	keyLabel[0x23]=(char*)"End";
	keyLabel[0x24]=(char*)"Home";
	keyLabel[0x25]=(char*)"ก็";
	keyLabel[0x26]=(char*)"ก่";
	keyLabel[0x27]=(char*)"กๆ";
	keyLabel[0x28]=(char*)"ก้";
	keyLabel[0x29]=(char*)"Select";
	keyLabel[0x2A]=(char*)"Print";
	keyLabel[0x2B]=(char*)"Execute";
	keyLabel[0x2C]=(char*)"PrtSc";
	keyLabel[0x2D]=(char*)"Insert";
	keyLabel[0x2E]=(char*)"Delete";
	keyLabel[0x2F]=(char*)"Help";
	
	for(i=0x30;i<0x40;i++){
		keyLabel[i]=(char*)calloc(2,charsz);
		keyLabel[i][0]=i;
	}
	
	for(i=0x41;i<0x5B;i++){
		keyLabel[i]=(char*)calloc(2,charsz);
		keyLabel[i][0]=i;
	}
	
	
	keyLabel[0x5B]=(char*)"LWin";
	keyLabel[0x5C]=(char*)"RWin";
	keyLabel[0x5D]=(char*)"APPS";
	keyLabel[0x5F]=(char*)"Sleep";
	
	for(i=0x60;i<0x70;i++){
		keyLabel[i]=(char*)calloc(5,charsz);
		keyLabel[i][0]='N';
		keyLabel[i][1]='U';
		keyLabel[i][2]='M';
		keyLabel[i][3]=i-0x30;
	}
	keyLabel[0x6A]=(char*)"NUM*";
	keyLabel[0x6B]=(char*)"NUM+";
	keyLabel[0x6C]=(char*)"Separator";
	keyLabel[0x6D]=(char*)"NUM-";
	keyLabel[0x6E]=(char*)"NUM.";
	keyLabel[0x6F]=(char*)"NUM/";
	
	for(i=0x70;i<0x79;i++){
		keyLabel[i]=(char*)calloc(3,charsz);
		keyLabel[i][0]='F';
		keyLabel[i][1]=i-0x3F;
	}
	for(;i<0x88;i++){
		keyLabel[i]=(char*)calloc(4,charsz);
		t=i-0x6F;
		keyLabel[i][0]='F';
		keyLabel[i][1]=t/10+0x30;
		keyLabel[i][2]=t%10+0x30;
	}
	
	keyLabel[0x90]=(char*)"NumLock";
	keyLabel[0x91]=(char*)"ScrollLock";
	
	keyLabel[0xA0]=(char*)"LShift";
	keyLabel[0xA1]=(char*)"RShift";
	keyLabel[0xA2]=(char*)"LCtrl";
	keyLabel[0xA3]=(char*)"RCtrl";
	keyLabel[0xA4]=(char*)"LAlt";
	keyLabel[0xA5]=(char*)"RAlt";
	
	keyLabel[0xBA]=(char*)";";
	keyLabel[0xBB]=(char*)"+";
	keyLabel[0xBC]=(char*)",";
	keyLabel[0xBD]=(char*)"-";
	keyLabel[0xBE]=(char*)".";
	keyLabel[0xBF]=(char*)"/";
	
	keyLabel[0xC0]=(char*)"`";
	
	keyLabel[0xDB]=(char*)"[";
	keyLabel[0xDC]=(char*)"\\";
	keyLabel[0xDD]=(char*)"]";
	keyLabel[0xDE]=(char*)"\'";
}
