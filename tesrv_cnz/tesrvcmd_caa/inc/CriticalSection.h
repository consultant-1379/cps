/*
 * CriticalSection.h
 *
 *  Created on: Aug 21, 2010
 *      Author: xdtthng
 */

#ifndef CRITICALSECTION_H_
#define CRITICALSECTION_H_

#include "boost/thread/recursive_mutex.hpp"

class CriticalSection {

public:
	CriticalSection() : m_cs() { }
	~CriticalSection() {}
	void enter() { m_cs.lock(); }
	void leave() { m_cs.unlock(); }

private:
	CriticalSection(const CriticalSection& ); // not impl.
	CriticalSection& operator=(const CriticalSection& ); // -:-
	boost::recursive_mutex  m_cs;
};


class AutoCS
{
public:
	AutoCS(CriticalSection& cs) : m_cs(cs) { m_cs.enter(); }
	~AutoCS() { m_cs.leave(); }

private:
	AutoCS(const AutoCS& );
	AutoCS& operator=(const AutoCS& );

	CriticalSection& m_cs;
};


#endif /* CRITICALSECTION_H_ */
