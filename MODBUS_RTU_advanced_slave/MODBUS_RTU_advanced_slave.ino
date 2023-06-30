/**
 *  Modbus slave example 2:
 *  The purpose of this example is to link the Arduino digital and analog
 *	pins to an external device.
 *
 *  Recommended Modbus Master: QModbus
 *  http://qmodbus.sourceforge.net/
 *
 *  Edited in Spanish by LuxARTS
 */

//Includes the Modbus protocol library
#include <ModbusRtu.h>
#define ID   1
#define DE_RE 2
//Crear instancia
Modbus slave(ID, Serial, DE_RE); //ID del nodo. 0 para el master, 1-247 para esclavo
                        //Puerto serie (0 = TX: 1 - RX: 0)
                        //Protocolo serie. 0 para RS-232 + USB (default), cualquier pin mayor a 1 para RS-485
boolean led;
int8_t state = 0;
unsigned long tempus;

uint16_t au16data[16]; //La tabla de registros que se desea compartir por la red

long lrandNumber;
float frandNumber;

/*********************************************************
 Configuración del programa
*********************************************************/
void setup() {
 // io_setup(); //configure inputs and outputs

  Serial.begin(9600); //Abre la comunicación como esclavo
  slave.start();
  tempus = millis() + 100; //Guarda el tiempo actual + 100ms
  digitalWrite(13, HIGH ); //Prende el led del pin 13 (el de la placa)
}

/*********************************************************
 Inicio del programa
*********************************************************/
void loop() {
  //Check the input buffer

  state = slave.poll( au16data, 16 ); //Parameters: Table of records for information exchange
                                     //            Records table size
                                     //Returns 0 if there is no request for data
                                     //Returns 1 to 4 if there was a communication error
                                     //Returns more than 4 if the order was processed correctly
/*
  if (state > 4) { //If it is greater than 4 = the order was correct
    tempus = millis() + 50; //Current time + 50ms
    digitalWrite(13, HIGH);//turn on the led
  }
  if (millis() > tempus) digitalWrite(13, LOW );//Turn off the led 50ms later
  
  //Update Arduino pins with Modbus board
  io_poll();
  */
  /*
  lrandNumber = random(-9000, 9000);
  frandNumber = (float)lrandNumber / 100.00;
  au16data[0] = frandNumber;

  lrandNumber = random(-9000, 9000);
  frandNumber = (float)lrandNumber / 100.00;
  au16data[1] = frandNumber;

  lrandNumber = random(-9000, 9000);
  frandNumber = (float)lrandNumber / 100.00;
  au16data[2] = frandNumber;
  */
/*
  lrandNumber = random(-9000, 9000);
  frandNumber = (float)lrandNumber / 100.00;
  au16data[3] = frandNumber;
*/
} 

/**
 * pin maping:
 * 2 - digital input
 * 3 - digital input
 * 4 - digital input
 * 5 - digital input
 * 6 - digital output
 * 7 - digital output
 * 8 - digital output
 * 9 - digital output
 * 10 - analog output
 * 11 - analog output
 * 14 - analog input
 * 15 - analog input
 *
 * pin 13 reserved to see the communication status
 */
void io_setup() {
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(13, OUTPUT);

  digitalWrite(6, LOW );
  digitalWrite(7, LOW );
  digitalWrite(8, LOW );
  digitalWrite(9, LOW );
  digitalWrite(13, HIGH ); //Led del pin 13 de la placa
  analogWrite(10, 0 ); //PWM 0%
  analogWrite(11, 0 ); //PWM 0%
}

/*********************************************************
Bind the table of registers to the pins
*********************************************************/
void io_poll() {
  // digital inputs -> au16data[0]
  // Lee las entradas digitales y las guarda en bits de la primera variable del vector
  // (es lo mismo que hacer una máscara)
  bitWrite( au16data[0], 0, digitalRead( 2 )); //Lee el pin 2 de Arduino y lo guarda en el bit 0 de la variable au16data[0] 
  bitWrite( au16data[0], 1, digitalRead( 3 ));
  bitWrite( au16data[0], 2, digitalRead( 4 ));
  bitWrite( au16data[0], 3, digitalRead( 5 ));

  // digital outputs -> au16data[1]
  // Lee los bits de la segunda variable y los pone en las salidas digitales
  digitalWrite( 6, bitRead( au16data[1], 0 )); //Lee el bit 0 de la variable au16data[1] y lo pone en el pin 6 de Arduino
  digitalWrite( 7, bitRead( au16data[1], 1 ));
  digitalWrite( 8, bitRead( au16data[1], 2 ));
  digitalWrite( 9, bitRead( au16data[1], 3 ));

  // Cambia el valor del PWM
  analogWrite( 10, au16data[2] ); //El valor de au16data[2] se escribe en la salida de PWM del pin 10 de Arduino. (siendo 0=0% y 255=100%)
  analogWrite( 11, au16data[3] );

  // Lee las entradas analógicas (ADC)
  au16data[4] = analogRead( 0 ); //El valor analógico leido en el pin A0 se guarda en au16data[4]. (siendo 0=0v y 1023=5v)
  au16data[5] = analogRead( 1 );

  // Diagnóstico de la comunicación (para debug)
  au16data[6] = slave.getInCnt();  //Devuelve cuantos mensajes se recibieron
  au16data[7] = slave.getOutCnt(); //Devuelve cuantos mensajes se transmitieron
  au16data[8] = slave.getErrCnt(); //Devuelve cuantos errores hubieron
}
