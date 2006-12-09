/*
   TrayAlive.cpp - Main program
   
   Copyright 2006 Jens Georg
   
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
   
       http://www.apache.org/licenses/LICENSE-2.0
   
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#define _WIN32_IE 0x0600

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include "resource.h"

#include <winsock2.h>
#include <windows.h>
#include <shellapi.h>
#include <stdio.h>

#include "NotifyWindowClass.h"
#include "Ping.h"

const unsigned int uID = 1862347973;

CPing* ping;
HINSTANCE myInstance;
HICON m_hIconUp;
HICON m_hIconDown;
char pszToolTip[255] = {0};
HMENU hSubMenu;

LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}


LRESULT CALLBACK MyWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	char buf[255] = {0};
	NOTIFYICONDATA ntd = {0};
	int num;
	BOOL res;

	switch (msg)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_TRAYMENU_QUIT:
			// end program;
			ping->stop();
			ntd.cbSize = sizeof(NOTIFYICONDATA);
			ntd.hWnd = hWnd;
			ntd.uID = uID;
			Shell_NotifyIcon(NIM_DELETE, &ntd);
			DestroyWindow(hWnd);
			return 0;
		case ID_TRAYMENU_PINGHOST:
			switch (GetMenuState(hSubMenu, ID_TRAYMENU_PINGHOST, MF_BYCOMMAND))
			{
			case MF_CHECKED:
				ping->stop();
				num = CheckMenuItem(hSubMenu, ID_TRAYMENU_PINGHOST, MF_BYCOMMAND | MF_UNCHECKED);
				break;
			case MF_UNCHECKED:
				ping->start();
				num = CheckMenuItem(hSubMenu, ID_TRAYMENU_PINGHOST, MF_BYCOMMAND | MF_CHECKED);
			default:
				break;
			}
			return 0;
		case ID_TRAYMENU_ABOUT:
			DialogBox(myInstance, (LPCTSTR)IDD_ABOUT, hWnd, (DLGPROC)About);
		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}
	case CPing::PING_HOST_UP:
	case CPing::PING_HOST_DOWN:
		ntd.cbSize = sizeof(NOTIFYICONDATA);
		ntd.hWnd = hWnd;
		ntd.uID = uID;
		num = sprintf(buf, "Host \"%s\" is %s", ((CPing*)wParam)->getHostName(), msg == CPing::PING_HOST_UP ? "up" : "down");
		ntd.hIcon = msg == CPing::PING_HOST_UP ? m_hIconUp : m_hIconDown;
		strncpy(ntd.szInfo, buf, num);
		ntd.uFlags = NIF_ICON | NIF_INFO;
		ntd.uTimeout = 10000;
		ntd.uVersion = 0x500;

		res = Shell_NotifyIcon(NIM_MODIFY, &ntd);
		return 0;
	case WM_USER + 0x5000:
		switch (LOWORD(lParam))
		{
		case WM_LBUTTONUP:
			ping->stop();
			break;
		case WM_RBUTTONUP:
			POINT pos;
			GetCursorPos(&pos);
			SetForegroundWindow(hWnd);
			TrackPopupMenu(hSubMenu, 0, pos.x, pos.y, 0, hWnd, NULL);
			PostMessage(hWnd, WM_NULL, 0, 0);
			break;
		}
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	WSADATA wsaData;
	int num;

	// Initialise Winsock library
	WSAStartup(MAKEWORD(2,2), &wsaData);

	CNotifyWindowClass wndClass(hInstance, (WNDPROC) MyWindowProc);
	HWND hWnd;
	int bRet;
	MSG msg;
	NOTIFYICONDATA ntd = {0};

	myInstance = hInstance;

	if (!wndClass.registerClass())
	{
		MessageBox(NULL, "Could not register window class for window", "Error", MB_OK);
		return 1;
	}

	hWnd = CreateWindow(CNotifyWindowClass::sc_lpszClassName, "", WS_POPUP,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, 0,
		hInstance, 0);

	if (hWnd == NULL)
	{
		MessageBox(NULL, "Error while creating window", "Error", MB_OK);
		return 1;
	}

	m_hIconDown = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON_DOWN));
	m_hIconUp = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON_UP));

	HMENU hMenu = LoadMenu(myInstance, MAKEINTRESOURCE(IDR_MENU1));
	hSubMenu = GetSubMenu(hMenu, 0);
	SetMenuDefaultItem(hSubMenu, ID_TRAYMENU_QUIT, FALSE);

	ntd.cbSize = sizeof(NOTIFYICONDATA);
	ntd.hWnd = hWnd;
	ntd.hIcon = m_hIconUp;
	ntd.uID = uID;
	ntd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	ntd.uCallbackMessage = WM_USER + 0x5000;
	ntd.uVersion = 0x500;

	num = sprintf(pszToolTip, "TrayAlive for host %s", lpCmdLine);
	strncpy(ntd.szTip, pszToolTip, num);
	Shell_NotifyIcon(NIM_ADD, &ntd);

	ping = new CPing(hWnd, lpCmdLine, 32, 3000, 4, false);
	ping->start();

	while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
	{
		if (bRet == -1)
		{
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	delete ping;

	WSACleanup();
	return 0;
}



