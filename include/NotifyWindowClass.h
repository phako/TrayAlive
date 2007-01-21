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

/** @file */
#ifndef _NOTIFY_WINDOW_CLASS
#define _NOTIFY_WINDOW_CLASS

#include <windows.h>

/**
 * Simple wrapper around WNDCLASSEX structure.
 * Hide declaration and creation of window class code in class.
 */
class CNotifyWindowClass
{
public:
	/**
	 * Static member containing the class name necessary to create the 
	 * window using this class
	 */
	static const char* sc_lpszClassName;
	
	/**
	 * Constructor. Create the ping window class.
	 *
	 * @param hInstance the instance
	 * @param lpfnWndProc window proc associated with this class
	 */
	CNotifyWindowClass(HINSTANCE hInstance, WNDPROC lpfnWndProc);

	/**
	 * Register the window class.
	 *
	 * @return true on success, false on error.
	 */
	bool registerClass();

	/**
	 * Destructor.
	 */
	virtual ~CNotifyWindowClass() {};
private:

	/// wrapped WNDCLASSEX structure.
	WNDCLASSEX m_clsx;
};

#endif // !defined(AFX_NOTIFYWINDOWCLASS_H__7FE01281_2A4A_4A2F_B659_5656F6570FA6__INCLUDED_)
