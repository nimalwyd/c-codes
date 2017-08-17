class MoxaSerialPortMock
    # The constructor will take the ip address and port of the MOXA device
    def initialize( logger, ip_address, port )
        @logger = logger
        @ip_address = ip_address
        @port = port
        puts "MoxaSerialPort::#{__method__} -> IP Address: #{@ip_address} / Port: #{@port}"
    end
    # The parameter is meant to an arry of bytes
    def write( message )
        logger.write1 "MoxaSerialPort::#{__method__} **** BEGIN ****"
        print_bytes( message )
        logger.write1 "MoxaSerialPort::#{__method__} ****  END  ****"
        return message.length
    end
    def close
        #does nothing, just here for orthogonality (a-la standard serialport)
    end
    def read( bytes_to_read )
        return "HELO"
    end
    private
    def print_bytes( packet )
        bytestream = "["
        for i in 0..packet.length-1 do
            bytestream = bytestream + " 0x%02X " % [ packet[ i ].to_i ]
        end
        bytestream = bytestream + "]"
        logger.write1 bytestream
    end
end