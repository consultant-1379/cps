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
	CriticalSection() : m_cs(), m_count(0) { }
	~CriticalSection() {}
	void enter() { m_cs.lock(); ++m_count; }
	void leave() { m_cs.unlock(); --m_count; }
	long int getCounter() const { return m_count; }
	
private:
	CriticalSection(const CriticalSection& ); 
	CriticalSection& operator=(const CriticalSection& );
	boost::recursive_mutex  m_cs;
	
	long int m_count;
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
