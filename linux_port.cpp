//
// Created by sergey on 21.01.2022.
//
#ifdef __linux__

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <dirent.h>
#include <cstring>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <cerrno>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <list>
#include <sys/ioctl.h>

#include "linux_port.h"

using namespace std;

std::LinuxSerialPort::LinuxSerialPort() = default;

std::LinuxSerialPort::~LinuxSerialPort() {
    ::close(fd);
}

void std::LinuxSerialPort::close() {

    if(fd!=-1)::close(fd);
    fd = -1;
    isOpenPort = false;
    name="no port";
}

bool LinuxSerialPort::openPort(std::string portname, unsigned long baudRate) {

    struct termios tty{};
    if (fd != -1) this->close();

    fd = ::open( portname.c_str(), O_RDWR | O_NOCTTY | O_SYNC); //O_NDELAY);
    if(fd == -1) {throw std::runtime_error(portname+" Port doesn't exist or Permission denied");}
    fcntl(fd, F_SETFL, 0);
    memset(&tty, 0, sizeof tty);
    if (tcgetattr(fd, &tty) != 0){ this->close(); throw std::runtime_error("Unable to config port");};

    int i = -1;

    switch (baudRate) {
        case 9600:   i=0; break;
        case 38400:  i=1; break;
        case 115200: i=2; break;
        case 230400: i=3; break;
        case 406000: i=4; break;
        default:
            break;
    }

    if(i==-1) {this->close(); throw std::runtime_error("Unsupported BaudRate ");};

    int spd[5] = { B9600,  B38400, B115200, B230400, B460800 };

    cfsetospeed(&tty, spd[i]);
    cfsetispeed(&tty, spd[i]);

    tty.c_cflag |= (CLOCAL | CREAD);    /* ignore modem controls */
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;         /* 8-bit characters */
    tty.c_cflag &= ~PARENB;     /* no parity bit */
    tty.c_cflag &= ~CSTOPB;     /* only need 1 stop bit */
    tty.c_cflag &= ~CRTSCTS;    /* no hardware flowcontrol */

    /* setup for non-canonical mode */
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tty.c_oflag &= ~OPOST;

    /* fetch bytes as they become available */
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 1;

    if (tcsetattr(fd, TCSANOW, &tty) != 0)
    {
        this->close();
        throw std::runtime_error("Error from tcsetattr: "+ string(strerror(errno)));
    }

    isOpenPort = true;
    name=portname;
    return true;
}

bool LinuxSerialPort::write(char * data, unsigned long len) {

    if(!isOpenPort) throw std::runtime_error("port is not open!\r\n");
    int res = ::write(fd, data, len);
    if(res!=len or res==-1) perror("Serial port error:");

    return true;
}

bool LinuxSerialPort::write(char data) {
    this->write(&data, 1);
    return true;
}

unsigned long LinuxSerialPort::read(char * data, unsigned long maxNumBytesRead) {

    int bytes;
    if(!isOpenPort) throw std::runtime_error("port is not open!");

    ::ioctl(fd, FIONREAD, &bytes) ;
    if(bytes!= maxNumBytesRead) throw std::runtime_error("can't read requested number of byte");
    if( ::read(fd, data, maxNumBytesRead) ==-1 )   throw std::runtime_error("port error");

    return 0;
}

unsigned long LinuxSerialPort::bytesToRead() {
    int res;
    ioctl(fd, FIONREAD, &res) ;
    if(res<0) res=0;
    return res;
}

bool LinuxSerialPort::isOpen() {
    return this->isOpenPort;
}

bool LinuxSerialPort::waitForReadyRead(int timeout) {
    int bytes=0;
    int bts=-1;
    int i=0;

    while(i<timeout){
        bts=bytes;
        usleep(1000);
        if(ioctl(fd, FIONREAD, &bytes)==-1) throw std::runtime_error("port error");
        if(bytes==bts){i++;}else{i=0;};
    }

    if(bytes>0) return true;
    return false;
}

bool LinuxSerialPort::waitForReadyRead(int timeout, unsigned long size) {
    unsigned long bytes = 0;
    unsigned long bts = 0;
    int i = 0;

    while (i < timeout || bytes!= size) {
        bytes = this->bytesToRead();
        if (bytes == bts) {
            i++;
        } else {
            i = 0;
        };
        bts = bytes;
        usleep(1000);
    }
    if (bytes > 0) return true;

    throw std::runtime_error("time out!");
}

std::string LinuxSerialPort::readAll() {
    int bytes=0;
    char buf[256];

    string res;

    if(::ioctl(fd, FIONREAD, &bytes)==-1) {throw std::runtime_error("port error");}
    if(::read(fd, &buf, bytes)==-1)       {throw std::runtime_error("port error");}
    res.assign(buf, bytes);
    return res;

}

std::string LinuxSerialPort::readAll(int timeout) {
    std::string res;
    while (waitForReadyRead(timeout)) {
        res += this->readAll();
    }

    if(res.length() == 0) throw std::runtime_error("time out!");
    return res;
}

std::string LinuxSerialPort::getName() const {
    return this->name;
}

void LinuxSerialPort::wait(int time_ms){
	usleep(time_ms*1000);
}


#endif






