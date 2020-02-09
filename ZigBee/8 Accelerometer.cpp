//#include <WaspSensorEvent_v30.h>
#include <WaspXBeeZB.h>

// Redundant libs
#include <WaspPWR.h>
#include <WaspRTC.h>
#include <WaspACC.h>
#include <WaspUtils.h>
#include <stdio.h>

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
void checkNetworkParams();

void setup()
{
  // Włączenie USB
  USB.ON();
  USB.println(F("Rozpocznij program"));
  xbeeZB.ON();
  // Uruchomienie tablicy czujnika
  ACC.ON();

  delay(3000);
  checkNetworkParams();

}

void loop()
{
  // send XBee packet
  error = xbeeZB.send( RX_ADDRESS, node_ID);
  error = xbeeZB.send( RX_ADDRESS, "Its Working !!!");
  status = ACC.check();
  error = sendMeasurement(status, "AS:", "");

  //----------X Value-----------------------
  x_acc = ACC.getX();
  USB.print(x_acc, DEC);

  error = sendMeasurement(x_acc, "AX:", "");

  //----------Y Value-----------------------
  y_acc = ACC.getY();
  USB.print(y_acc, DEC);

  error = sendMeasurement(y_acc, "AY:", "");

  //----------Z Value-----------------------
  z_acc = ACC.getZ();
  USB.print(z_acc, DEC);

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

  USB.print(F("\n------------------------------\nCheck: 0x")); 
  USB.println(status, HEX);
  USB.println(F("\n \t0X\t0Y\t0Z")); 
  USB.print(F(" ACC\t")); 
  USB.print(x_acc, DEC);
  USB.print(F("\t")); 
  USB.print(y_acc, DEC);
  USB.print(F("\t")); 
  USB.println(z_acc, DEC);

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

void checkNetworkParams()
{
  // 1. get operating 64-b PAN ID
  xbeeZB.getOperating64PAN();

  // 2. wait for association indication
  xbeeZB.getAssociationIndication();
 
  while( xbeeZB.associationIndication != 0 )
  { 
    delay(2000);
    
    // get operating 64-b PAN ID
    xbeeZB.getOperating64PAN();

    USB.print(F("operating 64-b PAN ID: "));
    USB.printHex(xbeeZB.operating64PAN[0]);
    USB.printHex(xbeeZB.operating64PAN[1]);
    USB.printHex(xbeeZB.operating64PAN[2]);
    USB.printHex(xbeeZB.operating64PAN[3]);
    USB.printHex(xbeeZB.operating64PAN[4]);
    USB.printHex(xbeeZB.operating64PAN[5]);
    USB.printHex(xbeeZB.operating64PAN[6]);
    USB.printHex(xbeeZB.operating64PAN[7]);
    USB.println();     
    
    xbeeZB.getAssociationIndication();
  }

  USB.println(F("\nJoined a network!"));

  // 3. get network parameters 
  xbeeZB.getOperating16PAN();
  xbeeZB.getOperating64PAN();
  xbeeZB.getChannel();

  USB.print(F("operating 16-b PAN ID: "));
  USB.printHex(xbeeZB.operating16PAN[0]);
  USB.printHex(xbeeZB.operating16PAN[1]);
  USB.println();

  USB.print(F("operating 64-b PAN ID: "));
  USB.printHex(xbeeZB.operating64PAN[0]);
  USB.printHex(xbeeZB.operating64PAN[1]);
  USB.printHex(xbeeZB.operating64PAN[2]);
  USB.printHex(xbeeZB.operating64PAN[3]);
  USB.printHex(xbeeZB.operating64PAN[4]);
  USB.printHex(xbeeZB.operating64PAN[5]);
  USB.printHex(xbeeZB.operating64PAN[6]);
  USB.printHex(xbeeZB.operating64PAN[7]);
  USB.println();

  USB.print(F("channel: "));
  USB.printHex(xbeeZB.channel);
  USB.println();

}
