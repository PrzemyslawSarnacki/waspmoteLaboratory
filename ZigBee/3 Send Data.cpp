#include <WaspSensorEvent_v30.h>
#include <WaspXBeeZB.h>
// Redundant libs
#include <WaspPWR.h>
#include <WaspRTC.h>
#include <WaspUtils.h>
#include <stdio.h>

float temp;
float humd;
float pres;
float bat;
uint8_t error;
char node_ID[] = "Sensor_1"; //Kolejne płytki Waspmote powinny dosta ID; Sensor_2, Sensor_3, etc.
uint8_t  PANID[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
// Adres MAC bramy
char RX_ADDRESS[] = "0013A20040F698CA"; //odczytać w XCTU (kolumna z lewej strony)

void logToFile(char data[],uint8_t c);
uint8_t sendMeasurement(float measurement, char descr[], char unit[]);

void setup() 
{
    // Włączenie USB
    USB.ON();
    USB.println(F("Rozpocznij program"));
    xbeeZB.ON();
    // Uruchomienie tablicy czujnika
    Events.ON(); 
    delay(3000);
}

void loop()
{
  // send XBee packet
  error = xbeeZB.send( RX_ADDRESS, node_ID);   
  error = xbeeZB.send(RX_ADDRESS,"\n");
  error = xbeeZB.send( RX_ADDRESS, "Its Working !!!");   
  error = xbeeZB.send(RX_ADDRESS,"\n");
  bat = PWR.getBatteryLevel();
  error = xbeeZB.send(RX_ADDRESS,"\n");
  error = sendMeasurement(bat, "BT:", "%");
  temp = Events.getTemperature();
  error = xbeeZB.send(RX_ADDRESS,"\n");
  error = sendMeasurement(temp, "TC:", "C");
  humd = Events.getHumidity();
  error = xbeeZB.send(RX_ADDRESS,"\n");
  error = sendMeasurement(humd, "WP:", "%");
  pres = Events.getPressure();
  error = xbeeZB.send(RX_ADDRESS,"\n");
  error = sendMeasurement(pres, "CP:", "Pa");
  
  // check TX flag
  if( error == 0 )
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
  char number_var[10];
  Utils.float2String (measurement, number_var, 2);
  USB.println(number_var);
  sprintf(endData, "%s%s%s", descr, number_var, unit );
  uint8_t flag = xbeeZB.send(RX_ADDRESS, endData);
  return flag;
}
