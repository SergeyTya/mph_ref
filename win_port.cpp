/*
 * win_port.cpp
 *
 *  Created on: 29 ����. 2021 ￄ1�7.
 *      Author: Sergey
 */

#include "win_port.h"


#define Exception const char *

#if defined(__MINGW32__) || defined(_WIN32)


namespace std {

WinSerialPort::WinSerialPort(){
	//cout<<"WinSerialPort created"<<endl;
};

bool WinSerialPort::openPort(std::string portname, unsigned long baudRate){
	try {

		ZeroMemory(&dcbComPort,sizeof(DCB));
			this->portname = "\\\\.\\"+portname;

		    hComPort = CreateFile(portname.c_str(),GENERIC_READ | GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL);
		    if (hComPort == INVALID_HANDLE_VALUE) {
		        throw std::runtime_error("can't open port");
		    }
		    if(!GetCommState(hComPort, &dcbComPort)) {
		        throw std::runtime_error("can't open port: GetCommState");
		    };

		    dcbComPort.DCBlength = sizeof(DCB);
		    GetCommState(hComPort, &dcbComPort);
		    dcbComPort.fBinary = true;
		    dcbComPort.BaudRate = DWORD(baudRate);
		    dcbComPort.ByteSize = 8;
		    dcbComPort.Parity = NOPARITY;
			dcbComPort.StopBits = ONESTOPBIT;
			dcbComPort.fAbortOnError = FALSE; //fAbortOnError
			dcbComPort.wReserved = 0;
			dcbComPort.fDtrControl = DTR_CONTROL_DISABLE;
			dcbComPort.fRtsControl = RTS_CONTROL_DISABLE;
			dcbComPort.fBinary = TRUE;
			dcbComPort.fParity = FALSE;
			dcbComPort.fInX = FALSE;
			dcbComPort.fOutX = FALSE;
			dcbComPort.XonChar = 0x11;
			dcbComPort.XoffChar = (unsigned char) 0x13;
			dcbComPort.ErrorChar = 0;
			dcbComPort.fErrorChar = FALSE;
			dcbComPort.fNull = FALSE;
			dcbComPort.fOutxCtsFlow = FALSE;
			dcbComPort.fOutxDsrFlow = FALSE;
			dcbComPort.XonLim = 128;
			dcbComPort.XoffLim = 128;
		    if(!SetCommState(hComPort, &dcbComPort)) {
		        throw std::runtime_error("can't open port: SetCommState");
		    };
		    if(!GetCommTimeouts(hComPort, &commTimeoutsComPort)) {
		        throw std::runtime_error("can't open port: GetCommTimeouts");
		    };
		    commTimeoutsComPort.ReadIntervalTimeout          = 100; // milliseconds
		    commTimeoutsComPort.ReadTotalTimeoutMultiplier   = 0;  //
		    commTimeoutsComPort.ReadTotalTimeoutConstant     = 0;  //
		    commTimeoutsComPort.WriteTotalTimeoutMultiplier  = 0;  //
		    commTimeoutsComPort.WriteTotalTimeoutConstant    = 0;  //
		    if(!SetCommTimeouts(hComPort, &commTimeoutsComPort)) {
		        throw std::runtime_error("can't open port: SetCommTimeouts");
		    };
		    isOpenPort = true;
		    return true;

	} catch (std::runtime_error &e) {
		std::cout<<"Error: "<<e.what()<<std::endl;
		isOpenPort = false;
		return false;
	}

}

unsigned long WinSerialPort::read(char* data, unsigned long maxNumBytesRead) {
    if (isOpenPort) {
        DWORD dwBytesRead = 0;
        DWORD numRedByte, temp;
        COMSTAT comstat;
        ClearCommError(hComPort, &temp, &comstat); // заполнить структуру COMSTAT
        numRedByte = comstat.cbInQue; //получить количество принятых байтов
			if (numRedByte > 0) {
            if(!ReadFile(hComPort, data, maxNumBytesRead, &dwBytesRead, NULL)){
                throw std::runtime_error("read error");
            } else {
                return dwBytesRead;
            }
        } else {
            return 0;
        }
    }
    return 0;
}

std::string WinSerialPort::readAll(){
	std::string res="";
	int bytes = bytesToRead();
	if(bytes > 65536) throw std::runtime_error("buffer size error");
	char buf[bytes];
	WinSerialPort::read(buf, bytes);
	if(bytes>0)res.assign(buf, bytes);
	//cout<<res<<endl;
	return res;
}

std::string WinSerialPort::readAll(int timeout){

	std::string res;
	while (waitForReadyRead(timeout)) {
		res += WinSerialPort::readAll();
	}

	if(res.length() == 0) throw std::runtime_error("time out!");
	return res;

}

bool WinSerialPort::write(char *data, unsigned long len) {

	if (isOpenPort) {
		DWORD dwBytesWrite = len; // кол-во записанных байтов
		if (!WriteFile(hComPort, data, len, &dwBytesWrite, NULL)) {
			throw std::runtime_error("write error\r\n");
		}
		if (!FlushFileBuffers(hComPort)) {
			throw std::runtime_error(
					"com port write error: FlushFileBuffers\r\n");
		};
		return true;
	} else {
		throw std::runtime_error("port is not open!\r\n");
	}
}

bool WinSerialPort::write(char data){
	WinSerialPort::write(&data, 1);
	return true;
}

unsigned long WinSerialPort::bytesToRead(void) {
    if (isOpenPort) {
        unsigned long numRedByte, temp; // temp - заглушка
        COMSTAT comstat; // структура для получения притяных байтов
        ClearCommError(hComPort, &temp, &comstat); // заполнить структуру COMSTAT
        numRedByte = comstat.cbInQue; //получить количество принятых байтов
        return numRedByte;

    } else {
    	throw std::runtime_error("port is not open!\r\n");
    }
}


void WinSerialPort::close(void) {
    if (isOpenPort) {
        CloseHandle(hComPort);
        isOpenPort = false;
    }
}

bool WinSerialPort::waitForReadyRead(int timeout) {

	unsigned long bytes = 0;
	unsigned long bts = 0;
	int i = 0;

	while (i < timeout) {
		bytes = bytesToRead();
		Sleep(1);
		if (bytes >= bts) {
			i++;
		} else {
			i = 0;
		}
		bts = bytes;
	}

	if (bytes > 0) {
		return true;
	}

	return false;

}

bool WinSerialPort::waitForReadyRead(int timeout, unsigned long size) {

  unsigned long bytes = 0;
  unsigned long bts = 0;
  int i = 0;

  while (i < timeout) {
	bytes = bytesToRead();
	Sleep(10);
    if(bytes == size){
      return true;
    }
    if (bytes == bts) {
      i++;
    } else {
      i = 0;
      bts = bytes;
    }
  }

  throw std::runtime_error("time out!");
}

void WinSerialPort::wait(int time_ms){
	Sleep(time_ms);
}

} /* namespace std */


#endif
