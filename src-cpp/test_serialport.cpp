#include <ostream>
#include <string.h>
#include "InvalidOperationException.h"
#include "NullPointerException.h"
#include "serialport.h"

const char* const goodPort = "/dev/ttyUSB0";
const char* const badPort  = "/dev/ttyS1";

#define PRINTRESULT( a ) { if( a ) std::cout << "**** Success *****" << std::endl; else std::cout << "**** Failure *****" << std::endl; };


void ShowFunctionName( const char* string_function_name )
{
    std::cout << "In function: " << string_function_name << std::endl;
}

bool TestNullInputToConstructor()
{
    ShowFunctionName( __FUNCTION__ );
    bool expectedResult = false;
    try
    {
        SerialPort serialPort = SerialPort( NULL );
    }
    catch( NullPointerException& ex )
    {
        std::cout<< "Caught exception: " << ex.what() << std::endl;
        expectedResult = true;
    }
    return expectedResult;
}

bool TestUnableToOpenPort()
{
    ShowFunctionName( __FUNCTION__ );
    bool expectedResult = false;
    try
    {
        SerialPort serialPort = SerialPort( badPort );
    }
    catch( InvalidOperationException& ex )
    {
        std::cout << "Caught exception: " << ex.what() << std::endl;
        expectedResult = true;
    }
    return expectedResult;
}

bool TestSuccessfulOpenPort()
{
    ShowFunctionName( __FUNCTION__ );
    bool expectedResult = false;
    try
    {
        SerialPort serialPort = SerialPort( goodPort );
        expectedResult = true;
    }
    catch( InvalidOperationException& ex )
    {
        std::cout << "Caught exception: " << ex.what() << std::endl;
    }
    return expectedResult;
}

bool TestUnsuccessfulWriteToSerialPort()
{
    ShowFunctionName( __FUNCTION__ );
    bool expectedResult = false;
    try
    {
        char* message = (char *)"ZYWXT";
        SerialPort serialPort = SerialPort( badPort );
        serialPort.Write( message, strlen( message ) );
    }
    catch( InvalidOperationException& ex )
    {
        std::cout << "Caught exception: " << ex.what() << std::endl;
        expectedResult = true;
    }
    return expectedResult;
}

bool TestSuccesfulWriteToSerialPort( )
{
    ShowFunctionName( __FUNCTION__ );
    bool expectedResult = false;
    try
    {
        char* message = ( char* )"1234ABC";
        SerialPort serialPort = SerialPort( goodPort );
        serialPort.Write( message, strlen( message ) );
        expectedResult = true;
    }
    catch( InvalidOperationException& ex )
    {
        std::cout << "Caught exception: " << ex.what() << std::endl;
    }
    return expectedResult;
}

int main( void )
{
    std::cout << "Testing serial port" << std::endl;
    PRINTRESULT( TestNullInputToConstructor() );
    PRINTRESULT( TestUnableToOpenPort() );
    PRINTRESULT( TestSuccessfulOpenPort() );

    PRINTRESULT( TestUnsuccessfulWriteToSerialPort() );
    PRINTRESULT( TestSuccesfulWriteToSerialPort() );

   return 0;
}
