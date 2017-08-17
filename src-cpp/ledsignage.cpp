#include "libSevenSegment.h"
#include "serialport.h"
#include <sstream>
#include <iostream>
#include <boost/program_options.hpp>

const char* const SERIAL_PORT_OPTION = "device";
const char* const BAUDRATE_OPTION = "baud";
const char* const ADDRESS_OPTION = "addr";
const char* const DISPLAY_OPTION_FULLMESSAGE = "full";
const char* const DISPLAY_OPTION_OPENMESSAGE = "open";
const char* const DISPLAY_OPTION_CLOSEDMESSAGE = "closed";
const char* const DISPLAY_OPTION_BLANKMESSAGE = "blank";
const char* const DISPLAY_OPTION_NUMMESSAGE = "num";
const char* const FIVELEDDISPLAY_OPTION = "led5";

int main( int argc, char* argv[] )
{
    boost::program_options::options_description desc( "Allowed Options" );
    desc.add_options()
        ( SERIAL_PORT_OPTION, boost::program_options::value< std::string >()->required(), "Serial port device." )
        ( BAUDRATE_OPTION, boost::program_options::value< int >()->required(), "The baudrate of the serial port.")
        ( ADDRESS_OPTION, boost::program_options::value< int >()->required(), "The address of the sign")
        ( DISPLAY_OPTION_FULLMESSAGE, "Display FULL message." )
        ( DISPLAY_OPTION_OPENMESSAGE, "Display OPEN message." )
        ( DISPLAY_OPTION_CLOSEDMESSAGE, "Display clsd message." )
        ( DISPLAY_OPTION_BLANKMESSAGE, "Blank display" )
        ( DISPLAY_OPTION_NUMMESSAGE, boost::program_options::value< int >(), "The number to display" )
        ( FIVELEDDISPLAY_OPTION, "Use the 5-LED sign" )
    ;

    boost::program_options::variables_map variable_map;
    boost::program_options::store( boost::program_options::parse_command_line( argc, argv, desc ), variable_map );
    boost::program_options::notify( variable_map );

    if( variable_map.count( DISPLAY_OPTION_NUMMESSAGE ) )
    {
        std::cout << DISPLAY_OPTION_NUMMESSAGE << ":" << variable_map[ DISPLAY_OPTION_NUMMESSAGE ].as< int >() << std::endl;
    }
    SerialPort sp = SerialPort( variable_map[ SERIAL_PORT_OPTION ].as< std::string>().c_str() );
    SevenSegmentSignLibrary* lib;
    if( variable_map.count( FIVELEDDISPLAY_OPTION ) )
    {
        lib = new SevenSegmentSignLibrary5Led( &sp, static_cast< char >( variable_map[ ADDRESS_OPTION ].as< int >() ) );
    }
    else
    {
        lib = new SevenSegmentSignLibrary( &sp, static_cast< char >( variable_map[ ADDRESS_OPTION ].as< int >() ) );
    }
    
    if( variable_map.count( DISPLAY_OPTION_FULLMESSAGE) )
    {
        std::cout << "Displaying Full message." << std::endl;
        lib->ShowFullMessage();
    }
    else if( variable_map.count( DISPLAY_OPTION_OPENMESSAGE ) )
    {
        std::cout << "Displaying Open message." << std::endl;
        lib->ShowOpenMessage();
    }
    else if( variable_map.count( DISPLAY_OPTION_CLOSEDMESSAGE ) )
    {
        std::cout << "Displaying Closed message." << std::endl;
        lib->ShowClosedMessage();
    }
    else if( variable_map.count( DISPLAY_OPTION_BLANKMESSAGE ) )
    {
        std::cout << "Displaying Blank message." << std::endl;
        lib->ShowBlankMessage();
    }
    else if( variable_map.count( DISPLAY_OPTION_NUMMESSAGE) )
    {
        std::cout << "Displaying number." << std::endl;
        lib->ShowNumber( variable_map[ DISPLAY_OPTION_NUMMESSAGE ].as< int >() );
    }
    delete lib;
    return 0;
}