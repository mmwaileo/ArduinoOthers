#include <Adafruit_MAX31865.h>
// Max31865
#define MAXCS   7
#define MAXDI   6
#define MAXDO   5
#define MAXCLK  4
#define RREF      430.0
#define RNOMINAL  100.0

// Max31865
Adafruit_MAX31865 thermo = Adafruit_MAX31865(MAXCS, MAXDI, MAXDO, MAXCLK);
void setup() {
  // put your setup code here, to run once:
// Setup Max31865
  thermo.begin(MAX31865_3WIRE);  // set to 2WIRE or 4WIRE as necessary

}

void loop() {
  // put your main code here, to run repeatedly:
  au16data[0] = thermo.temperature(RNOMINAL, RREF);
}
