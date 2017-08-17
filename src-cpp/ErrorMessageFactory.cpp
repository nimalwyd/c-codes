#include <sstream>
#include "ErrorMessageFactory.h"

const char* ErrorMessageFactory::Create( const char* className, const char* functionName, const char* errorMessage, const char* errorMessageDetails, int errorCode )
{
    std::stringstream fullErrorMessageStream;
    fullErrorMessageStream << "ERROR: {" << className << "::" << functionName << "} " << errorMessage << "--" << errorMessageDetails << " Error Code: (" << errorCode << ")" << std::endl;
    return fullErrorMessageStream.str().c_str();
}

