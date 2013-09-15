/*
 * GeometricVerifier.h
 *
 *  Created on: Aug 14, 2013
 *      Author: daniewang
 */

#ifndef GEOMETRICVERIFIER_H_
#define GEOMETRICVERIFIER_H_

#include <boost/shared_ptr.hpp>
#include "GeoDescriptorVector.h"
#include <opencv2/opencv.hpp>

class GeometricVerifier {
public:
	GeometricVerifier();
	~GeometricVerifier();
	// prepare the ann index for the query descriptor.
	void prepare(const GeoDescriptorVector<float>& query_geo);
	bool verify(const GeoDescriptorVector<float>& candidate_geo) const;
	// return 2 * 3 affine matrix using ransac.
	// if ransac fails, the empty matrix is returned.
	cv::Mat get_affine(const cv::Mat& src_points, const cv::Mat& dst_points, int k = 500) const;
private:
	cv::Mat m_query_descriptor_mat;
	boost::shared_ptr<cv::flann::Index> m_index;
	GeoDescriptorVector<float> m_query_geo;
};

#endif /* GEOMETRICVERIFIER_H_ */
