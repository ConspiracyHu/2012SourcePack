
#include "SetupDialog.h"
#include "resource.h"

SETUPCFG setupcfg;

typedef struct {
	int w,h;
} RES;

RES ress[]={
	{ 320, 240},
	{ 400, 300},
	{ 512, 384},
	{ 640, 480},
	{ 800, 600},
	{1024, 768},
	//{1152, 864},
	//{1280, 960},
	//{1600,1200},
};

BOOL CALLBACK DlgFunc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
  switch ( uMsg ) {
		case WM_INITDIALOG: {
			int i;
			for (i=0; i<sizeof(ress)/sizeof(RES); i++) {
				char s[500];
				sprintf(s,"%d * %d",ress[i].w,ress[i].h);
				SendDlgItemMessage(hWnd, IDC_RESOLUTION, CB_ADDSTRING, 0, (LPARAM)s);
			}
			for (i=0; i<sizeof(ress)/sizeof(RES); i++)
				if (ress[i].w==1024)
					SendDlgItemMessage(hWnd, IDC_RESOLUTION, CB_SETCURSEL, i, 0);


			//EnableWindow(GetDlgItem(hWnd,IDC_TEXDETAIL),1);
			//SendDlgItemMessage(hWnd, IDC_TEXDETAIL, CB_ADDSTRING, 0, (LPARAM)"Default");
			SendDlgItemMessage(hWnd, IDC_TEXDETAIL, CB_ADDSTRING, 0, (LPARAM)"Low");
			SendDlgItemMessage(hWnd, IDC_TEXDETAIL, CB_ADDSTRING, 0, (LPARAM)"Medium");
			SendDlgItemMessage(hWnd, IDC_TEXDETAIL, CB_ADDSTRING, 0, (LPARAM)"High");
			SendDlgItemMessage(hWnd, IDC_TEXDETAIL, CB_SETCURSEL, 2, 0);


			SendDlgItemMessage(hWnd, IDC_FULLSCREEN, BM_SETCHECK, 1, 1);
			SendDlgItemMessage(hWnd, IDC_MUSIC,      BM_SETCHECK, 1, 1);
		} break;
		case WM_COMMAND:
    switch( LOWORD(wParam) ) {
			case IDOK:
				setupcfg.resolution = SendDlgItemMessage(hWnd, IDC_RESOLUTION, CB_GETCURSEL, 0, 0);
				setupcfg.fullscreen = SendDlgItemMessage(hWnd, IDC_FULLSCREEN, BM_GETCHECK , 0, 0);
				setupcfg.music      = SendDlgItemMessage(hWnd, IDC_MUSIC,      BM_GETCHECK , 0, 0);
				setupcfg.texturedetail = SendDlgItemMessage(hWnd, IDC_TEXDETAIL, CB_GETCURSEL, 0, 0);
				setupcfg.alwaysontop= SendDlgItemMessage(hWnd, IDC_ONTOP,      BM_GETCHECK , 0, 0);
		    EndDialog (hWnd, TRUE);
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