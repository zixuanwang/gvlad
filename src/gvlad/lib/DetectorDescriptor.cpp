/*
 * DetectorDescriptor.cpp
 *
 *  Created on: Jul 15, 2013
 *      Author: zxwang
 */

#include "DetectorDescriptor.h"

DetectorDescriptor* DetectorDescriptor::p_instance = NULL;

DetectorDescriptor::DetectorDescriptor() {

}

DetectorDescriptor* DetectorDescriptor::instance() {
	if (p_instance == NULL) {
		p_instance = new DetectorDescriptor;
	}
	return p_instance;
}

void DetectorDescriptor::init(const std::string& detector_type,
		const std::string& descriptor_type) {
	m_detector_type = detector_type;
	m_descriptor_type = descriptor_type;
	if (detector_type == "DENSE") {
		m_detector = cv::Ptr<cv::FeatureDetector>(
				new cv::DenseFeatureDetector(16.f, 1, 0.1f, 6, 0, true, false));
	}
	if (detector_type == "SIFT" || detector_type == "SURF") {
		m_detector = cv::FeatureDetector::create(detector_type);
	}
	m_descriptor = cv::DescriptorExtractor::create(descriptor_type);
}

void DetectorDescriptor::compute_descriptor(const cv::Mat& image,
		GeoDescriptorVector<float>& geo_descriptor_vector) {
	try {
		if (image.empty()) {
			return;
		}
		cv::Mat gray = image;
		if (image.type() == CV_8UC3)
			cv::cvtColor(image, gray, CV_BGR2GRAY);
		std::vector<cv::KeyPoint> keypoints;
		m_detector->detect(gray, keypoints, cv::Mat());
		if (!keypoints.empty()) {
			cv::Mat descriptors;
			m_descriptor->compute(gray, keypoints, descriptors);
			geo_descriptor_vector.width = gray.cols;
			geo_descriptor_vector.height = gray.rows;
			geo_descriptor_vector.convert_from(keypoints, descriptors);
		}
	} catch (const cv::Exception& e) {
		std::cerr << e.what() << std::endl;
	}
}

void DetectorDescriptor::draw(cv::Mat& image,
		GeoDescriptorVector<float>& geo_descriptor_vector, bool show_angle) {
	int count = geo_descriptor_vector.descriptor_count();
	cv::Scalar color_array[4] = { cv::Scalar(220, 50, 20), cv::Scalar(50, 220,
			20), cv::Scalar(20, 50, 220), cv::Scalar(220, 50, 220) };
	for (int i = 0; i < count; ++i) {
		GeoDescriptor<float>& descriptor =
				geo_descriptor_vector.geo_descriptor_vector[i];
		int color_index = 0;
		if (show_angle) {
			color_index = Angle::get_index(descriptor.angle);
		}
		cv::line(image, cv::Point((int)descriptor.x, (int)descriptor.y),
				cv::Point((int)
						(descriptor.x
								+ cosf(descriptor.angle * 3.14159f / 180.f)
										* descriptor.scale),
						(int)(descriptor.y
								+ sinf(descriptor.angle * 3.14159f / 180.f)
										* descriptor.scale)),
				color_array[color_index], 2, CV_AA);
	}
}

void DetectorDescriptor::get_angle(const cv::Mat& image,
		std::vector<cv::KeyPoint>& keypoints) {
	// compute gradient.
	cv::Mat dx_mat(image.rows, image.cols, CV_32FC1, cv::Scalar(0));
	cv::Mat dy_mat(image.rows, image.cols, CV_32FC1, cv::Scalar(0));
	for (int row = 0; row < image.rows - 1; ++row) {
		const uchar* row_ptr = image.ptr<uchar>(row);
		const uchar* row_ptr_next = image.ptr<uchar>(row + 1);
		float* dx_ptr = dx_mat.ptr<float>(row);
		float* dy_ptr = dy_mat.ptr<float>(row);
		for (int col = 0; col < image.cols - 1; ++col) {
			dx_ptr[col] = static_cast<float>(row_ptr[col + 1])
					- static_cast<float>(row_ptr[col]);
			dy_ptr[col] = static_cast<float>(row_ptr_next[col])
					- static_cast<float>(row_ptr[col]);
		}
	}
	for (size_t i = 0; i < keypoints.size(); ++i) {
		cv::Rect rect(keypoints[i].pt.x - keypoints[i].size / 2,
				keypoints[i].pt.y - keypoints[i].size / 2, keypoints[i].size,
				keypoints[i].size);
		rect &= cv::Rect(0, 0, image.cols, image.rows);
		cv::Mat patch = dx_mat(rect);
		float dx = static_cast<float>(cv::sum(patch).val[0]);
		patch = dy_mat(rect);
		float dy = static_cast<float>(cv::sum(patch).val[0]);
		float ang = 0.f;
		if (dx != 0.f || dy != 0.f)
			ang = atan2f(dy, dx);
		ang += 3.1415926f;
		ang *= 180.f / 3.1415926f;
		keypoints[i].angle = ang;
	}
}
