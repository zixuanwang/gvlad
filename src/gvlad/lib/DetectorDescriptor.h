/*
 * DetectorDescriptor.h
 *
 *  Created on: Jul 15, 2013
 *      Author: zxwang
 */

#ifndef DETECTORDESCRIPTOR_H_
#define DETECTORDESCRIPTOR_H_

#include "Angle.h"
#include "GeoDescriptorVector.h"
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/opencv.hpp>

class DetectorDescriptor {
public:
	static DetectorDescriptor* instance();
	// initialize the detector and descriptor pointers.
	// this method is slow for BRISK. only called once.
	void init(const std::string& detector_type,
			const std::string& descriptor_type);
	void compute_descriptor(const cv::Mat& image,
			GeoDescriptorVector<float>& geo_descriptor_vector);
	void draw(cv::Mat& image, GeoDescriptorVector<float>& geo_descriptor_vector,
			bool show_angle = false);
	void get_angle(const cv::Mat& image, std::vector<cv::KeyPoint>& keypoints);
private:
	DetectorDescriptor();
	DetectorDescriptor(const DetectorDescriptor&);
	DetectorDescriptor& operator=(const DetectorDescriptor&);
	static DetectorDescriptor* p_instance;
	cv::Ptr<cv::FeatureDetector> m_detector;
	cv::Ptr<cv::DescriptorExtractor> m_descriptor;
	std::string m_detector_type;
	std::string m_descriptor_type;
};

#endif /* DETECTORDESCRIPTOR_H_ */
