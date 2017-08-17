require 'socket'

#
# This class is meant to abstract out the Moxa Serial Port 5130A adapter's 
# "internal" workings from the cruel outside world. 
# It looks and smells like a regular old serial port, but it is more, so much
# more. 
# It is expected that the MOXA device will be set up as a TCP server, *not* 
# as a RealCOM port. This can be done either via telnet to the device, or 
# (better) via the web browser.
# The LED signs that we are using, from SignalTech, expects the serial port  
# to have a set up of 9600 bps, 8bit, No parity and 1 stop bit.
class MoxaSerialPort
    # The constructor will take the ip address and port of the MOXA device
    def initialize( logger, ip_address, port )
        @logger = logger
        @ip_address = ip_address
        @port = port
    end
    # The parameter is meant to be an arry of bytes
    def write( message )
        bytes_written = -1
        begin
            client = TCPSocket.new( @ip_address, @port )
            print_bytes( message )
            bytes_written = client.write message.pack( 'C*' )
            client.close
        rescue Exception => e
            @logger.write1 "MoxaSerialPort::#{__method__}:: caught exception: #{e.message}"
        end
        return bytes_written
    end
    def close
        #does nothing, just here for orthogonality (a-la standard serialport)
    end
    # The following method hasn't actually been tested yet... So use with caution!
    def read( bytes_to_read )
        begin
            client = TCPSocket.new( @ip_address, @port )
            read_bytes = client.read( bytes_to_read )
            puts "Read #{read_bytes.length}:"
            print_bytes( read_bytes )
            return read_bytes
        rescue Errno::ETIMEDOUT
            return -1
        end
    end
    private        
    def print_bytes( packet )
        bytestream = "["
        for i in 0..packet.length-1 do
            bytestream = bytestream + " 0x%02X " % [ packet[ i ].to_i ]
        end
        bytestream = bytestream + "]"
        @logger.write1 "MoxaSerialPort:: Sending bytestream: #{bytestream}"
    end
end