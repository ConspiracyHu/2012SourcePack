
#include "SetupDialog.h"
#include "resource.h"

SETUPCFG setupcfg;

typedef struct {
	int w,h;
} RES;

/*RES ress[]={
	{ 320, 240},
	{ 400, 300},
	{ 512, 384},
	{ 640, 480},
	{ 800, 600},
	{1024, 768},
	{1152, 864},
	{1280, 960},
	{1280,1024},
	{1400,1050},
	{1600,1200},
};*/

BOOL CALLBACK DlgFunc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
  switch ( uMsg ) {
  case WM_LBUTTONDOWN:
	  {
		  SendMessage(hWnd,WM_NCLBUTTONDOWN,HTCAPTION,NULL);
	  }
	  break;
		case WM_INITDIALOG: {

			DEVMODE Test;
			int mode=0;
			int m=0;

			while (EnumDisplaySettings(NULL,mode++,&Test))
				if (Test.dmBitsPerPel==32)
			{
				char s[500];
				sprintf(s,"%dx%d@%d hz",Test.dmPelsWidth,Test.dmPelsHeight,Test.dmDisplayFrequency);
				SendDlgItemMessage(hWnd, IDC_RESOLUTION, CB_ADDSTRING, 0, (LPARAM)s);
				if (Test.dmPelsWidth==800 && Test.dmPelsHeight==600) SendDlgItemMessage(hWnd, IDC_RESOLUTION, CB_SETCURSEL, m, 0);
				m++;
			}

			
			/*for (int i=0; i<sizeof(ress)/sizeof(RES); i++) {
				char s[500];
				sprintf(s,"%d * %d",ress[i].w,ress[i].h);
				SendDlgItemMessage(hWnd, IDC_RESOLUTION, CB_ADDSTRING, 0, (LPARAM)s);
			}
			for (i=0; i<sizeof(ress)/sizeof(RES); i++)
				if (ress[i].w==800)
					SendDlgItemMessage(hWnd, IDC_RESOLUTION, CB_SETCURSEL, i, 0);*/

			SendDlgItemMessage(hWnd, IDC_ASPECTRATIO, CB_ADDSTRING, 0, (LPARAM)"4:3");
			SendDlgItemMessage(hWnd, IDC_ASPECTRATIO, CB_ADDSTRING, 0, (LPARAM)"5:4");
			SendDlgItemMessage(hWnd, IDC_ASPECTRATIO, CB_ADDSTRING, 0, (LPARAM)"16:10");
			SendDlgItemMessage(hWnd, IDC_ASPECTRATIO, CB_ADDSTRING, 0, (LPARAM)"16:9");
			SendDlgItemMessage(hWnd, IDC_ASPECTRATIO, CB_ADDSTRING, 0, (LPARAM)"2:1");
			SendDlgItemMessage(hWnd, IDC_ASPECTRATIO, CB_ADDSTRING, 0, (LPARAM)"3:4");
			SendDlgItemMessage(hWnd, IDC_ASPECTRATIO, CB_ADDSTRING, 0, (LPARAM)"4:5");
			SendDlgItemMessage(hWnd, IDC_ASPECTRATIO, CB_ADDSTRING, 0, (LPARAM)"10:16");
			SendDlgItemMessage(hWnd, IDC_ASPECTRATIO, CB_ADDSTRING, 0, (LPARAM)"9:16");
			SendDlgItemMessage(hWnd, IDC_ASPECTRATIO, CB_ADDSTRING, 0, (LPARAM)"1:2");
			SendDlgItemMessage(hWnd, IDC_ASPECTRATIO, CB_SETCURSEL, 0, 0);

			EnableWindow(GetDlgItem(hWnd,IDC_TEXDETAIL),0);
			EnableWindow(GetDlgItem(hWnd,IDC_ASPECTRATIO),1);
			
			//SendDlgItemMessage(hWnd, IDC_TEXDETAIL, CB_ADDSTRING, 0, (LPARAM)"Default");
			//SendDlgItemMessage(hWnd, IDC_TEXDETAIL, CB_ADDSTRING, 0, (LPARAM)"Low");
			//SendDlgItemMessage(hWnd, IDC_TEXDETAIL, CB_ADDSTRING, 0, (LPARAM)"Medium");
			//SendDlgItemMessage(hWnd, IDC_TEXDETAIL, CB_ADDSTRING, 0, (LPARAM)"High");
			//SendDlgItemMessage(hWnd, IDC_TEXDETAIL, CB_SETCURSEL, 2, 0);

			SendDlgItemMessage(hWnd, IDC_FULLSCREEN, BM_SETCHECK, 1, 1);
			SendDlgItemMessage(hWnd, IDC_MUSIC,      BM_SETCHECK, 1, 1);
			SendDlgItemMessage(hWnd, IDC_VSYNC,		 BM_SETCHECK, 1, 1);


			//EnableWindow(GetDlgItem(hWnd,IDC_MUSIC),0);
		} break;
		case WM_COMMAND:
    switch( LOWORD(wParam) ) {
			case IDOK:
				{
					int modecnt=SendDlgItemMessage(hWnd, IDC_RESOLUTION, CB_GETCURSEL, 0, 0);

					DEVMODE Test;
					int mode=0;
					int m=0;


					while (EnumDisplaySettings(NULL,mode++,&Test))
						if (Test.dmBitsPerPel==32)
					{
							if (modecnt==m) { setupcfg.mode=Test; break; }
							m++;
					}

					setupcfg.AspectRatio=4/3.0f;
					switch (SendDlgItemMessage(hWnd, IDC_ASPECTRATIO, CB_GETCURSEL, 0, 0))
					{
					case 0: setupcfg.AspectRatio=4/3.0f; break;
					case 1: setupcfg.AspectRatio=5/4.0f; break;
					case 2: setupcfg.AspectRatio=16/10.0f; break;
					case 3: setupcfg.AspectRatio=16/9.0f; break;
					case 4: setupcfg.AspectRatio=2; break;
					case 5: setupcfg.AspectRatio=3/4.0f; break;
					case 6: setupcfg.AspectRatio=4/5.0f; break;
					case 7: setupcfg.AspectRatio=10/16.0f; break;
					case 8: setupcfg.AspectRatio=9/16.0f; break;
					case 9: setupcfg.AspectRatio=0.5f; break;
					}
					
					setupcfg.fullscreen = SendDlgItemMessage(hWnd, IDC_FULLSCREEN, BM_GETCHECK , 0, 0);
					setupcfg.music      = SendDlgItemMessage(hWnd, IDC_MUSIC,      BM_GETCHECK , 0, 0);
					setupcfg.vsync		= SendDlgItemMessage(hWnd, IDC_VSYNC,      BM_GETCHECK , 0, 0);
					setupcfg.alwaysontop= SendDlgItemMessage(hWnd, IDC_ONTOP,      BM_GETCHECK , 0, 0);
				EndDialog (hWnd, TRUE);
				}
				break;
			case IDCANCEL:
		    EndDialog (hWnd, FALSE);
				break;
			case IDC_FULLSCREEN:
				if (SendDlgItemMessage(hWnd, IDC_FULLSCREEN, BM_GETCHECK , 0, 0)) {
					SendDlgItemMessage(hWnd, IDC_ONTOP, BM_SETCHECK, 0, 0);
					EnableWindow(GetDlgItem(hWnd,IDC_ONTOP),0);
				} else {
					EnableWindow(GetDlgItem(hWnd,IDC_ONTOP),1);
				}
				break;
			break;
		} break;
	}
  return ( WM_INITDIALOG == uMsg ) ? TRUE : FALSE;
}

int OpenSetupDialog(HINSTANCE hInstance) {
	return DialogBox(hInstance,MAKEINTRESOURCE(IDD_SETUP),GetForegroundWindow(),DlgFunc);
}