//#include <WaspSensorEvent_v30.h>
#include <WaspXBeeZB.h>

// Redundant libs
#include <WaspPWR.h>
#include <WaspACC.h>
#include <WaspRTC.h>
#include <WaspUtils.h>
#include <stdio.h>

float temp;
float humd;
float pres;
float bat;
uint8_t error;
uint8_t status;
int x_acc;
int y_acc;
int z_acc;

char node_ID[] = "Sensor_1"; //Kolejne płytki Waspmote powinny dosta ID; Sensor_2, Sensor_3, etc.
uint8_t  PANID[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
// Adres MAC bramy
char RX_ADDRESS[] = "0013A20040F698CA"; //odczytać w XCTU (kolumna z lewej strony)

void logToFile(char data[], uint8_t c);
uint8_t sendMeasurement(float measurement, char descr[], char unit[]);

void setup()
{
  // Włączenie USB
  USB.ON();
  USB.println(F("Rozpocznij program"));
  xbeeZB.ON();
  // Uruchomienie tablicy czujnika
  ACC.ON();

  delay(3000);
}

void loop()
{
  // send XBee packet
  error = xbeeZB.send( RX_ADDRESS, node_ID);
  error = xbeeZB.send( RX_ADDRESS, "Its Working !!!");
  bat = PWR.getBatteryLevel();
  error = sendMeasurement(bat, "BT:", "%");
  status = ACC.check();
  error = sendMeasurement(status, "AS:", "");

  //----------X Value-----------------------
  x_acc = ACC.getX();

  error = sendMeasurement(x_acc, "AX:", "");

  //----------Y Value-----------------------
  y_acc = ACC.getY();

  error = sendMeasurement(y_acc, "AY:", "");

  //----------Z Value-----------------------
  z_acc = ACC.getZ();

  error = sendMeasurement(z_acc, "AZ:", "");

  // check TX flag
  if ( error == 0 )
  {
    USB.println(F("send ok"));

    // blink green LED
    Utils.blinkGreenLED();

  }
  else
  {
    USB.println(F("send error"));

    // blink red LED
    Utils.blinkRedLED();
  }

  // wait for five seconds
  delay(5000);
}

uint8_t sendMeasurement(float measurement, char descr[], char unit[])
{
  char endData[52];
  char number3[10];
  Utils.float2String (measurement, number3, 2);
  USB.println(number3);
  //char data2[] = "Temperatura:";
  sprintf(endData, "%s%s%s", descr, number3, unit );
  uint8_t flag = xbeeZB.send(RX_ADDRESS, endData);
  return flag;
}
