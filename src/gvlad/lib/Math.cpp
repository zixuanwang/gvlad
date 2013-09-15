/*
 * Math.cpp
 *
 *  Created on: May 30, 2013
 *      Author: daniewang
 */

#include "Math.h"

const float Math::PI = 3.1415926f;

Math::Math() {

}

Math::~Math() {

}

float Math::l1_norm(const std::vector<float>& array) {
	float sum = 0.f;
	for (size_t i = 0; i < array.size(); ++i) {
		sum += array[i];
	}
	return sum;
}

void Math::l1_normalize(std::vector<float>& array) {
	float norm = l1_norm(array);
	if (norm == 0.f)
		return;
	for (size_t i = 0; i < array.size(); ++i) {
		array[i] /= norm;
	}
}

float Math::l2_norm(const std::vector<float>& array) {
	float sum = 0.f;
	for (size_t i = 0; i < array.size(); ++i) {
		sum += array[i] * array[i];
	}
	return sqrtf(sum);
}

void Math::l2_normalize(std::vector<float>& array) {
	float norm = l2_norm(array);
	if (norm == 0.f)
		return;
	for (size_t i = 0; i < array.size(); ++i) {
		array[i] /= norm;
	}
}

void Math::square_root(std::vector<float>& array) {
	for (size_t i = 0; i < array.size(); ++i) {
		array[i] = sqrtf(array[i]);
	}
}

void Math::power_normalize(cv::Mat& mat) {
	for (int i = 0; i < mat.rows; ++i) {
		float* ptr = mat.ptr<float>(i);
		for (int j = 0; j < mat.cols; ++j) {
			if (ptr[j] >= 0.f)
				ptr[j] = sqrtf(ptr[j]);
			else
				ptr[j] = -1.f * sqrtf(-1.f * ptr[j]);
		}
	}
}

void Math::z_normalize(cv::Mat& mat) {
	for (int x = 0; x < mat.cols; ++x) {
		cv::Mat col = mat.col(x);
		cv::Scalar mean;
		cv::Scalar stddev;
		cv::meanStdDev(col, mean, stddev);
		if(stddev.val[0] != 0)
			col = (col - mean.val[0]) / stddev.val[0];
	}
}

void Math::row_normalize(cv::Mat& mat) {
	for (int i = 0; i < mat.rows; ++i) {
		cv::normalize(mat.row(i), mat.row(i));
	}
}
