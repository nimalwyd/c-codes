#include "serialport.h"
#include "libSevenSegment.h"
#include <thread>
#include <iostream>

void magicsleep( int seconds, int microseconds )
{
    struct timeval tv;
    tv.tv_sec = seconds;
    tv.tv_usec = microseconds;
    select( 0, NULL, NULL, NULL, &tv );
}

void show_current_time( const SevenSegmentSignLibrary* ssDisplay )
{
    time_t now;
    time( &now );
    struct tm *now_tm = localtime( &now );
    char time_display[ 5 ];
    strftime( time_display, 5, "%H%M", now_tm );
    std::cout << "Time is now: " << time_display << std::endl;
    ssDisplay->ShowGenericMessage( time_display, 4 );
}

int main( void )
{
    SerialPort serialPort( "/dev/ttyUSB0" );
    SevenSegmentSignLibrary ssDisplay = SevenSegmentSignLibrary( &serialPort );

    std::cout << "Showing open" << std::endl;
    std::cout <<  "Result: " << ssDisplay.ShowOpenMessage( ) << std::endl;

    magicsleep( 5, 0 );
    std::cout << "Showing door" << std::endl;
    std::cout << "Result: " << ssDisplay.ShowGenericMessage( "door", 4 ) << std::endl;

    magicsleep( 5, 0 );
    std::cout << "Showing Full" << std::endl;
    std::cout << "Result: " << ssDisplay.ShowFullMessage( ) << std::endl;

    magicsleep( 5, 0 );
    std::cout << "Showing ShowNumber" << std::endl;
    std::cout << "Result: " << ssDisplay.ShowNumber( 42 ) << std::endl;

    magicsleep( 5, 0 );
    std::cout << "Showing ShowNumber" << std::endl;
    std::cout << "Result: " << ssDisplay.ShowNumber( 666 ) << std::endl;

    magicsleep( 5, 0 );
    std::cout << "Showing ShowNumber" << std::endl;
    std::cout << "Result: " << ssDisplay.ShowNumber( 1234 ) << std::endl;

    magicsleep( 5, 0 );
    std::cout << "Showing Closed" << std::endl;
    std::cout << "Result: " << ssDisplay.ShowClosedMessage( ) << std::endl;

    magicsleep( 5, 0 );
    std::cout << "Showing Closed" << std::endl;
    std::cout << "Result: " <<  ssDisplay.ShowBlankMessage( ) << std::endl;
    //int ShowNumber( int numberToDisplay );
    //int ShowNumberAsString( const char* number_as_string );
    //int ShowGenericMessage( const char* message, int messageLength );
    //int ShowFullMessage( );
    //int ShowClosedMessage( );
    //int ShowBlankMessage( );
    while( 1 )
    {
        show_current_time( &ssDisplay );
        magicsleep( 60, 0 );
    }
    return 0;
}
