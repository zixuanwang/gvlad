/*
 * Math.h
 *
 *  Created on: May 30, 2013
 *      Author: daniewang
 */

#ifndef MATH_H_
#define MATH_H_

#include <boost/unordered_map.hpp>
#include <opencv2/opencv.hpp>

class Math {
public:
	Math();
	~Math();
	static float l1_norm(const std::vector<float>& array);
	static void l1_normalize(std::vector<float>& array);
	static float l2_norm(const std::vector<float>& array);
	static void l2_normalize(std::vector<float>& array);
	static void square_root(std::vector<float>& array);
	static int majority(const std::vector<int>& array) {
		boost::unordered_map<int, int> map;
		for (size_t i = 0; i < array.size(); ++i) {
			map[array[i]]++;
		}
		int majority;
		int max_count = -1;
		for (size_t i = 0; i < array.size(); ++i) {
			int count = map[array[i]];
			if (count > max_count) {
				max_count = count;
				majority = array[i];
			}
		}
		return majority;
	}
	// matrix should be CV_32FC1.
	// val = sign(val) * sqrt(|val|)
	static void power_normalize(cv::Mat& mat);
	// z normalize to each column.
	static void z_normalize(cv::Mat& mat);
	// normalize each row.
	static void row_normalize(cv::Mat& mat);
	const static float PI;
};

#endif /* MATH_H_ */
