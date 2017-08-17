#ifndef __ERRORMESSAGEFACTORY_H__
#define __ERRORMESSAGEFACTORY_H__

class ErrorMessageFactory
{
    public:
    static const char* Create( const char* className, const char* functionName, const char* errorMessage, const char* errorMessageDetails, int errorCode );
};

#endif // _ERRORMESSAGEFACTORY_H__
