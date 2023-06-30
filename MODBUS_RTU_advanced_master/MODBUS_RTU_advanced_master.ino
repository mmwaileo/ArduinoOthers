/**
 *  Modbus master example 2:
 *  The purpose of this example is to query several sets of data
 *  from an external Modbus slave device. 
 *  The link media can be USB or RS232.
 *
 *  Recommended Modbus slave: 
 *  diagslave http://www.modbusdriver.com/diagslave.html
 *
 *  In a Linux box, run 
 *  "./diagslave /dev/ttyUSB0 -b 19200 -d 8 -s 1 -p none -m rtu -a 1"
 * 	This is:
 * 		serial port /dev/ttyUSB0 at 19200 baud 8N1
 *		RTU mode and address @1
 */

#include <ModbusRtu.h>
#define DE_RE 17
uint16_t au16data[16]; //!< data array for modbus network sharing
uint8_t u8state; //!< machine state
uint8_t u8query; //!< pointer to message query

/**
 *  Modbus object declaration
 *  u8id : node id = 0 for master, = 1..247 for slave
 *  port : serial port
 *  u8txenpin : 0 for RS-232 and USB-FTDI 
 *               or any pin number > 1 for RS-485
 */
Modbus master(0,Serial1,DE_RE); // this is master and RS-232 or USB-FTDI

/**
 * This is an structe which contains a query to an slave device
 */
modbus_t telegram[2];
float temp, humidity, pressure;
unsigned long u32wait;

void setup() {
  // telegram 0: read registers
  telegram[0].u8id = 1; // slave address
  telegram[0].u8fct = 3; // function code (this one is registers read)
  telegram[0].u16RegAdd = 0; // start address in slave
  telegram[0].u16CoilsNo = 3; // number of elements (coils or registers) to read
  telegram[0].au16reg = au16data; // pointer to a memory array in the Arduino

  // telegram 1: write a single register
  telegram[1].u8id = 1; // slave address
  telegram[1].u8fct = 6; // function code (this one is write a single register)
  telegram[1].u16RegAdd = 3; // start address in slave
  telegram[1].u16CoilsNo = 1; // number of elements (coils or registers) to read
  telegram[1].au16reg = au16data+3; // pointer to a memory array in the Arduino

    // Setup Serial Terminal
  Serial.begin(9600);

  Serial1.begin( 9600 ); // baud-rate at 19200
  master.start();
  master.setTimeOut( 5000 ); // if there is no answer in 5000 ms, roll over
  u32wait = millis() + 1000;
  u8state = u8query = 0; 

  // Initialise variables
  temp = 0.0; humidity = 0.0; pressure = 0.0;
  au16data[3] = 0;
  

}

void loop() {
  switch( u8state ) {
  case 0: 
    if (millis() > u32wait) u8state++; // wait state
    break;
  case 1: 
    master.query( telegram[u8query] ); // send query (only once)
    u8state++;
	u8query++;
	if (u8query > 1) u8query = 0;
    break;
  case 2:
    master.poll(); // check incoming messages
    if (master.getState() == COM_IDLE) {
      u8state = 0;
      u32wait = millis() + 1000; 
    }
    break;
  }

  temp = au16data[0]; // temp from Max31865
  humidity = au16data[1]/10.0; // humidity from RK330
  pressure = au16data[2]/10.0; // pressure from RK330
  
    if (Serial.available()) {
     au16data[3] = Serial.parseInt();
  }

  Serial.print("temp: "); Serial.print(temp);
  Serial.print(" deg, humidity: "); Serial.print(humidity);
  Serial.print(" %RH, pressure: "); Serial.print(pressure);
  Serial.print(" kPa, au16data[3]: "); Serial.println(au16data[3]);
}

