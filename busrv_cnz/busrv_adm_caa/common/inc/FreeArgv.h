#ifndef FREE_ARGV_BU_CMDS
#define FREE_ARGV_BU_CMDS

// These are for cout in FreeArgv
//#include <iostream>
//using namespace std;

class FreeArgv
{
public:
	FreeArgv(int c = 0) : mArgc(c) {}
	void operator()(char** argv)
	{
		//cout << "FreeArgv::operator()" << endl;
		if (argv == 0) {
		//	cout << "Nothing to delete" << endl;
			return;
		}
		char** p = argv;
		//cout << "Delete ";
		for(int ii = mArgc; ii > 0; --ii) {
		//	cout << *p << ' ';
			delete [] *p++;
		}
		//cout << endl << "Delete argv" << endl;
		delete [] argv;
	}
private:
	int mArgc;
};

#endif
