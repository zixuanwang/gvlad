/*
 * GeoDescriptorVector.h
 *
 *  Created on: May 23, 2013
 *      Author: daniewang
 */

#ifndef GEODESCRIPTORVECTOR_H_
#define GEODESCRIPTORVECTOR_H_

#include <boost/algorithm/string.hpp>
#include "GeoDescriptor.h"
#include "Math.h"
#include <opencv2/opencv.hpp>

template<typename T>
class GeoDescriptorVector {
public:
	GeoDescriptorVector() :
			width(0), height(0) {
	}
	~GeoDescriptorVector() {
	}
	void load(const std::string& file) {
		geo_descriptor_vector.clear();
		std::ifstream in_stream;
		in_stream.open(file.c_str(), std::ios::binary);
		if (in_stream.good()) {
			in_stream.read((char*) &width, sizeof(width));
			in_stream.read((char*) &height, sizeof(height));
			std::streampos start = in_stream.tellg();
			in_stream.seekg(0, in_stream.end);
			int length = in_stream.tellg() - start;
			in_stream.seekg(start, in_stream.beg);
			if (length > 0) {
				GeoDescriptor<T> descriptor;
				descriptor.load(in_stream);
				int descriptor_count = length / descriptor.byte_size();
				geo_descriptor_vector.reserve(descriptor_count);
				geo_descriptor_vector.push_back(descriptor);
				for (int i = 1; i < descriptor_count; ++i) {
					descriptor.load(in_stream);
					geo_descriptor_vector.push_back(descriptor);
				}
			}
			in_stream.close();
		}
	}
	void save(const std::string& file) const {
		std::ofstream out_stream;
		out_stream.open(file.c_str(), std::ios::binary);
		if (out_stream.good()) {
			out_stream.write((char*) &width, sizeof(width));
			out_stream.write((char*) &height, sizeof(height));
			for (size_t i = 0; i < geo_descriptor_vector.size(); ++i) {
				geo_descriptor_vector[i].save(out_stream);
			}
			out_stream.close();
		}
	}
	// load the hessian-affine detector and sift descriptor from a file
	// the format is http://cmp.felk.cvut.cz/~perdom1/hesaff/
	void load_hesaff(const std::string& file) {
		geo_descriptor_vector.clear();
		std::ifstream in_stream;
		in_stream.open(file.c_str());
		if (in_stream.good()) {
			std::string line;
			getline(in_stream, line);
			getline(in_stream, line);
			int descriptor_count = atoi(line.c_str());
			for (int i = 0; i < descriptor_count; ++i) {
				getline(in_stream, line);
				std::vector<std::string> token_vector;
				boost::split(token_vector, line, boost::is_any_of(" "));
				GeoDescriptor<T> geo_descriptor;
				geo_descriptor.x = atof(token_vector[0].c_str());
				geo_descriptor.y = atof(token_vector[1].c_str());
				float a = atof(token_vector[2].c_str());
				float b = atof(token_vector[3].c_str());
				float c = atof(token_vector[4].c_str());
				float angle = 0.f;
				// using the math from http://mathworld.wolfram.com/Ellipse.html
				// angle is [0, pi)
				if (b == 0.f && a < c)
					angle = 0.f;
				if (b == 0.f && a > c)
					angle = Math::PI / 2.f;
				if (b != 0.f && a < c)
					angle = atanf(2.f * b / (a - c)) / 2.f;
				if (b != 0.f && a > c)
					angle = atanf(2.f * b / (a - c)) / 2.f + Math::PI / 2.f;
				geo_descriptor.angle = angle;
				geo_descriptor.scale = Math::PI / sqrtf(a * c - b * b);
				for (size_t j = 5; j < token_vector.size(); ++j) {
					geo_descriptor.descriptor.component.push_back(
							atof(token_vector[j].c_str()));
				}
				// root normalization.
				Math::l1_normalize(geo_descriptor.descriptor.component);
				Math::square_root(geo_descriptor.descriptor.component);
				Math::l2_normalize(geo_descriptor.descriptor.component);
				geo_descriptor_vector.push_back(geo_descriptor);
			}
			in_stream.close();
		}
	}

	void convert(cv::Mat& descriptor_mat) const {
		if (geo_descriptor_vector.empty()) {
			descriptor_mat = cv::Mat();
		} else {
			int rows = descriptor_count();
			int cols = geo_descriptor_vector[0].dimension();
			descriptor_mat = cv::Mat(rows, cols, CV_32FC1, cv::Scalar(0));
			// fill the matrix
			for (int i = 0; i < rows; ++i) {
				float* ptr = descriptor_mat.ptr<float>(i);
				for (int j = 0; j < cols; ++j) {
					ptr[j] =
							(float) geo_descriptor_vector[i].descriptor.component[j];
				}
			}
		}
	}
	int descriptor_count() const {
		return (int) geo_descriptor_vector.size();
	}
	template<typename U> void copy_from(const GeoDescriptorVector<U>& rhs) {
		width = rhs.width;
		height = rhs.height;
		geo_descriptor_vector.clear();
		geo_descriptor_vector.reserve(rhs.descriptor_count());
		for (int i = 0; i < rhs.descriptor_count(); ++i) {
			GeoDescriptor<T> geo_descriptor;
			geo_descriptor.copy_from(rhs.geo_descriptor_vector[i]);
			geo_descriptor_vector.push_back(geo_descriptor);
		}
	}
	// convert from opencv's output. descriptors must be of type CV_32FC1.
	void convert_from(const std::vector<cv::KeyPoint>& keypoints,
			const cv::Mat& descriptors) {
		geo_descriptor_vector.clear();
		geo_descriptor_vector.reserve(keypoints.size());
		for (size_t i = 0; i < keypoints.size(); ++i) {
			GeoDescriptor<T> geo_descriptor;
			geo_descriptor.x = keypoints[i].pt.x;
			geo_descriptor.y = keypoints[i].pt.y;
			geo_descriptor.scale = keypoints[i].size;
			geo_descriptor.angle = keypoints[i].angle;
			if (descriptors.type() == CV_32FC1)
				geo_descriptor.descriptor.component.assign(
						descriptors.ptr<float>(i),
						descriptors.ptr<float>(i) + descriptors.cols);
			if (descriptors.type() == CV_8UC1)
				geo_descriptor.descriptor.component.assign(
						descriptors.ptr<uchar>(i),
						descriptors.ptr<uchar>(i) + descriptors.cols);
			geo_descriptor_vector.push_back(geo_descriptor);
		}
	}
	// split the geo_descriptor_vector by xy coordinates.
	// by default, split to 2 * 2 grids
	void split_space(std::vector<GeoDescriptorVector<T> >& vector,
			int row_bins = 2, int col_bins = 2) const {
		float width_step = (float) width / (float) col_bins;
		float height_step = (float) height / (float) row_bins;
		int bins = row_bins * col_bins;
		GeoDescriptorVector<T> geo;
		vector.assign(bins, geo);
		for (size_t i = 0; i < geo_descriptor_vector.size(); ++i) {
			int row_index = (int) (geo_descriptor_vector[i].y / height_step);
			int col_index = (int) (geo_descriptor_vector[i].x / width_step);
			if (row_index >= 0 && row_index < row_bins && col_index >= 0
					&& col_index < col_bins) {
				vector[row_index * col_bins + col_index].geo_descriptor_vector.push_back(
						geo_descriptor_vector[i]);
			}
		}
	}

	// split the geo_descriptor_vector by angles of key points.
	// the possible values of angles are in a range [0, 360).
	void split_angle(std::vector<GeoDescriptorVector<T> >& vector, int bins = 2,
			float offset = 0.f) const {
		float step = 360.f / (float) bins;
//		float step = Math::PI / (float) bins;
		GeoDescriptorVector<T> geo;
		vector.assign(bins, geo);
		for (size_t i = 0; i < geo_descriptor_vector.size(); ++i) {
			float angle = geo_descriptor_vector[i].angle - offset;
			if (angle < 0.f)
				angle += 360.f;
			int index = (int) (angle / step);
			if (index >= 0 && index < bins) {
				vector[index].geo_descriptor_vector.push_back(
						geo_descriptor_vector[i]);
			}
		}
	}
	// split the geo_descriptor_vector by angles of key points.
	// the possible values of scales are in a range [0, 100).
	void split_scale(std::vector<GeoDescriptorVector<T> >& vector,
			int bins = 2) const {
		float step = 100.f / (float) bins;
		GeoDescriptorVector<T> geo;
		vector.assign(bins, geo);
		for (size_t i = 0; i < geo_descriptor_vector.size(); ++i) {
			int index = (int) (geo_descriptor_vector[i].scale / step);
			if (index >= 0 && index < bins) {
				vector[index].geo_descriptor_vector.push_back(
						geo_descriptor_vector[i]);
			}
		}
	}

	// draw key points on the image.
	void draw(cv::Mat& image) {
		for (size_t i = 0; i < geo_descriptor_vector.size(); ++i) {
			geo_descriptor_vector[i].draw(image);
		}
	}
	// image width.
	int width;
	// image height.
	int height;
	// store all descriptors from one image in the vector.
	std::vector<GeoDescriptor<T> > geo_descriptor_vector;
};

#endif /* GEODESCRIPTORVECTOR_H_ */
