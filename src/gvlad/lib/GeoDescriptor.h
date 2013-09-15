/*
 * GeoDescriptor.h
 *
 *  Created on: May 23, 2013
 *      Author: daniewang
 */

#ifndef GEODESCRIPTOR_H_
#define GEODESCRIPTOR_H_

#include "Descriptor.h"
#include <opencv2/opencv.hpp>

// The format conforms with the holiday dataset.
// http://lear.inrialpes.fr/~jegou/data.php
template<typename T>
class GeoDescriptor {
public:
	GeoDescriptor() :
			x(0.f), y(0.f), scale(0.f), angle(0.f), mi11(0.f), mi12(0.f), mi21(
					0.f), mi22(0.f), cornerness(0.f) {
	}
	~GeoDescriptor() {
	}
	void load(std::istream& in_stream) {
		in_stream.read((char*) &x, sizeof(x));
		in_stream.read((char*) &y, sizeof(y));
		in_stream.read((char*) &scale, sizeof(scale));
		in_stream.read((char*) &angle, sizeof(angle));
		in_stream.read((char*) &mi11, sizeof(mi11));
		in_stream.read((char*) &mi12, sizeof(mi12));
		in_stream.read((char*) &mi21, sizeof(mi21));
		in_stream.read((char*) &mi22, sizeof(mi22));
		in_stream.read((char*) &cornerness, sizeof(cornerness));
		descriptor.load(in_stream);
	}
	void save(std::ostream& out_stream) const {
		out_stream.write((char*) &x, sizeof(x));
		out_stream.write((char*) &y, sizeof(y));
		out_stream.write((char*) &scale, sizeof(scale));
		out_stream.write((char*) &angle, sizeof(angle));
		out_stream.write((char*) &mi11, sizeof(mi11));
		out_stream.write((char*) &mi12, sizeof(mi12));
		out_stream.write((char*) &mi21, sizeof(mi21));
		out_stream.write((char*) &mi22, sizeof(mi22));
		out_stream.write((char*) &cornerness, sizeof(cornerness));
		descriptor.save(out_stream);
	}
	int byte_size() const {
		return sizeof(x) + sizeof(y) + sizeof(scale) + sizeof(angle)
				+ sizeof(mi11) + sizeof(mi12) + sizeof(mi21) + sizeof(mi22)
				+ sizeof(cornerness) + sizeof(int) + descriptor.byte_size();
	}
	// return the dimension of the descriptor.
	int dimension() const {
		return descriptor.dimension();
	}
	template<typename U> void copy_from(
			const GeoDescriptor<U>& geo_descriptor) {
		x = geo_descriptor.x;
		y = geo_descriptor.y;
		scale = geo_descriptor.scale;
		angle = geo_descriptor.angle;
		mi11 = geo_descriptor.mi11;
		mi12 = geo_descriptor.mi12;
		mi21 = geo_descriptor.mi21;
		mi22 = geo_descriptor.mi22;
		cornerness = geo_descriptor.cornerness;
		descriptor.copy_from(geo_descriptor.descriptor);
	}
	// draw key point on the image.
	void draw(cv::Mat& image) {
		// draw center.
		cv::circle(image, cv::Point(x, y), 1, cv::Scalar(70, 180, 70), 1,
				CV_AA);
		// draw angle.
		cv::line(image, cv::Point(x, y),
				cv::Point(x + cosf(angle) * scale, y + sinf(angle) * scale),
				cv::Scalar(70, 180, 70), 1, CV_AA);
		// draw circle.
		cv::circle(image, cv::Point(x, y), (int) scale, cv::Scalar(50, 70, 210),
				1, CV_AA);
	}
	float x;
	float y;
	float scale;
	float angle;
	float mi11;
	float mi12;
	float mi21;
	float mi22;
	float cornerness;
	Descriptor<T> descriptor;
};

// print the descriptor for debug.
template<typename T>
std::ostream& operator<<(std::ostream& os,
		const GeoDescriptor<T>& geo_descriptor) {
	os << "x: " << geo_descriptor.x << "\t";
	os << "y: " << geo_descriptor.y << "\t";
	os << "scale: " << geo_descriptor.scale << "\t";
	os << "angle: " << geo_descriptor.angle << "\t";
	os << "mi11: " << geo_descriptor.mi11 << "\t";
	os << "mi12: " << geo_descriptor.mi12 << "\t";
	os << "mi21: " << geo_descriptor.mi21 << "\t";
	os << "mi22: " << geo_descriptor.mi22 << "\t";
	os << "cornerness:" << geo_descriptor.cornerness << "\t";
	os << geo_descriptor.descriptor << "\t";
	os << std::endl;
	return os;
}

#endif /* GEODESCRIPTOR_H_ */
