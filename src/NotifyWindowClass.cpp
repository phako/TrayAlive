/*
   NotifyWindowClass.cpp - Implementation of notification window class
   
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

#include "NotifyWindowClass.h"

//////////////////////////////////////////////////////////////////////
// Static members
const char* CNotifyWindowClass::sc_lpszClassName = "TrayAliveNotificationWindow";

CNotifyWindowClass::CNotifyWindowClass(HINSTANCE hInstance, WNDPROC lpfnWndProc)
{
	m_clsx.cbSize = sizeof(WNDCLASSEX);
	m_clsx.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	m_clsx.lpfnWndProc = lpfnWndProc;
	m_clsx.cbClsExtra = 0;
	m_clsx.cbWndExtra = 0;
	m_clsx.hInstance = hInstance;
	m_clsx.hIcon = 0;
	m_clsx.hCursor = 0;
	m_clsx.hbrBackground = 0;
	m_clsx.lpszMenuName = 0;
	m_clsx.lpszClassName = CNotifyWindowClass::sc_lpszClassName;
	m_clsx.hIconSm = 0;
}

bool CNotifyWindowClass::registerClass()
{
	ATOM res; 
	
	res = RegisterClassEx(&m_clsx);

	return res != 0;
}

