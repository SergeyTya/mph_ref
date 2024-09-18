/*
 * win_port.h
 *
 *  Created on: 29 锟斤拷锟斤拷. 2021 锟�.
 *      Author: Sergey
 */

#ifndef WIN_PORT_H_
#define WIN_PORT_H_

#if defined(__MINGW32__) || defined(_WIN32)

#include <windows.h>
#include <iostream>
#include <cstring>
#include <stdlib.h>
#include <stdio.h>

#include "iSerialPort.h"

namespace std {

class WinSerialPort: public ISerialPort {
public:

	WinSerialPort();
	virtual ~WinSerialPort(){};

	 bool openPort(std::string portname, unsigned long baudRate);
	 bool write(char *data, unsigned long len);
	 bool write(char  data);

	 unsigned long read(char *data, unsigned long maxNumBytesRead);

	virtual unsigned long bytesToRead(void);

	virtual bool isOpen() { return this->isOpenPort;};
	virtual bool waitForReadyRead(int timeout);
	virtual bool waitForReadyRead(int timeout , unsigned long size);

	virtual std::string readAll();
	virtual std::string readAll(int timeout);
	virtual std::string getName() const{ return portname; };
	virtual void close(void);
	virtual void wait(int time_ms);

protected:

private:
	//win API fields
	HANDLE hComPort = NULL;
	DCB dcbComPort;
	COMMTIMEOUTS commTimeoutsComPort;
	COMMPROP commPropComPort;
	std::string portname;
	bool isOpenPort =false;




};

} /* namespace std */

#endif

#endif /* WIN_PORT_H_ */
