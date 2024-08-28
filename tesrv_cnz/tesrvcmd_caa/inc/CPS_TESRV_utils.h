/*
 * utility.h
 *
 *  Created on: Mar 23, 2011
 *      Author: xtuangu
 */

#ifndef CPS_TESRV_UTILS_H_
#define CPS_TESRV_UTILS_H_

#include <string>

enum eSystemType
{
	eMultipleCP	= 0,
	eOneCP		= 1

};

class CPS_TESRV_utils
{
public:
	// Change source string to upper case
	static void toUpper(std::string& source);

	// Get CP system type
	static int getCPSysType(eSystemType& cpSystemType, bool& classic);

	//tesrvtran -f file is already run
	static bool multipleTesrvtranFile();

	//tesrvtran -f media is already run
	static bool multipleTesrvtranMedia();

private:
	CPS_TESRV_utils();
};
#endif /* CPS_TESRV_UTILS_H_ */
