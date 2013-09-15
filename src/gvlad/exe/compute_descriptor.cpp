/*
 * compute_descriptor.cpp
 *
 *  Created on: May 25, 2013
 *      Author: daniewang
 */

#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include "gvlad/lib/DetectorDescriptor.h"
#include "gvlad/lib/File.h"
#include "gvlad/lib/Timer.h"
#include <opencv2/nonfree/nonfree.hpp>

std::string image_dir;
std::string descriptor_dir;
std::vector<std::string> file_vector;
std::string detector_type = "SURF";
std::string descriptor_type = "SURF";
float scale = 0.f;

void compute_descriptor(size_t i) {
	std::string stem = File::get_stem(file_vector[i]);
	cv::Mat img = cv::imread(file_vector[i], 0);
	if (!img.empty()) {
		cv::Mat image = img;
		if (scale != 0.f) {
			double s = (double) scale / std::max(img.rows, img.cols);
			cv::resize(img, image, cv::Size(), s, s);
		}
		GeoDescriptorVector<float> geo_descriptor_vector;
		Timer timer;
		timer.start();
		DetectorDescriptor::instance()->compute_descriptor(image,
				geo_descriptor_vector);
		std::cout << "computing descriptors for " << file_vector[i] << " "
				<< geo_descriptor_vector.descriptor_count()
				<< " keypoints are detected... (" << timer.stop() << " ms)"
				<< std::endl;
		geo_descriptor_vector.save(descriptor_dir + "/" + stem + ".geodesc");
	}
}

int main(int argc, char* argv[]) {
	if (argc == 1) {
		std::cout
				<< "Usage: ./compute_descriptor image_dir descriptor_dir <detector_type = SURF> <descriptor_type = SURF> <scale = 0>"
				<< std::endl;
		std::cout << "image_dir is the directory containing all images." << std::endl;
		std::cout << "descriptor_dir is the output directory containing all image descriptors." << std::endl;
		std::cout << "detector_type can be SURF, SIFT and DENSE." << std::endl;
		std::cout << "descriptor_type can be SURF and SIFT." << std::endl;
		std::cout << "scale is to resize the maximum dimension of the image to this number." << std::endl;
		std::cout << "For example: ./compute_descriptor ~/images ~/surf SURF SURF 0" << std::endl;
		return 0;
	}
	if (argc >= 3) {
		cv::initModule_nonfree();
		image_dir = argv[1];
		descriptor_dir = argv[2];
		if (argc >= 3) {
			detector_type = argv[3];
			descriptor_type = argv[4];
		}
		if (argc == 6) {
			scale = atof(argv[5]);
		}
		DetectorDescriptor::instance()->init(detector_type, descriptor_type);
		boost::filesystem::create_directories(descriptor_dir);
		File::get_files(file_vector, image_dir, true);
		for (size_t i = 0; i < file_vector.size(); ++i) {
			compute_descriptor(i);
		}
	}
	return 0;
}
