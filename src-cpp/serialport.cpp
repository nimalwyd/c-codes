#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include "NullPointerException.h"
#include "InvalidOperationException.h"
#include "serialport.h"
#include "ErrorMessageFactory.h"

extern "C" void close( int );
extern "C" int write( int, const char*, int );
extern "C" ssize_t read(int fd, void *buf, size_t count);
extern "C" int errno;

SerialPort::SerialPort( const char* sPort ) : fd_( 0 ), openFlag_( false )
{
    if( sPort == NULL ) throw NullPointerException();

    fd_ = open( sPort, O_RDWR | O_NOCTTY );
    std::cout << "Attemping to open port: " << sPort << ". Result = " << fd_ << std::endl;
    if( fd_ < 0 )
        throw InvalidOperationException( ErrorMessageFactory::Create( "SerialPort", __FUNCTION__, "Unable to open port", sPort, errno ) );

    memset( &currentTerminalSettings_, 0, sizeof( currentTerminalSettings_ ) );
    memset( &originalTerminalSettings_, 0, sizeof( originalTerminalSettings_ ) );

    OpenPort( );
}

SerialPort::~SerialPort( )
{
    ClosePort( );
}

bool SerialPort::OpenPort( )
{
    // first get the settings
    tcgetattr( fd_, &originalTerminalSettings_ );

    memset( &currentTerminalSettings_, 0, sizeof( currentTerminalSettings_ ) );

    /*
    BAUDRATE: Set bps rate. You could also use cfsetispeed and cfsetospeed.
    CS8     : 8n1 (8bit,no parity,1 stopbit)
    */
    currentTerminalSettings_.c_cflag = B9600 | CS8 ;

    currentTerminalSettings_.c_iflag = IGNPAR | ICRNL;

    /* Set up all control characters */
    currentTerminalSettings_.c_cc[ VINTR ]    = 0;     /* Ctrl-c */
    currentTerminalSettings_.c_cc[ VQUIT ]    = 0;     /* Ctrl-\ */
    currentTerminalSettings_.c_cc[ VERASE ]   = 0;     /* del */
    currentTerminalSettings_.c_cc[ VKILL ]    = 0;     /* @ */
    currentTerminalSettings_.c_cc[ VEOF ]     = 4;     /* Ctrl-d */
    currentTerminalSettings_.c_cc[ VTIME ]    = 0;     /* inter-character timer unused */
    currentTerminalSettings_.c_cc[ VMIN ]     = 1;     /* blocking read until 1 character arrives */
    currentTerminalSettings_.c_cc[ VSWTC ]    = 0;     /* '\0' */
    currentTerminalSettings_.c_cc[ VSTART ]   = 0;     /* Ctrl-q */
    currentTerminalSettings_.c_cc[ VSTOP ]    = 0;     /* Ctrl-s */
    currentTerminalSettings_.c_cc[ VSUSP ]    = 0;     /* Ctrl-z */
    currentTerminalSettings_.c_cc[ VEOL ]     = 0;     /* '\0' */
    currentTerminalSettings_.c_cc[ VREPRINT ] = 0;     /* Ctrl-r */
    currentTerminalSettings_.c_cc[ VDISCARD ] = 0;     /* Ctrl-u */
    currentTerminalSettings_.c_cc[ VWERASE ]  = 0;     /* Ctrl-w */
    currentTerminalSettings_.c_cc[ VLNEXT ]   = 0;     /* Ctrl-v */
    currentTerminalSettings_.c_cc[ VEOL2 ]    = 0;     /* '\0' */

    tcflush( fd_, TCIFLUSH );
    tcsetattr( fd_, TCSANOW, &currentTerminalSettings_ );

    openFlag_ = true;

    return true;
}

bool SerialPort::ClosePort( )
{
    tcflush( fd_, TCIFLUSH );
    tcsetattr( fd_, TCSANOW, &originalTerminalSettings_ );
    close( fd_ );
    openFlag_ = false;
}

int SerialPort::Write( const char *buffer, unsigned int bufferSize ) const
{
    if( !IsOpen() )
        throw InvalidOperationException( );

    return write( fd_, buffer, bufferSize );
}

int SerialPort::Read( char* receiveBuffer, unsigned int bufferSize ) const
{
    if( !IsOpen() )
        throw InvalidOperationException( );

    int bytesRead = 0;
    bytesRead = read( fd_, receiveBuffer, bufferSize );
    return bytesRead;
}

bool SerialPort::IsOpen( ) const
{
    return openFlag_;
}

serial_port_t serial_port_new( const char* string_port )
{
    SerialPort* sp = new SerialPort( string_port );
    return static_cast< serial_port_t >( sp );
}

void  serial_port_delete( serial_port_t serial_port )
{
    SerialPort* sp = static_cast< SerialPort* >( serial_port );
    delete sp;
}

int   serial_port_write( serial_port_t serial_port, const char* buffer, unsigned int buffer_size )
{
    SerialPort* sp = static_cast< SerialPort* >( serial_port );
    return sp->Write( buffer, buffer_size );
}

int   serial_port_read( serial_port_t serial_port, char* receive_buffer, unsigned int buffer_size )
{
    SerialPort* sp = static_cast< SerialPort* >( serial_port );
    return sp->Read( receive_buffer, buffer_size );
}

int   serial_port_is_open( serial_port_t serial_port )
{
    SerialPort* sp = static_cast< SerialPort* >( serial_port );
    return sp->IsOpen( );
}
