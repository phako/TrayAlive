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

#ifndef _PING_EXCEPTION_H
#define _PING_EXCEPTION_H

#include <exception>

class CPingException : public std::exception  
{
public:
	CPingException(const std::string& message) { m_message = message; };
	virtual ~CPingException() throw() {};
	virtual const char* what() const throw() { return m_message.c_str(); };
private:
	std::string m_message;
};

#endif
