/*
 *  ------------------ [BLE_11] - Characteristic notification - Slave ----
 *  Explanation: This example shows how notifying processes works.
 *  The program first make itself discoverable and connectable, waiting 
 *  for incoming connections. Once connected, waits for notification 
 *  subscribing events and, when they are found, the subscribed attribute is 
 *  written five times to allow the master receive notification events.
 *  Then, it keeps waiting events till the connection is over.
 *
 *  Copyright (C) 2016 Libelium Comunicaciones Distribuidas S.L.
 *  http://www.libelium.com
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS ARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Version:		1.0
 *  Design:			David Gascón
 *  Implementation:	Javier Siscart
 */

#include <WaspBLE.h>
#include <WaspSensorEvent_v30.h>
// Redundant libs
#include <WaspUtils.h>
#include <WaspPWR.h>
#include <WaspRTC.h>
#include <WaspUSB.h>
#include <WaspUtils.h>
#include <stdio.h>
#include <wiring.h>
#include <WaspConstants.h>
#include "WaspAES.h"


void setName();
uint8_t sendEncrypted(float measure, char descr[], char unit[]);

// Auxiliary variable
uint8_t flag = 0;

void setup() 
{  
  USB.println(F("BLE_11 Example"));  

  // 0. Turn BLE module ON
  BLE.ON(SOCKET0);

}


void loop() 
{


  setName();
  // 1. Make Waspmote visible to other BLE modules
  BLE.setDiscoverableMode(BLE_GAP_GENERAL_DISCOVERABLE);

  // 2. Make Waspmote connectable to any other BLE device
  BLE.setConnectableMode(BLE_GAP_UNDIRECTED_CONNECTABLE);
  USB.println(F("Waiting for incoming connections..."));

  // 3. Wait for connection status event during 30 seconds. 
  flag = BLE.waitEvent(30000);
  if (flag == BLE_EVENT_CONNECTION_STATUS)
  {
    USB.println(F("Connected!"));
    USB.println(F("Now Waspmote is connected as slave.")); 

    // 3.1 Parse the status event to find MAC of master device who initiated the connection.
    // NOTE: The event captured is stored in BLE.event array.
    uint8_t b = 5;
    for(uint8_t a = 0; a < 6 ; a++)
    {
      BLE.BLEDev.mac[a] = BLE.event[b+6];
      b--;
    }

    // 3.2 Print MAC of the Master device.
    USB.print(F("MASTER MAC Address: "));
    for(uint8_t i =0; i<6; i++)
    {
      USB.printHex(BLE.BLEDev.mac[i]);
    }
    USB.println();

    // 3.3 Parse connection handler. other information about status not used in this example.
    BLE.connection_handle = BLE.event[4];



    // 4 Now wait to other events forever. If disconnection is detected, exit loop and start again
    flag = 0;
    while(flag != BLE_EVENT_CONNECTION_DISCONNECTED)
    {

      // 4.1 Wait for indication event 
      USB.println(F("Waiting events..."));
      flag = BLE.waitEvent(5000);

      if (flag == BLE_EVENT_ATTRIBUTES_STATUS)
      {
        USB.println(F("Status event found"));

        // 4.2 look if one attribute has been indicated. flag = 1 means that notifications are enabled.
        /* status event structure:
         Field:   | Message type | Payload| Msg Class | Method |  Handle | Flags |
         Length:  |       1      |    1   |     1     |    1   |    2    |    1  |
         Example: |      80      |   10   |     02    |   02   |    00   |   01  |
         */

        if(BLE.event[6] == 1)
        {
          // 4.3 Subscription received.
          uint16_t handler = 44;
          USB.print(F("The master has subscribed to notifications of the attribute with handle: "));
          USB.println(handler, DEC);

          // 4.4 Change the attribute value 5 times each 5 seconds 
          USB.println(F("Change the attribute 5 times each five seconds"));        
          for (uint8_t a = 1; a <= 100; a++)
          { 
            // 4.4.1 Write the local attribute which is indicated
            flag = BLE.writeLocalAttribute(handler, "Hello World");
            flag = sendEncrypted(PWR.getBatteryLevel, "Bateria:", "%");
            float bat = PWR.getBatteryLevel();
            flag = sendEncrypted(bat, "BT:", "%");
            float temp = Events.getTemperature();
            flag = sendEncrypted(temp, "TC:", "C");
            float humd = Events.getHumidity();
            flag = sendEncrypted(humd, "WP:", "%");
            float pres = Events.getPressure();
            flag = sendEncrypted(pres, "CP:", "Pa");
            
            if (flag == 0)
            {
              USB.print(a, DEC);
              USB.println(" - Attribute changed");
            }
            else
            {
              USB.print("Error writing. flag =");
              USB.println(flag, DEC);
            }

            // 4.4.2 Wait 5 seconds till change the attribute value
            delay(5000);
          }

          flag = 0;
        }
        else 
        {
          // 4.2.1 Notify subscription not received
          USB.println(F("Master not subscribed"));
        }
      }
      else
      {
        // 4.1.1 Maybe Other event found
        if (flag != 0)
        {          
          // Other event received from BLE module
          USB.print(F("Event found. "));
          USB.print("flag = ");
          USB.println(flag, DEC);
        }
        else 
        {
          // no event received. 
          USB.println(F("No event received"));

        }
      }

      //4.5 get status. If not connected, exit.
      if (BLE.getStatus(BLE.connection_handle) == 0)
      {        
        BLE.disconnect(BLE.connection_handle);
        break;
      }

    } // end while

    // 4. if here, disconnected.
    USB.println(F("Disconnected"));    

  }
  else 
  {
    if (flag == 0)
    {
      // If there are no events, then no one tried to connect Waspmote
      USB.println(F("No events found. No devices tried to connect Waspmote."));
    }
    else
    {
      // Other event received from BLE module
      USB.print(F("Other event found. "));
      USB.print("flag = ");
      USB.println(flag, DEC);
    }
  }

  USB.println(); 
}

void setName()
 {
  uint8_t handler = 3;
  // Variable containing the device name as an array of char.
  char deviceName[20] = "WASP BLE 1";
  USB.print(F("Setting device name as: "));
  USB.println(deviceName);
  // Write the local attribute containing the device name
  flag = BLE.writeLocalAttribute(handler, deviceName);
  if (flag == 0)
  {
  USB.println("Name written");
  }
  else
  {
  USB.print("Error writing. flag =");
  USB.println(flag, DEC);
  }
  // 2. Get the BLE device name by reading the corresponding attribute.
  USB.println(F("Reading name..."));
  flag = BLE.readLocalAttribute(3);
  // Attribute value is stored in a global variable called attributeValue.
  // NOTE: according to the standard, first byte is field length.
  USB.print(F("Device name: "));
  for (uint8_t i = 0; i < BLE.attributeValue[0]; i++)
  {
  USB.print(BLE.attributeValue[i]);
  }
  USB.println();
  delay(1000);
}

uint8_t sendEncrypted(float measure, char descr[], char unit[])
{
  uint16_t handler = 44;
  char buffer1[52];
  char number_var[10];
  Utils.float2String (measure, number_var, 2);
  USB.println(number_var);
  //char data2[] = "Temperatura:";
  sprintf(buffer1, "%s%s%s", descr, number_var, unit );
  char password[] = "libeliumlibelium";
  // Variable for encrypted message's length
  uint16_t encrypted_length;
  // Declaration of variable encrypted message
  uint8_t encrypted_message[300];
  encrypted_length = AES.sizeOfBlocks(buffer1);
  AES.encrypt(  AES_128
                , password
                , buffer1
                , encrypted_message
                , ECB
                , PKCS5);
  AES.printMessage(encrypted_message, encrypted_length);
  flag = BLE.writeLocalAttribute(handler, encrypted_message, encrypted_length);
  return flag;
}
