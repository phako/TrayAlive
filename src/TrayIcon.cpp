/*
   TrayIcon.cpp

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
#include <string>

#include "TrayIcon.h"

TrayIcon::TrayIcon(const HWND hWnd, const unsigned int callbackMessage) :
	m_uid(0),
	m_hWnd(hWnd),
	m_callbackMessage(callbackMessage),
	m_added(false)
{
	m_uid = GetCurrentProcessId();
}

TrayIcon::~TrayIcon()
{
	remove();
}

void TrayIcon::setIcon(const HICON hIcon)
{
	NOTIFYICONDATA ntd = {0};

	ntd.cbSize = sizeof(NOTIFYICONDATA);
	ntd.hWnd = m_hWnd;
	ntd.uID = m_uid;
	ntd.hIcon = hIcon;
	ntd.uFlags = NIF_ICON;
	ntd.uVersion = 0x500;

	if (!m_added)
	{
		ntd.uFlags |= NIF_MESSAGE;
		ntd.uCallbackMessage = m_callbackMessage;
		Shell_NotifyIcon(NIM_ADD, &ntd);
		m_added = true;
	}
	else
	{
		Shell_NotifyIcon(NIM_MODIFY, &ntd);
	}
}

void TrayIcon::setToolTip(const char* toolTip)
{
	NOTIFYICONDATA ntd = {0};

	ntd.cbSize = sizeof(NOTIFYICONDATA);
	ntd.hWnd = m_hWnd;
	ntd.uID = m_uid;
	ntd.uVersion = 0x500;
	ntd.uFlags = NIF_TIP;
	memcpy(ntd.szTip, toolTip, strlen(toolTip));

	Shell_NotifyIcon(NIM_MODIFY, &ntd);
}

void TrayIcon::remove()
{
	NOTIFYICONDATA ntd = {0};

	ntd.cbSize = sizeof(NOTIFYICONDATA);
	ntd.hWnd = m_hWnd;
	ntd.uID = m_uid;
	ntd.uVersion = 0x500;

	Shell_NotifyIcon(NIM_DELETE, &ntd);
	m_added = false;
}

void TrayIcon::setInfo(const char* message, const char* title, const unsigned int timeout, const long icon)
{
	NOTIFYICONDATA ntd = {0};

	ntd.cbSize = sizeof(NOTIFYICONDATA);
	ntd.hWnd = m_hWnd;
	ntd.uID = m_uid;
	ntd.uVersion = 0x500;
	ntd.uFlags = NIF_INFO;
	ntd.uTimeout = timeout;
	memcpy(ntd.szInfo, message, strlen(message));
	memcpy(ntd.szInfoTitle, title, strlen(title));
	ntd.dwInfoFlags = icon;

	Shell_NotifyIcon(NIM_MODIFY, &ntd);
}
