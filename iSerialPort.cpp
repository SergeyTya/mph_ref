/*
 * iSerialPort.cpp
 *
 *  Created on: 30 ���. 2021 �.
 *      Author: Sergey
 */

#include "iSerialPort.h"
#include "win_port.h"
#include "linux_port.h"

namespace std {

 unique_ptr<ISerialPort> ISerialPort::create(){
#if defined(__MINGW32__) || defined(_WIN32)
	 return std::unique_ptr<ISerialPort> (new WinSerialPort());
#endif
#ifdef __linux__
     return std::unique_ptr<ISerialPort> (new LinuxSerialPort());
#endif

}

 ISerialPort * ISerialPort::create2(){
	// return new   WinSerialPort() ;
}



} /* namespace sp */
