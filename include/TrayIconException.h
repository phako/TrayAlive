/*
   TrayIconException.h - Exception used in TrayAlive
   
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

#ifndef _TRAY_ICON_EXCEPTION_H
#define _TRAY_ICON_EXCEPTION_H

#include <string>
#include <exception>

class TrayIconException : public std::exception  
{
public:
	/**
	 * Constructor.
	 *
	 * Create a tray icon exception with a explaining message.
	 *
	 * @param message the explaining message for the exception
	 */
	TrayIconException(const std::string& message) : m_message(message) {};

	/**
	 * Destructor.
	 */
	virtual ~TrayIconException() throw() {};

	/**
	 * Get the textual description for the exception's cause.
	 *
	 * @return a const pointer to the message.
	 */
	virtual const char* what() const throw() { return m_message.c_str(); };
private:

	/// Textual description of the exception
	std::string m_message;
};

#endif
