//
// Created by sergey on 21.01.2022.
//

#ifndef LIB_SERIALPORT_LINUX_PORT_H
#define LIB_SERIALPORT_LINUX_PORT_H

#ifdef __linux__

#include "iSerialPort.h"

namespace std {

    class LinuxSerialPort: public ISerialPort{
    public:
        LinuxSerialPort();
        ~LinuxSerialPort() override;
        void close() override;


        bool openPort(std::string portname, unsigned long baudRate) override;
        bool write(char *data, unsigned long len) override;
        bool write(char data) override;

        unsigned long read(char *data, unsigned long maxNumBytesRead)override;
        unsigned long bytesToRead()override;

        bool isOpen() override;

        bool waitForReadyRead(int timeout) override;
        bool waitForReadyRead(int timeout, unsigned long size) override;

        std::string readAll() override;

        std::string readAll(int timeout)override;

        std::string getName() const override;

        virtual void wait(int time_ms);


    private:

        int fd = -1;
        std::string name="no port";
        bool isOpenPort = false;
    };

}

#endif
#endif //LIB_SERIALPORT_LINUX_PORT_H
