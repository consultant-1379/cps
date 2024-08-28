#include "CPCmdOptions.h"
#include "InvalidValueError.h"
#include <cstring>

const char* InvalidValueError::getValue()
{
	if (strcmp(mValue, CPCmdOption::cpOptionLong) == 0)
		return CPCmdOption::cpOption;
	else if (strcmp(mValue, CPCmdOption::cpOptionLongFake) == 0)
		return CPCmdOption::cpOptionLong;
	else 
		return mValue;
}
