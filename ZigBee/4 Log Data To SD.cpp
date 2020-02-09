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

char filename[] = "File2.TXT";
uint8_t sd_answer;
int counter = 0;

void checkNetworkParams();
uint8_t sendMeasurement(float measurement, char descr[], char unit[]);
void logToFile(char data[],uint8_t c);


uint8_t error;
char node_ID[] = "Sensor_1"; //Kolejne płytki Waspmote powinny dosta ID; Sensor_2, Sensor_3, etc.
uint8_t  PANID[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
// Adres MAC bramy
char RX_ADDRESS[] = "0013A20040F698CA"; //odczytać w XCTU (kolumna z lewej strony)
void checkNetworkParams();
uint8_t sendMeasurement(float measurement, char descr[], char unit[]);

void setup() 
{
  // Włączenie USB
  USB.ON();
  USB.println(F("Rozpocznij program"));

  // check at command flag
  xbeeZB.ON();  
  // Uruchomienie tablicy czujnika
    //   Events.ON();   
    // Set SD ON
    SD.ON();
    // Delete file
    sd_answer = SD.del(filename);
    if ( sd_answer == 1 )
    {
      USB.println(F("file deleted"));
    }
    else
    {
        USB.println(F("file NOT deleted"));
    }
    // Create file
    sd_answer = SD.create(filename);
    if ( sd_answer == 1 )
    {
        USB.println(F("file created"));
    }
    else
    {
        USB.println(F("file NOT created"));
 }

}

void loop()
{
  // send XBee packet
  error = xbeeZB.send( RX_ADDRESS, node_ID);   
  error = xbeeZB.send( RX_ADDRESS, "Its Working !!!");   
  bat = PWR.getBatteryLevel();
  float volt = PWR.getBatteryLevel();
  error = sendMeasurement(volt, "VOLTY:", "V");
  logToFile("\n");
    //  error = sendMeasurement(temp, "TC:", "C");
    //  error = sendMeasurement(humd, "WP:", "%");
    //  error = sendMeasurement(pres, "CP:", "Pa");


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

}
void logToFile(char data[])
{
    char buffer2[52];
    sd_answer = SD.writeSD(filename, buffer2, counter);
    counter += (int)(strlen(buffer2));
    USB.print(counter, DEC);
    if ( sd_answer == 1 )
    {
        USB.print(F("\n1 - Write succesfull "));
        SD.showFile(filename);
    }
    else
    {
       USB.println(F("\n1 - Write failed"));
    }

 }



uint8_t sendMeasurement(float measurement, char descr[], char unit[])
{
  char endData[52];
  char number3[10];
  Utils.float2String (measurement, number3, 2);
  USB.println(number3);
  char logNumber[10];
  sprintf(logNumber, "%s;", number3);
  //char data2[] = "Temperatura:";
  sprintf(endData, "%s%s%s", descr, number3, unit );
  char logData[52];
  sprintf(logData, "%s%s;", descr, number3);
  logToFile(logData);
  
  uint8_t flag = xbeeZB.send(RX_ADDRESS, endData);
  
  return flag;
}
