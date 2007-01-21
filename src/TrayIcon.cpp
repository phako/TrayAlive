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

/** @file */
#include <string>

#include "TrayIconException.h"
#include "TrayIcon.h"


////////////////////////////////////////////////////////////////////////////////
// Static members
// 
const unsigned int TrayIcon::WM_PING_TASKBAR_CREATED = ::RegisterWindowMessage(TEXT("TaskbarCreated"));

TrayIcon::TrayIcon(const HWND hWnd, const unsigned int callbackMessage):
	m_uid(0),
	m_hWnd(hWnd),
	m_callbackMessage(callbackMessage),
	m_added(false)
{
	m_uid = GetCurrentProcessId();
}

TrayIcon::~TrayIcon()
{
	if (m_added)
	{
		remove();
	}
}

void TrayIcon::setIcon(const HICON hIcon) throw(TrayIconException)
{
	BOOL res = FALSE;
	NOTIFYICONDATA ntd = {0};

	ntd.cbSize = sizeof(NOTIFYICONDATA);
	ntd.hWnd = m_hWnd;
	ntd.uID = m_uid;
	ntd.hIcon = hIcon;
	ntd.uFlags = NIF_ICON;

	if (!m_added)
	{
		ntd.uFlags |= NIF_MESSAGE;
		ntd.uCallbackMessage = m_callbackMessage;
		res = Shell_NotifyIcon(NIM_ADD, &ntd);
	}
	else
	{
		res = Shell_NotifyIcon(NIM_MODIFY, &ntd);
	}

	if (FALSE == res)
	{
		throw TrayIconException("Adding the tray icon failed");
	}
	else
	{
		m_added = true;
	}
}

void TrayIcon::setToolTip(const char* toolTip) throw(TrayIconException)
{
	NOTIFYICONDATA ntd = {0};
	BOOL res = FALSE;

	ntd.cbSize = sizeof(NOTIFYICONDATA);
	ntd.hWnd = m_hWnd;
	ntd.uID = m_uid;
	ntd.uFlags = NIF_TIP;
	memcpy(ntd.szTip, toolTip, strlen(toolTip));

	res = Shell_NotifyIcon(NIM_MODIFY, &ntd);
	if (FALSE == res)
	{
		throw TrayIconException("Failed to set the tool tip");
	}
}

void TrayIcon::remove() throw(TrayIconException)
{
	NOTIFYICONDATA ntd = {0};
	BOOL res = FALSE;

	ntd.cbSize = sizeof(NOTIFYICONDATA);
	ntd.hWnd = m_hWnd;
	ntd.uID = m_uid;

	res = Shell_NotifyIcon(NIM_DELETE, &ntd);
	if (FALSE == res)
	{
		throw TrayIconException("Failed to remove the icon");
	}
	else
	{
		m_added = false;
	}
}

void TrayIcon::setVersion(const unsigned int version) throw(TrayIconException)
{
	NOTIFYICONDATA ntd = {0};
	BOOL res = FALSE;

	ntd.cbSize = sizeof(NOTIFYICONDATA);
	ntd.hWnd = m_hWnd;
	ntd.uID = m_uid;
	ntd.uVersion = version;

	res = Shell_NotifyIcon(NIM_SETVERSION, &ntd);
	if (FALSE == res)
	{
		throw TrayIconException("Failed to set the version");
	}
}

void TrayIcon::setInfo(const char* message, const char* title, const unsigned int timeout, const long icon) throw(TrayIconException)
{
	NOTIFYICONDATA ntd = {0};
	BOOL res = FALSE;

	ntd.cbSize = sizeof(NOTIFYICONDATA);
	ntd.hWnd = m_hWnd;
	ntd.uID = m_uid;
	ntd.uFlags = NIF_INFO;
	ntd.uTimeout = timeout;
	memcpy(ntd.szInfo, message, strlen(message));
	memcpy(ntd.szInfoTitle, title, strlen(title));
	ntd.dwInfoFlags = icon;

	res = Shell_NotifyIcon(NIM_MODIFY, &ntd);
	if (FALSE == res)
	{
		throw TrayIconException("Failed to set a balloon tool tip");
	}
}
