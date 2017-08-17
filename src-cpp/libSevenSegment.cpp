#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <cassert>

#include "libSevenSegment.h"

const int MAX_PACKET_LENGTH = 20;

const int MAX_CHARS_ALLOWED_ON_DISPLAY = 4;
const int MAX_CHARS_ALLOWED_ON_DISPLAY_5LED = 5;

const int RECEIVE_BUFFER_SIZE = 0xff;

const char RESPONSE_ACK = 0x06;
const char RESPONSE_NAK = 0x15;

const char SYN = 0x16;
const char STX = 0x02;
const char ETX = 0x03;

class send_packet_byte_position
{
    public:
    const static int SYN0 = 0;
    const static int SYN1 = 1;
    const static int STX  = 2;
    const static int sign_address = 3;
    const static int command_type = 4;
    const static int response_packet = 5;
    const static int char0 = 6;
    const static int char1 = 7;
    const static int char2 = 8;
    const static int char3 = 9;
    const static int checksum = 10;
    const static int ETX = 11;
};

class send_packet_byte_position_5led : send_packet_byte_position
{
    public:
    const static int SYN0 = 0;
    const static int SYN1 = 1;
    const static int STX  = 2;
    const static int sign_address = 3;
    const static int command_type = 4;
    const static int response_packet = 5;
    const static int char0 = 6;
    const static int char1 = 7;
    const static int char2 = 8;
    const static int char3 = 9;
    const static int char4 = 10;
    const static int checksum = 11;
    const static int ETX = 12;
};

enum receive_packet_byte_positions
{
    receive_packet_byte_position_SYN0,
    receive_packet_byte_position_SYN1,
    receive_packet_byte_position_STX,
    receive_packet_byte_position_ACKNAK,
    receive_packet_byte_position_ETX,
    receive_packet_byte_position_last
};

enum command_types
{
    command_display_number_old,
    command_display_full,     // FULL
    command_display_open,     // OPEn
    command_display_closed,   // CLSd
    command_display_blank,
    command_display_positive_temperature,
    command_display_number,
    command_display_negative_temperature
};

char command = command_display_blank; // blank display

char expect_response = 0x00;    // 0x00 = we don't expect anything
                                // 0x01 = we expect responses

const char null_message[ ] = { 0x00, 0x00, 0x00, 0x00 };
const unsigned int null_message_length = sizeof( null_message ) / sizeof( null_message[ 0 ] );

SevenSegmentSignLibrary::SevenSegmentSignLibrary( const SerialPort* serialPort, const char sign_address, bool expectResponse ) :
    serialPort_( serialPort ),
    sign_address_( sign_address ),
    expectResponse_( expectResponse ),
    serialPortCreated_( false ),
    max_chars_allowed_on_display_( MAX_CHARS_ALLOWED_ON_DISPLAY )
{
    assert( serialPort );
}

SevenSegmentSignLibrary::SevenSegmentSignLibrary( const char* sPort, const char sign_address, bool expectResponse ) :
    sign_address_( sign_address ),
    expectResponse_( expectResponse ),
    serialPortCreated_( true ),
    max_chars_allowed_on_display_( MAX_CHARS_ALLOWED_ON_DISPLAY )
{
    serialPort_ = new SerialPort( sPort );
}

SevenSegmentSignLibrary::~SevenSegmentSignLibrary()
{
    if( serialPortCreated_ )
        delete serialPort_;
}


char SevenSegmentSignLibrary::CalculateChecksum( const char* packet, unsigned int packetLength ) const
{
    char checksum = packet[ send_packet_byte_position::sign_address ] ^ packet[ send_packet_byte_position::command_type ];
    checksum = checksum ^ packet[ send_packet_byte_position::char0 ];
    checksum = checksum ^ packet[ send_packet_byte_position::char1 ];
    checksum = checksum ^ packet[ send_packet_byte_position::char2 ];
    checksum = checksum ^ packet[ send_packet_byte_position::char3 ];
    return checksum;
}

int SevenSegmentSignLibrary::SendToSerialport( const char* message, unsigned int message_length )
{
    return serialPort_->Write( message, message_length );
}

int SevenSegmentSignLibrary::CheckVerdict( const char* packet, unsigned int packetSize ) const
{
    if( packetSize > receive_packet_byte_position_last )
        return -1;

    if( packet[ receive_packet_byte_position_ACKNAK ] == RESPONSE_ACK )
        return 1;
    else
        return 0;
}

int SevenSegmentSignLibrary::ShowNumberAsString( const char* number_as_string )
{
    if( strlen( number_as_string ) > max_chars_allowed_on_display_ )
    {
        std::cout << "Too many characters to show!" << std::endl;
        return -2;
    }

    std::cout << "ShowNumberAsString: \"" << number_as_string << "\" (" << strlen( number_as_string ) << "/" << max_chars_allowed_on_display_ << ")" << std::endl;
    // Right-justify the number
    unsigned int messageLength = strlen( number_as_string );
    char newMessage[ max_chars_allowed_on_display_ + 1 ];

    if( messageLength  <= max_chars_allowed_on_display_ )
    {
        strncpy( &newMessage[ max_chars_allowed_on_display_ - messageLength ], number_as_string, messageLength );
        memset( newMessage, 0x20, max_chars_allowed_on_display_ - messageLength );
    }
    return ShowGenericMessage( newMessage, max_chars_allowed_on_display_ );
}

int SevenSegmentSignLibrary::ShowNumber( int numberToDisplay )
{
    std::cout << "ShowNumber: " << numberToDisplay << std::endl;
    if( numberToDisplay > 9999 )
    {
        return -3;
    }

    std::stringstream streamNumber;
    streamNumber << numberToDisplay;

    if( streamNumber.str().length() <= max_chars_allowed_on_display_ )
        return ShowNumberAsString( streamNumber.str().c_str() );
    else
        return -1;
}

int SevenSegmentSignLibrary::ShowGenericMessage( const char* message, int messageLength )
{
    char allowedCharacters[] = { 'U', 'c', 'C', 'L', 'S', 'd', 'o', 'O', 'P', 'E', 'n', 'r', 'F', ' ', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0' };
    int allowedCharactersSize = sizeof( allowedCharacters ) / sizeof( allowedCharacters[ 0 ] );

    if( messageLength > max_chars_allowed_on_display_ )
    {
        std::cout << "Too many characters, unable to display." << std::endl;
        return -1;
    }

    // Disallow unsupported characters.
    bool bSupported;
    for( int i = 0; i < messageLength; i++ )
    {
        bSupported = false;
        for( int j = 0; j < allowedCharactersSize; j++ )
        {
            if( message[ i ] == message[ j ] )
            {
                bSupported = true;
                break;
            }
        }
    }
    if( !bSupported )
    {
        std::cout << "Unsupported characters found." << std::endl;
        return -2;
    }

    SendMessage( command_display_number, message, messageLength );
}

int SevenSegmentSignLibrary::PacketFactory( int command, const char* message, unsigned int messageLength, char* finalPacket, unsigned int finalPacketSize )
{
    char fullPacket[] = { SYN, SYN, STX, sign_address_, ( char )command, expect_response, message[ 0 ], message[ 1 ], message[ 2 ], message[ 3 ], 0, ETX };
    int fullPacketSize = sizeof( fullPacket ) / sizeof( fullPacket[ 0 ] );

    char checksum = CalculateChecksum( fullPacket, fullPacketSize );
    fullPacket[ send_packet_byte_position::checksum ] = checksum;

    memcpy( finalPacket, fullPacket, ( fullPacketSize < finalPacketSize ) ? fullPacketSize : finalPacketSize );

    return fullPacketSize;
}

int SevenSegmentSignLibrary::SendMessage( int command, const char* message, unsigned int messageLength )
{
    char finalPacket[ MAX_PACKET_LENGTH ];
    memset( finalPacket, 0, sizeof( finalPacket ) / sizeof( finalPacket[ 0 ] ) );

    // Construct a packet.
    int finalPacketSize = PacketFactory( command, message, messageLength, finalPacket, MAX_PACKET_LENGTH );

    PrintBytes( finalPacket, finalPacketSize );
    int bytesSent = SendToSerialport( finalPacket, finalPacketSize );
    std::cout << "Sent " << bytesSent << " bytes" << std::endl;

    int success = -1;
    if( expectResponse_ )
    {
        char receiveBuffer[ RECEIVE_BUFFER_SIZE ];
        memset( &receiveBuffer, 0, sizeof( receiveBuffer ) / sizeof( receiveBuffer[ 0 ] ) );

        int bytesReceived = serialPort_->Read( receiveBuffer, sizeof( receiveBuffer ) / sizeof( receiveBuffer[ 0 ] ) );
        success = CheckVerdict( receiveBuffer, bytesReceived );
    }
    else
    {
        success = bytesSent;
    }
    return success;
}

int SevenSegmentSignLibrary::ShowFullMessage( )
{ 
    return SendMessage( command_display_full, null_message, null_message_length );
}

int SevenSegmentSignLibrary::ShowOpenMessage( )
{
    return SendMessage( command_display_open, null_message, null_message_length );
}

int SevenSegmentSignLibrary::ShowClosedMessage( )
{
    return SendMessage( command_display_closed, null_message, null_message_length );
}

int SevenSegmentSignLibrary::ShowBlankMessage( )
{
    return SendMessage( command_display_blank, null_message, null_message_length );
}

int SevenSegmentSignLibrary::TestCalculateChecksum( )
{
    char packet[] = { SYN, SYN, STX, 0x07, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00 };

    char checksum = CalculateChecksum( packet, sizeof( packet ) / sizeof( packet[ 0 ] ) );

    fprintf( stdout, "Checksum: %02X", checksum );

    return 1;
}

void SevenSegmentSignLibrary::PrintBytes( const char* packet, unsigned int messageLength )
{
    for( int i = 0; i < messageLength; i++ )
    {
        fprintf( stdout, "0x%02X\t", packet[ i ] );

        std::cout << std::endl;
    }
}

SevenSegmentSignLibrary5Led::SevenSegmentSignLibrary5Led( const SerialPort* serialPort, const char sign_address, bool expectResponse ) 
    : SevenSegmentSignLibrary ( serialPort, sign_address, expectResponse )
{
    max_chars_allowed_on_display_ = MAX_CHARS_ALLOWED_ON_DISPLAY_5LED;
    std::cout << "5-segment LED selected" << std::endl;
}

int SevenSegmentSignLibrary5Led::PacketFactory( int command, const char* message, unsigned int messageLength, char* finalPacket, unsigned int finalPacketSize )
{
    char fullPacket[] = { SYN, SYN, STX, sign_address_, ( char )command, expect_response, message[ 0 ], message[ 1 ], message[ 2 ], message[ 3 ], message[4], 0, ETX };
    int fullPacketSize = sizeof( fullPacket ) / sizeof( fullPacket[ 0 ] );

    char checksum = CalculateChecksum( fullPacket, fullPacketSize );
    fullPacket[ send_packet_byte_position_5led::checksum ] = checksum;

    memcpy( finalPacket, fullPacket, ( fullPacketSize < finalPacketSize ) ? fullPacketSize : finalPacketSize );

    return fullPacketSize;
}

char SevenSegmentSignLibrary5Led::CalculateChecksum( const char* packet, unsigned int packetLength ) const
{
    char checksum = packet[ send_packet_byte_position::sign_address ] ^ packet[ send_packet_byte_position::command_type ];
    checksum = checksum ^ packet[ send_packet_byte_position_5led::char0 ];
    checksum = checksum ^ packet[ send_packet_byte_position_5led::char1 ];
    checksum = checksum ^ packet[ send_packet_byte_position_5led::char2 ];
    checksum = checksum ^ packet[ send_packet_byte_position_5led::char3 ];
    checksum = checksum ^ packet[ send_packet_byte_position_5led::char4 ];
    return checksum;
}

int SevenSegmentSignLibrary5Led::ShowBlankMessage( )
{
    char null_message[ ] = { 0x00, 0x00, 0x00, 0x00, 0x00 };
    int null_message_length = sizeof( null_message ) / sizeof( null_message[ 0 ] );
    return SendMessage( command_display_blank, null_message, null_message_length );
}

seven_segment_library_t lib_sevensegment_new(                 const char* string_port_name, const char sign_address )
{
    SevenSegmentSignLibrary* libSS = new SevenSegmentSignLibrary( string_port_name, sign_address );
    return static_cast< seven_segment_library_t >( libSS );
}

void  lib_sevensegment_delete(              seven_segment_library_t lib )
{
    SevenSegmentSignLibrary* libSS = static_cast< SevenSegmentSignLibrary* >( lib );
    delete libSS;
}

int   lib_sevensegment_show_full_message(   seven_segment_library_t lib )
{
    SevenSegmentSignLibrary* libSS = static_cast< SevenSegmentSignLibrary* >( lib );
    return libSS->ShowFullMessage();
}

int   lib_sevensegment_show_open_message(   seven_segment_library_t lib )
{
    SevenSegmentSignLibrary* libSS = static_cast< SevenSegmentSignLibrary* >( lib );
    return libSS->ShowOpenMessage();
}

int   lib_sevensegment_show_closed_message( seven_segment_library_t lib )
{
    SevenSegmentSignLibrary* libSS = static_cast< SevenSegmentSignLibrary* >( lib );
    return libSS->ShowClosedMessage();
}

int   lib_sevensegment_show_blank_message(  seven_segment_library_t lib )
{
    SevenSegmentSignLibrary* libSS = static_cast< SevenSegmentSignLibrary* >( lib );
    return libSS->ShowBlankMessage();
}

int   lib_sevensegment_show_number(         seven_segment_library_t lib, int num_to_display )
{
    SevenSegmentSignLibrary* libSS = static_cast< SevenSegmentSignLibrary* >( lib );
    return libSS->ShowNumber( num_to_display );
}
