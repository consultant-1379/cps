#ifndef INVALID_VALUE_ERROR_EXCEPTION
#define INVALID_VALUE_ERROR_EXCEPTION

class InvalidValueError
{
public:
	InvalidValueError(const char* v, char opt) : mValue(v), mOpt(opt) {}
	const char* getValue();
	char getOpt() const { return mOpt; }
	
private:
	const char*	mValue;
	char		mOpt;
};


#endif
