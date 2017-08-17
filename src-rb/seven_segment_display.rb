### FILE_DESCRIPTION
# This file contains the interfaces to the Signal-Tech seven-segment display
# LED signs that we are using, as of June 21, 2016.
#
# The standard sign uses a 4-LED setup and the SevenSegmentDisplayLibrary class
# is meant to handle that case.
#
# Signal-Tech also has a 5-LED setup, and for that case, we've got the 
# SevenSegmentDisplayLibrary5Led class.
### END_FILE_DESCRIPTION

require_relative 'exceptions'

# This is here because Ruby doesn't do enums
module SendPacketBytePosition
    SYN0            = 0
    SYN1            = 1
    STX             = 2
    SignAddress     = 3
    CommandType     = 4
    ResponsePacket  = 5
    Char0           = 6
    Char1           = 7
    Char2           = 8
    Char3           = 9
    Checksum        = 10
    ETX             = 11
end

# This is here because Ruby doesn't do enums
# For the 5-LED case
module SendPacketBytePosition5Led
    SYN0            = 0
    SYN1            = 1
    STX             = 2
    SignAddress     = 3
    CommandType     = 4
    ResponsePacket  = 5
    Char0           = 6
    Char1           = 7
    Char2           = 8
    Char3           = 9
    Char4           = 10
    Checksum        = 11
    ETX             = 12
end

module ReceivePacketBytePosition
    SYN0    = 0
    SYN1    = 1
    STX     = 2
    ACKNAK  = 3
    ETX     = 4
end

# These are the commands that the Signal-Tech signs
# support.
module Command
    DisplayNumberOld            = 0
    DisplayFull                 = 1     #FULL
    DisplayOpen                 = 2     #OPEn
    DisplayClosed               = 3     #CLSd
    DisplayBlank                = 4
    DisplayPositiveTemperature  = 5
    DisplayNumber               = 6
    DisplayNegativeTemperature  = 7
end

class SevenSegmentDisplayLibrary
    attr_reader     :address
    # The constructor has the following inputs:
    #  port: the serial port shall be passed in via dependency injection.
    #           It is an object of type SerialPort
    #  address: this shall be a number. It indicates the sign's physical address.
    #  expect_response: this shall be a boolean, indicates whether we a
    #           response back from the sign.
    def initialize( serial_port, address, expect_response )
        @expect_response = expect_response ? 1 : 0
        @serial_port = serial_port # SerialPort.new( LED_SERIAL_PORT_NAME, LED_SERIAL_PORT_BAUDRATE, 8, 1, SerialPort::NONE )
        @address = address
        @null_message = [ 0x00, 0x00, 0x00, 0x00 ]
        @max_chars_allowed_on_display = 4
        @SYN = 0x16
        @STX = 0x02
        @ETX = 0x03
    end
    
    def show_number( number_to_display )
        puts "In #{__method__} =-> #{number_to_display}"
        if( number_to_display > 9999 || number_to_display < -999 )
            raise NumberOutOfRangeException
        end

        show_number_as_string( number_to_display.to_s )
    end

    def show_number_as_string( number_as_string )
        puts "In #{__method__} =-> #{number_as_string}"
        if( number_as_string.length > @max_chars_allowed_on_display )
            raise NumberOutOfRangeException
        end
        
        right_justified_string = right_justify_message( number_as_string, number_as_string.length )
        puts "In #{__method__} =-> '#{right_justified_string}' (#{right_justified_string.length})"
        return send_message( Command::DisplayNumber, right_justified_string.bytes.to_a, right_justified_string.length )
    end

    def show_full( )
        puts "In #{__method__}"
        return send_message( Command::DisplayFull, @null_message, @null_message.length )
    end

    def show_open( )
        puts "In #{__method__}"
        return send_message( Command::DisplayOpen, @null_message, @null_message.length )
    end

    def show_closed( )
        puts "In #{__method__}"
        return send_message( Command::DisplayClosed, @null_message, @null_message.length )
    end

    def show_blank( )
        puts "In #{__method__}"
        return send_message( Command::DisplayBlank, @null_message, @null_message.length )
    end

    def test_right_justify( message )
        puts "In #{__method__}"
        return right_justify_message( message, message.length )
    end

    private
    def calculate_checksum( packet )
        puts "In #{__method__}"
        checksum =  packet[ SendPacketBytePosition::SignAddress ] ^ packet[ SendPacketBytePosition::CommandType ]
        checksum = checksum ^ ( packet[ SendPacketBytePosition::Char0 ] ).to_i
        checksum = checksum ^ ( packet[ SendPacketBytePosition::Char1 ] ).to_i
        checksum = checksum ^ ( packet[ SendPacketBytePosition::Char2 ] ).to_i
        checksum = checksum ^ ( packet[ SendPacketBytePosition::Char3 ] ).to_i
        return checksum
    end

    def send_message( command, message, message_length )
        if( message_length > @max_chars_allowed_on_display )
            puts "#{__method__} Message too long"
            raise MessageTooLongException
        end
        puts "In #{__method__}, message_length = #{message_length}"
        packet = packet_factory( message, command )
        puts "In #{__method__} bytes:"
        print_bytes( packet )
        send_to_serial_port( packet )
    end

    def packet_factory( message, command )
        if( message.count > @max_chars_allowed_on_display )
            puts "#{__method__} Message too long (#{message.count} for #{@max_chars_allowed_on_display})"
            raise MessageTooLongException
        end
        full_packet = [ @SYN, @SYN, @STX, @address, command, @expect_response, message[ 0 ], message[ 1 ], message[ 2 ], message[ 3 ], 0, @ETX ]
        checksum = calculate_checksum( full_packet )
        full_packet[ SendPacketBytePosition::Checksum ] = checksum
        puts "Returning full packet in #{__method__}"
        return full_packet
    end
    
    def right_justify_message( message, message_length )
        return message.rjust( @max_chars_allowed_on_display, ' ' )
    end
    
    def print_bytes( packet )
        bytestream = "["
        for i in 0..packet.length-1 do
            bytestream = bytestream + " 0x%02X " % [ packet[ i ].to_i ]
        end
        bytestream = bytestream + "]"
        puts bytestream
    end
    def send_to_serial_port( message )
        puts "In #{__method__}"
        bytes_sent = @serial_port.write( message )
        puts "Sent #{bytes_sent} bytes."
    end

end

class SevenSegmentDisplayLibrary5Led < SevenSegmentDisplayLibrary
    # The constructor has the following inputs:
    #  port: the serial port shall be passed in via dependency injection.
    #           It is an object of type SerialPort
    #  address: this shall be a number. It indicates the sign's physical address.
    #  expect_response: this shall be a boolean, indicates whether we a
    #           response back from the sign.
    def initialize( serial_port, address, expect_response )
        super( serial_port, address, expect_response )
        @null_message = [ 0x00, 0x00, 0x00, 0x00, 0x00 ]
        @max_chars_allowed_on_display = 5
    end
    private
    def calculate_checksum( packet )
        puts "In #{__method__}"
        checksum =  packet[ SendPacketBytePosition5Led::SignAddress ] ^ packet[ SendPacketBytePosition5Led::CommandType ]
        checksum = checksum ^ ( packet[ SendPacketBytePosition5Led::Char0 ] ).to_i
        checksum = checksum ^ ( packet[ SendPacketBytePosition5Led::Char1 ] ).to_i
        checksum = checksum ^ ( packet[ SendPacketBytePosition5Led::Char2 ] ).to_i
        checksum = checksum ^ ( packet[ SendPacketBytePosition5Led::Char3 ] ).to_i
        checksum = checksum ^ ( packet[ SendPacketBytePosition5Led::Char4 ] ).to_i
        return checksum
    end
    def packet_factory( message, command )
        if( message.length > @max_chars_allowed_on_display )
            raise MessageTooLongException
        end
        full_packet = [ @SYN, @SYN, @STX, @address, command, @expect_response, message[ 0 ], message[ 1 ], message[ 2 ], message[ 3 ], message[ 4 ], 0, @ETX ]
        checksum = calculate_checksum( full_packet )
        full_packet[ SendPacketBytePosition5Led::Checksum ] = checksum
        return full_packet
    end
end
