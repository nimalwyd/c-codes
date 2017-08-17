#ifndef __LIBSEVENSEGMENTSIGN_H__
#define __LIBSEVENSEGMENTSIGN_H__

#include "serialport.h"

#ifdef __cplusplus
class SevenSegmentSignLibrary
{
    public:

    SevenSegmentSignLibrary( const SerialPort* serialPort, const char sign_address, bool expectResponse = false);
    SevenSegmentSignLibrary( const char* sPort, const char sign_address, bool expectResponse= false );
    virtual ~SevenSegmentSignLibrary();

    int ShowNumber( int numberToDisplay );
    int ShowNumberAsString( const char* number_as_string );
    int ShowGenericMessage( const char* message, int messageLength );
    int ShowFullMessage( );
    int ShowOpenMessage( );
    int ShowClosedMessage( );
    int ShowBlankMessage( );

    protected:
    virtual int PacketFactory( int command, const char* message, unsigned int messageLength, char* finalPacket, unsigned int finalPacketLength );
    virtual char CalculateChecksum( const char* packet, unsigned int packetLength ) const;
    int SendMessage( int command, const char* message, unsigned int messageLength );

    const char sign_address_;
    int max_chars_allowed_on_display_;

    private:
    int CheckVerdict( const char* packet, unsigned int packetLength ) const;

    int TestCalculateChecksum( );

    int SendToSerialport( const char* message, unsigned int messageLength );

    void PrintBytes( const char* packet, unsigned int messageLength );

    const SerialPort* serialPort_;
    bool expectResponse_;
    bool serialPortCreated_;
};

class SevenSegmentSignLibrary5Led : public SevenSegmentSignLibrary
{
    public:
    SevenSegmentSignLibrary5Led( const SerialPort* serialPort, const char sign_address, bool expectResponse = false );
    int ShowBlankMessage( );

    private:
    int PacketFactory( int command, const char* message, unsigned int messageLength, char* finalPacket, unsigned int finalPacketLength );
    char CalculateChecksum( const char* packet, unsigned int packetLength ) const;
    bool VerifyMessageLengthIsWithinLimits( unsigned int messageLength ) const;
};
#endif // __cplusplus

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus (extern "C" {)
typedef void* seven_segment_library_t;

seven_segment_library_t lib_sevensegment_new( const char* string_port_name, const char sign_address );
void  lib_sevensegment_delete(                 seven_segment_library_t lib );
int   lib_sevensegment_show_full_message(      seven_segment_library_t lib );
int   lib_sevensegment_show_open_message(      seven_segment_library_t lib );
int   lib_sevensegment_show_closed_message(    seven_segment_library_t lib );
int   lib_sevensegment_show_blank_message(     seven_segment_library_t lib );
int   lib_sevensegment_show_number(            seven_segment_library_t lib, int num_to_display );
#ifdef __cplusplus
}
#endif // __cplusplus (}) 

#endif
