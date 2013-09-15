/*
 * Timer.cpp
 *
 *  Created on: May 28, 2013
 *      Author: daniewang
 */

#include "Timer.h"

Timer::Timer() {

}

Timer::~Timer() {

}

void Timer::start(){
	m_start = boost::chrono::high_resolution_clock::now();
}

double Timer::stop(){
	boost::chrono::nanoseconds sec = boost::chrono::high_resolution_clock::now() - m_start;
	return sec.count() / 1e6;
}

