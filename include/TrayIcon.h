/*
   TrayIcon.h
   
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
#define _WIN32_IE 0x0600

#pragma warning(disable:4290)

#include <windows.h>
#include <shellapi.h>

#include "TrayIconException.h"

class TrayIcon 
{
	public:
		/**
		 * Constructor. Creates a tray icon for window hWnd.
		 * Notifications are sent to the window using callbackMessage.
		 *
		 * @param hWnd handle to parent window
		 * @param callbackMessage windows message to send on
		 * notification.
		 */
		TrayIcon(const HWND hWnd, const unsigned int callbackMessage);

		/// Destructor.
		virtual ~TrayIcon();

		/**
		 * Set a new icon. Also adds the icon to the tray on first call
		 * or after a call to remove has been made.
		 *
		 * @param hIcon handle to new icon
		 */
		void setIcon(const HICON hIcon) throw(TrayIconException);

		/**
		 * Set the tool tip of the tray icon. To show a balloon tip, use
		 * setInfo()
		 *
		 * @param toolTip tool tip of notification icon*/
		void setToolTip(const char* toolTip) throw(TrayIconException);

		/**
		 * removes icon from tray.
		 */
		void remove() throw(TrayIconException);

		/**
		 * set the version of the tray notification behaviour. Defaults
		 * to W2k.
		 *
		 * @param version the version to set.
		 */
		void setVersion(const unsigned int version) throw(TrayIconException);

		/**
		 * Show a balloon help message.
		 *
		 * @param message the message to show
		 * @param title title of the dialog.
		 * @param timeout timeout in milliseconds. Should be between
		 * 10000 and 30000
		 * @param icon the icon to show in the dialog.
		 */
		void setInfo(const char* message, const char* title = "",
				const unsigned int timeout = 10000, const long icon = NIIF_NONE) throw(TrayIconException);
		/// window message issued to listener when taskbar is (re-)created, for example after a crash.
		const static unsigned int WM_PING_TASKBAR_CREATED;

	private:
		/**
		 * Unique identifier for the tray icon. Is initialised to the
		 * current process id in constructor.
		 */
		unsigned int m_uid;

		/// Handle for notification window
		HWND m_hWnd;

		/// message sent to notification window on callback
		unsigned int m_callbackMessage;

		/** 
		 * internal flag whether the icon already has been added to
		 * tray.
		 */
		bool m_added;
};
