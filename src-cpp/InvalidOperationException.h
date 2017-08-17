#ifndef __INVALIDOPERATIONEXCEPTION_H__
#define __INVALIDOPERATIONEXCEPTION_H__

#include <stdexcept>

class InvalidOperationException : public std::runtime_error
{
    public:
    InvalidOperationException( ) : std::runtime_error( "Invalid operation attempted" ) {}
    InvalidOperationException( const char* pError ) : std::runtime_error( pError ) {}
};
#endif // __INVALIDOPERATIONEXCEPTION_H__