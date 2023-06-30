
#include <ModbusRtu.h>

#define DE_RE 17
Modbus master(0, Serial1, DE_RE);
modbus_t telegram[2];
unsigned long u32wait;
uint16_t au16data[4];
uint16_t au16data2[4];
uint8_t u8state;
uint8_t u8query;

void setup() {
  Serial.begin(9600);

  master.begin(9600);
  master.setTimeOut(5000);
  u32wait = millis() + 1000;
  // u8state = 0;
  u8state = u8query = 0;

  telegram[0].u8id = 3;            // slave address
  telegram[0].u8fct = 3;           // Function code: 1, 2, 3, 4, 5, 6, 15 or 16 */
  telegram[0].u16RegAdd = 0;       // start address in slave
  telegram[0].u16CoilsNo = 3;      // number of elements (coils or registers) to read
  telegram[0].au16reg = au16data;  // pointer to a memory array in the Arduino

  telegram[1].u8id = 3;             // slave address
  telegram[1].u8fct = 6;            // function code
  telegram[1].u16RegAdd = 3;        // start address in slave
  telegram[1].u16CoilsNo = 1;       // number of elements (coils or registers) to read
  telegram[1].au16reg = au16data+3;  // pointer to a memory array in the Arduino
}

void loop() {

  master.query(telegram[0]);
  master.poll();  // check incoming messages

  Serial.println(au16data[0]);

//  delay(200);  // Is there any other way to do this?

//  master.query(telegram[1]);
//  master.poll();

//  Serial.print(au16data[0]);
}