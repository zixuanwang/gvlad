/*
 * GeometricVerifier.cpp
 *
 *  Created on: Aug 14, 2013
 *      Author: daniewang
 */

#include "GeometricVerifier.h"

GeometricVerifier::GeometricVerifier() {

}

GeometricVerifier::~GeometricVerifier() {

}

void GeometricVerifier::prepare(const GeoDescriptorVector<float>& query_geo) {
	query_geo.convert(m_query_descriptor_mat);
	m_index = boost::shared_ptr<cv::flann::Index>(
			new cv::flann::Index(m_query_descriptor_mat,
					cv::flann::KDTreeIndexParams(1)));
	m_query_geo.copy_from(query_geo);
}

bool GeometricVerifier::verify(
		const GeoDescriptorVector<float>& candidate_geo) const {
	cv::Mat candidate_descriptor_mat;
	candidate_geo.convert(candidate_descriptor_mat);
	cv::Mat index_mat;
	cv::Mat dist_mat;
	m_index->knnSearch(candidate_descriptor_mat, index_mat, dist_mat, 1,
			cv::flann::SearchParams(16));
	int count = candidate_descriptor_mat.rows;
	cv::Mat src_points(count, 2, CV_32FC1, cv::Scalar(0));
	cv::Mat dst_points(count, 2, CV_32FC1, cv::Scalar(0));
	for (int i = 0; i < count; ++i) {
		int index = index_mat.ptr<int>(i)[0];
		float* src_ptr = src_points.ptr<float>(i);
		float* dst_ptr = dst_points.ptr<float>(i);
		src_ptr[0] = m_query_geo.geo_descriptor_vector[index].x;
		src_ptr[1] = m_query_geo.geo_descriptor_vector[index].y;
		dst_ptr[0] = candidate_geo.geo_descriptor_vector[i].x;
		dst_ptr[1] = candidate_geo.geo_descriptor_vector[i].y;
	}
	cv::Mat affine = get_affine(src_points, dst_points);
	return !affine.empty();
}

cv::Mat GeometricVerifier::get_affine(const cv::Mat& src_points,
		const cv::Mat& dst_points, int k) const {
	int count = src_points.rows;
	if (count < 3) {
		return cv::Mat();
	}
	float threshold_square = 6.f;
	std::vector<int> index_array(count, 0);
	for (int i = 1; i < count; ++i) {
		index_array[i] = i;
	}
	const float* src_ptr = src_points.ptr<float>(0);
	const float* dst_ptr = dst_points.ptr<float>(0);
	CvPoint2D32f src_array[3];
	CvPoint2D32f dst_array[3];
	double model_array[6];
	float f_model_array[6];
	float best_model_array[6];
	CvMat model_mat = cvMat(2, 3, CV_64FC1, model_array);
	int best_support = 0;
	for (int i = 0; i < k; ++i) {
		int support = 0;
		for (int j = 0; j < 3; ++j) {
			std::swap(index_array[j], index_array[rand() % count]);
		}
		for (int j = 0; j < 3; ++j) {
			int index = index_array[j];
			src_array[j].x = src_points.ptr<float>(index)[0];
			src_array[j].y = src_points.ptr<float>(index)[1];
			dst_array[j].x = dst_points.ptr<float>(index)[0];
			dst_array[j].y = dst_points.ptr<float>(index)[1];
		}
		cvGetAffineTransform(src_array, dst_array, &model_mat);
		std::copy(model_array, model_array + 6, f_model_array);
		for (int j = 0; j < count; ++j) {
			int ii = 2 * j;
			float srcX = src_ptr[ii];
			float srcY = src_ptr[ii + 1];
			float dstX = dst_ptr[ii];
			float dstY = dst_ptr[ii + 1];
			float _x = f_model_array[0] * srcX + f_model_array[1] * srcY
					+ f_model_array[2];
			float _y = f_model_array[3] * srcX + f_model_array[4] * srcY
					+ f_model_array[5];
			float dx = _x - dstX;
			float dy = _y - dstY;
			if (dx * dx + dy * dy < threshold_square) {
				++support;
			}
		}
		if (support > best_support) {
			best_support = support;
			std::copy(f_model_array, f_model_array + 6, best_model_array);
		}
		if (best_support > 12)
			break;
	}
	// check singularity
	if (best_model_array[0] * best_model_array[4]
			- best_model_array[1] * best_model_array[3] < 1e-3)
		return cv::Mat();
	if (best_support > 12) {
		cv::Mat model(2, 3, CV_32FC1, best_model_array);
		return model.clone();
	}
	return cv::Mat();
}
