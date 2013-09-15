/*
 * Timer.h
 *
 *  Created on: May 28, 2013
 *      Author: daniewang
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <boost/chrono.hpp>

class Timer {
public:
	Timer();
	~Timer();
	void start();
	double stop(); // return time elapsed in milliseconds.
private:
	boost::chrono::high_resolution_clock::time_point m_start;
};

#endif /* TIMER_H_ */
