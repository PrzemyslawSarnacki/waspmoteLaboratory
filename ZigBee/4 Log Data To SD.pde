#include <WaspSensorEvent_v30.h>
#include <WaspFrame.h>
#include <WaspXBeeZB.h>

float temp;
float humd;
float pres;
float bat;

char filename[] = "File2.TXT";
uint8_t sd_answer;
int counter = 0;


uint8_t error;
char node_ID[] = "Sensor_1"; //Kolejne płytki Waspmote powinny dosta ID; Sensor_2, Sensor_3, etc.
uint8_t  PANID[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
// Adres MAC bramy
char RX_ADDRESS[] = "0013A20040F698CA"; //odczytać w XCTU (kolumna z lewej strony)


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

void logToFile(char data[],uint8_t c)
{
    char buffer2[52];
    strcpy(buffer2, &(data[c]));
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

void setup() 
{
  // Włączenie USB
  USB.ON();
  USB.println(F("Rozpocznij program"));
   xbeeZB.setPAN(PANID);

  // check at command flag
  if (xbeeZB.error_AT == 0)
  {
    USB.println(F("2. PANID set OK"));
  }
  else
  {
    USB.println(F("2. Error while setting PANID"));
  }
  
  // Uruchomienie tablicy czujnika
  Events.ON(); 
  delay(3000);
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
  error = sendMeasurement(bat, "BT:", "%");
  error = sendMeasurement(temp, "TC:", "C");
  error = sendMeasurement(humd, "WP:", "%");
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
