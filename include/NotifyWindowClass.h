/*
   NotifyWindowClass.h

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
// NotifyWindowClass.h: interface for the CNotifyWindowClass class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _NOTIFY_WINDOW_CLASS
#define _NOTIFY_WINDOW_CLASS

#include <windows.h>

struct CNotifyWindowClass
{
public:
	static const char* sc_lpszClassName;
	
	CNotifyWindowClass(HINSTANCE hInstance, WNDPROC lpfnWndProc);
	bool registerClass();
	virtual ~CNotifyWindowClass() {};
private:
	WNDCLASSEX m_clsx;
};

#endif // !defined(AFX_NOTIFYWINDOWCLASS_H__7FE01281_2A4A_4A2F_B659_5656F6570FA6__INCLUDED_)
