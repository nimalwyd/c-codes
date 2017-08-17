#ifndef __NULLPOINTEREXCEPTION_H__
#define __NULLPOINTEREXCEPTION_H__

#include <stdexcept>

class NullPointerException : public std::runtime_error
{
    public:
    NullPointerException( ) : std::runtime_error( "Null pointer exception." ){};
};
#endif // __NULLPOINTEREXCEPTION_H__