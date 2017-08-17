# Description

This source code implements an interface to the 7-segment LED sign displays that we are using.

It uses a combination of C++ and Ruby to carry out the tasks.

C++ is used for the classes for the serial port and for the interface to the LED sign.

Ruby is used for the web server.

# Current Architecture

The Ruby-based webserver utilizes Webrick. It responds only to POST requests.

The POST request needs to be using basic authentication with a username and password.

The physical connection is via a MOXA 5130A Ethernet-to-RS485 server (it's quite fully featured). Here is the [user manual](http://www.moxa.com/doc/man/NPort_5100A_Series_Users_Manual_v4.pdf "5130A manual").

There is a Ruby serial port class designed to abstract out the details of the MOXA device.

There are two Ruby classes that implements the protocol for the Signal-Tech LED signs that we are using. One is for the sign that has four (4) LEDs and the other for the five (5) LEDs. 

The server uses both the serial port class and the protocol class to get the job done.

# Historical Architecture

(The following is deprecated but left in for context)

The Ruby-based webserver utilizes Webrick. It responds only to POST requests.

The POST request needs to be using basic authentication with a username and password.

The original idea was to make a ruby extension from the C++ classes, but that is a bit of black magic and didn't work properly.

Instead, we have a command-line program, called from the web server with relevant parameters. The command-line program, when it exits, will close the serial port.

# Pre-requisites

- Ruby v1.9 (minimum)

`sudo apt-get install ruby-dev`

- C++ compiler (g++) 
- Boost libraries

`sudo apt-get install libboost-all`

# How to use

## Configure MOXA device

Follow these steps for brand-new MOXA device (or one that has been reset via the reset button):
* Download DSU from the MOXA website ([file link (subject to change)](http://www.moxa.com/support/DownloadFile.aspx?type=support&id=10137 "File download link").
* Install DSU on a Windows based laptop.
* Ensure the device is powered up and has a network (RJ45) cable connected to it.
* Use the Search button to find the MOXA (it will be at IP address 192.168.127.254).
* Select the device once it is found, and right-click.
* Select the *Assign IP* option.
* Change the IP address to either DHCP or static IP (be sure to set the netmask and gateway if static).
* Save the settings.
* The MOXA device will reboot. After a minute, open up a browser to the device's IP address.
* Ensure the following settings for Serial port, port 1:
    * Baudrate: 9600 bps
    * Parity: None
    * Data bites: 8
    * Stop bits: 1
    * Flow control: None
    * FIFO: Enabled
    * Interface: RS485 2-Wire
* Ensure the following for Operating settings for port 1:
    * Operation Mode: TCP Server
    * Local TCP Port: 4001
* Save/Restart the device. 

## Connecting the sign to the MOXA device.

Use a standard DB9 connector to connect the MOXA end. The device end needs to be cut, and the wires stripped. The following are the colour to connector map:

| DB9 wire colour | Terminal Block Label |
|:---------------:|:--------------:|
| Orange | B+ |
| Yellow | A- |
| Green | G |

## Compile ledsignage

(deprecated)
Type `make all` to compile led signage.

## Configuration

Here is an example configuration, that will be found in signserver.conf.rb:

`SERVER_PORT = 2222`

`LED_SERIAL_PORT_NAME = "/dev/ttyUSB0"`

`LED_SERIAL_PORT_BAUDRATE = 9600`

`LED_SIGN_IDS = [ '1111', '2222' ]`

`LED_SIGN_ADDRESSES = [ 0x08, 0x04 ]`

`LED_TYPE = '5LED'`

`SCROLL_TIMER = 3`

`SHOW_MODE = 'OCCUPANCY'`

`MOXA_IP_ADDRESS = "192.168.1.141"`

`MOXA_IP_PORT = 4001`

### SERVER_PORT

This is the TCP/IP port that the server will listen on.

### LED_SERIAL_PORT_NAME
(deprecated)
This is the serial port device name as found on the O/S, under the /dev directory. For any USB to Serial device, it will be /dev/ttyUSBN, where N is 0, 1, 3, etc.

### LED_SERIAL_PORT_BAUDRATE
(deprecated)
This is the serial port speed to be set. Currently, it is ignored as it's hardcoded to 9600, but it needs to be here!

### LED_SIGN_IDS

This is the an array of LED IDs. The *[* *]* are the shorthand for creating an array in Ruby.

### LED_SIGN_ADDRESSES

This is the an array of addresses. Each address is configured via dip switches, on the LED sign itself. The *[* *]* are the shorthand for creating an array in Ruby.

### LED_TYPE

This should be set to '5LED' if communicating to a 5-LED sign. Note that 4 and 5 LED signs will not work with same protocol.

### SCROLL_TIMER

This is the value for which the sign will display the OPEN and then the number.

### SHOW_MODE

Setting this to 'OCCUPANCY' will show the occupancy value on the sign. Setting it to 'VACANCY' will show the vacancy value on the sign.

### MOXA_IP_ADDRESS

This is the IP Address of the MOXA 5130A device.

### MOXA_IP_ADDRESS

This is the **Local TCP Port** value as found on the MOXA 5130 Operating Settings configuration page (normally 4001). 

# Supported JSON calls

The following JSON are handled, via POST:

For route /rate:

`{`
`   "currentRateName": "Special event rate",`
`   "currentRateAmount": "5.00"`
`}`

For route /occupancy:

`{`
`   "currentOccupancy": "100",`
`   "currentVacancy": "0",`
`   "fullMessage": "Lot Full",`
`   "openMessage": "",`
`   "closedMessage": ""`
`   "ledId" : "1111"`
`}`

# Future enhancements

(deprecated for now)
- Allow serial port class to have the speed as a parameter.
- Do a C++ REST server instead.
