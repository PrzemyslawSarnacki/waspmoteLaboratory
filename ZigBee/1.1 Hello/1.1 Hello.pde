/*
    ------ Waspmote Pro Code Example --------

    Explanation: This is the basic Code for Waspmote Pro

    Copyright (C) 2016 Libelium Comunicaciones Distribuidas S.L.
    http://www.libelium.com

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

void setup()
{
  // Opening UART to show messages using 'Serial Monitor'
  USB.ON();
}

void loop()
{
  // Blinking LEDs
  Utils.blinkLEDs(1000);
 
  // Printing a message, remember to open 'Serial Monitor' to be able to see this message
  USB.println(F("Hello World, this is Waspmote!"));
 
  // A little delay
  delay(2000);
}
