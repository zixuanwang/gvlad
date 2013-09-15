/*
 * Angle.h
 *
 *  Created on: Jun 20, 2013
 *      Author: daniewang
 */

#ifndef ANGLE_H_
#define ANGLE_H_

#include <cmath>
#include <fstream>
#include "GlobalConfig.h"
#include <iostream>
#include <map>
#include <string>

class Angle {
public:
	// get the corresponding angle bin index.
	// number of bins and angle offset are set in GlobalConfig file.
	static int get_index(float value);
	// return the pointer to the singleton.
	static Angle* instance();
	void load(const std::string& file);
	// normalize the angle between [0, 360].
	float normalize(float value);
private:
	Angle();
	Angle(const Angle&);
	Angle& operator=(const Angle&);
	static Angle* p_instance;
	std::map<int, float> m_pmf;
	std::map<int, float> m_cmf;
};

#endif /* ANGLE_H_ */
