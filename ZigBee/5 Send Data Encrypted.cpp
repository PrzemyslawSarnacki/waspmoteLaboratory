#include <WaspSensorEvent_v30.h>
#include <WaspFrame.h>
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

uint8_t sendEncrypted(float measure, char descr[], char unit[])
{
    uint16_t handler = 44;
    char buffer1[52];
    char number3[10];
    Utils.float2String (measure, number3, 2);
    USB.println(number3);
    //char data2[] = "Temperatura:";
    sprintf(buffer1, "%s%s%s", descr, number3, unit );
    char password[] = "libeliumlibelium";
    // Variable for encrypted message's length
    uint16_t encrypted_length;
    // Declaration of variable encrypted message
    uint8_t encrypted_message[300];
    encrypted_length = AES.sizeOfBlocks(buffer1);
    AES.encrypt( AES_128
    , password
    , buffer1
    , encrypted_message
    , ECB
    , PKCS5);
    AES.printMessage(encrypted_message, encrypted_length);
    uint8_t flag = xbeeZB.send(RX_ADDRESS, encrypted_message, encrypted_length);
    return flag;
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
}


void loop()
{
    // send XBee packet
    error = xbeeZB.send( RX_ADDRESS, node_ID);   
    error = xbeeZB.send( RX_ADDRESS, "Its Working !!!");   
    error = sendEncrypted(bat, "BT:", "%");
    error = sendEncrypted(temp, "TC:", "C");
    error = sendEncrypted(humd, "WP:", "%");
    error = sendEncrypted(pres, "CP:", "Pa");
    
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
