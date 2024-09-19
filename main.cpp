#include <iostream>
#include "iSerialPort.h"
#include "crc16.h"
#include "math.h"

using namespace std;


int modbusAdr = 1;
string port_name = "/dev/ttyUSB0";
int baudrate = 115200;

std::unique_ptr<ISerialPort> port;

bool check_modbus_device();

int main(int argc, char* argv[]) {

  cout << "hello me" << endl;
  port = ISerialPort::create();
  port->openPort(port_name, baudrate);

  if (!port->isOpen()) {
    cout << "port not found" << endl;
    return 0;
  }

  bool res = check_modbus_device();
  if (!res) {
    cout << "modbus device not found" << endl;
    return 0;
  }

  float refTh = 0;

  while (1) {

    refTh += M_PI * 2 * 0.008 / 5; //5 sec

    if (refTh > M_PI) {
      refTh += -M_PI * 2;
    }

    if (refTh < -M_PI) {
      refTh += M_PI * 2;
    }

    float temp_max = 380.f * 6.f;  // rpm max
    float temp_ref = cos(refTh) * temp_max;
    int16_t temp_ref16 = (uint16_t)temp_ref;
    char req[] = { static_cast<char>(modbusAdr), 0x06, 0x0, 45, ((uint8_t*)&temp_ref16)[1], ((uint8_t*)&temp_ref16)[0], 0, 0 };
    crc16::usMBCRC16(req, 6);
    port->write(req, 6 + 2);
    port->wait(100);
    cout << temp_ref16 << endl;
  }

}


bool check_modbus_device() {

  try {

    if (!port->isOpen())
      throw std::runtime_error("Port is close");
    if (modbusAdr < 1 || modbusAdr > 255)
      throw std::runtime_error("Wrong modbus address");

    char req[] = { static_cast<char>(modbusAdr), 0x2b, 0xe, 0x1, 0x1, 0, 0 };

    cout << "Searching ModBus device address " << modbusAdr << " -> ";

    crc16::usMBCRC16(req, 5);

    port->readAll();
    port->write(req, 5 + 2);

    string str = port->readAll(15);
    if (str.length() < 4 || !crc16::checkCRC((char*)str.data(), str.length()))
      throw std::runtime_error("CRC mismatch");

    cout << "ok" << endl;
    cout << "Device: " << str.substr(1, str.length() - 3) << endl;

    cout.flush();
    return true;

  }
  catch (std::runtime_error& e) {
    cout << endl;
    cout.flush();
    std::cerr << "Error: " << e.what() << '\n';
    return false;
  }
}
