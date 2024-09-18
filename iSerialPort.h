/*
 * iSerialPort.h
 *
 *  Created on: 30 溴�. 2021 �.
 *      Author: Sergey
 */

#ifndef ISERIALPORT_H_
#define ISERIALPORT_H_


#include <memory>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>



namespace std {

class ISerialPort;


class ISerialPort {
public:

	static std::unique_ptr<ISerialPort> create();
	static  ISerialPort * create2();

	virtual bool openPort(std::string portname, unsigned long baudRate)=0;
	virtual bool write(char *data, unsigned long len)=0;
	virtual bool write(char data)=0;

	virtual unsigned long read(char *data, unsigned long maxNumBytesRead) =0;
	virtual unsigned long bytesToRead(void) =0;

	virtual bool isOpen()=0;
	virtual bool waitForReadyRead(int timeout)=0;
	virtual bool waitForReadyRead(int timeout, unsigned long size) =0;

	virtual std::string readAll() =0;
	virtual std::string readAll(int timeout) =0;
	virtual std::string getName() const = 0;
	virtual void close() = 0;

	virtual void wait(int time_ms) =0;

	virtual ~ISerialPort()=default;

};


} /* namespace sp */
#endif /* ISERIALPORT_H_ */
