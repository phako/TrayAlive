/*
   PingException.h - Exception used in TrayAlive
   
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
#ifndef _PING_EXCEPTION_H
#define _PING_EXCEPTION_H

#include <exception>
#include <string>

class CPingException : public std::exception  
{
public:
	/**
	 * Constructor. Create a ping exception with a message.
	 * 
	 * @param message the message accessable with what()
	 */
	CPingException(const std::string& message) : m_message(message) { };

	/// Destructor.
	virtual ~CPingException() throw() {};

	/**
	 * Get the cause of the exception.
	 *
	 * @return a textual description of the message cause.
	 */
	virtual const char* what() const throw() { return m_message.c_str(); };
private:

	/// Textual description of the cause of the exception
	std::string m_message;
};

#endif
