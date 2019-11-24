
#include <WaspXBeeZB.h>
#include <WaspFrame.h>
// Redundant libs
#include <WaspUtils.h>
#include <WaspPWR.h>

// Destination MAC address
//////////////////////////////////////////
char RX_ADDRESS[] = "0013A20040F698CA";
//////////////////////////////////////////

// Define the Waspmote ID
char WASPMOTE_ID[] = "node_01";


// define variable
uint8_t error;

void checkNetworkParams();

void setup()
{
  // init USB port
  USB.ON();
  USB.println(F("Sending packets example"));
  
  // init XBee
  xbeeZB.ON();
  
  delay(3000);
  
  //////////////////////////
  // 2. check XBee's network parameters
  //////////////////////////
  checkNetworkParams();
  
}


void loop()
{
  // send XBee packet
  float bat = PWR.getBatteryLevel();
  char bats[53];
  Utils.float2String(bat, bats, 2);
  error = xbeeZB.send( RX_ADDRESS, "Hello");   
  error = xbeeZB.send( RX_ADDRESS, bats);   
  error = xbeeZB.send( RX_ADDRESS, WASPMOTE_ID);   
  
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



/*******************************************
 *
 *  checkNetworkParams - Check operating
 *  network parameters in the XBee module
 *
 *******************************************/
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



