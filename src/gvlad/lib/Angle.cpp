/*
 * Angle.cpp
 *
 *  Created on: Jun 20, 2013
 *      Author: daniewang
 */

#include "Angle.h"

Angle* Angle::p_instance = NULL;

Angle::Angle(){

}

int Angle::get_index(float value) {
	float step = 360.f / (float) GlobalConfig::angle_bins;
	float angle = value - GlobalConfig::angle_offset;
	if (angle < 0.f)
		angle += 360.f;
	int index = (int) (angle / step);
	if (index >= 0 && index < GlobalConfig::angle_bins)
		return index;
	return 0;
}

Angle* Angle::instance() {
	if (p_instance == NULL) {
		p_instance = new Angle;
	}
	return p_instance;
}

void Angle::load(const std::string& file) {
	int angle_count = 0;
	std::map<int, int> count_map;
	std::ifstream in_stream;
	in_stream.open(file.c_str());
	if (in_stream.good()) {
		float angle;
		while (in_stream >> angle) {
			if (angle > 0.f) {
				count_map[(int) angle]++;
				angle_count++;
			}
		}
		for (std::map<int, int>::iterator iter = count_map.begin();
				iter != count_map.end(); ++iter) {
			m_pmf[iter->first] = (float) iter->second / (float) angle_count;
		}
		in_stream.close();
	}
	float sum = 0.f;
	for (std::map<int, float>::iterator iter = m_pmf.begin();
			iter != m_pmf.end(); ++iter) {
		sum += iter->second;
		m_cmf[iter->first] = sum;
	}
	// debug
	for (std::map<int, float>::iterator iter = m_cmf.begin();
			iter != m_cmf.end(); ++iter) {
		std::cout << iter->first << "\t" << iter->second << std::endl;
	}
}

float Angle::normalize(float value) {
	return m_cmf[(int) value] * 360.f;
}

