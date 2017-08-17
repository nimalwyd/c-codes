#ifndef __SERIALPORT_H__
#define __SERIALPORT_H__

#include <termios.h>

#ifdef __cplusplus
class SerialPort
{
    public:
    SerialPort( const char* sPort );
    ~SerialPort();

    int Write( const char *buffer, unsigned int bufferSize ) const;
    int Read( char *receiveBuffer, unsigned int bufferSize) const;
    bool IsOpen() const;

    private:
    int fd_;

    struct termios originalTerminalSettings_;
    struct termios currentTerminalSettings_;

    bool OpenPort( );
    bool ClosePort( );
    bool openFlag_;
};
#endif // __cplusplus (SerialPort)

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus (extern "C" {)
typedef void* serial_port_t;
serial_port_t serial_port_new(      const char* string_port );
void  serial_port_delete(           serial_port_t serial_port );
int   serial_port_write(            serial_port_t serial_port, const char* buffer, unsigned int buffer_size );
int   serial_port_read(             serial_port_t serial_port, const char* receive_buffer, unsigned int buffer_size );
int   serial_port_is_open(          serial_port_t serial_port );
#ifdef __cplusplus
}
#endif // __cplusplus (})

#endif // __SERIALPORT_H__
