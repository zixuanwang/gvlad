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

std::string detector_array[] = { "SURF", "SIFT", "DENSE" };
std::string descriptor_array[] = { "SURF", "SIFT" };

std::string image_dir;
std::string descriptor_dir;
std::vector<std::string> file_vector;
std::string detector_type = "SURF";
std::string descriptor_type = "SURF";
float scale = 0.f;

bool quiet_mode = false;

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
		DetectorDescriptor::instance()->compute_descriptor(image,
				geo_descriptor_vector);
		geo_descriptor_vector.save(descriptor_dir + "/" + stem + ".geodesc");
		if (!quiet_mode) {
			std::cout << "computing descriptors for " << file_vector[i] << " "
					<< geo_descriptor_vector.descriptor_count()
					<< " keypoints are detected..." << std::endl;
		}
	}
}

void exit_with_help() {
	std::cout
			<< "Usage: compute_descriptor [options] image_dir descriptor_dir\n"
					"options:\n"
					"-k keypoint type : set type of keypoint detectors (default 0)\n"
					"\t0 -- SURF\t\t\n"
					"\t1 -- SIFT\t\t\n"
					"\t2 -- DENSE\t\t\n"
					"-d descriptor type : set type of descriptors (default 0)\n"
					"\t0 -- SURF\t\t\n"
					"\t1 -- SIFT\t\t\n"
					"-s size : resize the maximum dimension of the image to this size (default 0)\n"
					"\t0 -- no scaling\t\t\n"
					"\t1024 -- recommend for all benchmark datasets (Holidays, Oxford and Paris)\n"
					"-q : quiet mode (no console outputs)\n";
	exit(1);
}

void parse_command_line(int argc, char* argv[]) {
	int i;
	for (i = 1; i < argc; ++i) {
		if (argv[i][0] != '-')
			break;
		if (++i >= argc)
			exit_with_help();
		switch (argv[i - 1][1]) {
		case 'k':
			detector_type = detector_array[atoi(argv[i])];
			break;
		case 'd':
			descriptor_type = descriptor_array[atoi(argv[i])];
			break;
		case 's':
			scale = atof(argv[i]);
			break;
		case 'q':
			quiet_mode = true;
			--i;
			break;
		default:
			std::cerr << "Unknown option: " << argv[i - 1][1] << std::endl;
			exit_with_help();
		}
	}
	if (i >= argc - 1)
		exit_with_help();
	image_dir = argv[i++];
	descriptor_dir = argv[i];
}

int main(int argc, char* argv[]) {
	parse_command_line(argc, argv);
	cv::initModule_nonfree();
	DetectorDescriptor::instance()->init(detector_type, descriptor_type);
	boost::filesystem::create_directories(descriptor_dir);
	File::get_files(file_vector, image_dir, true);
	for (size_t i = 0; i < file_vector.size(); ++i) {
		compute_descriptor(i);
	}
//	if (argc == 1) {
//		std::cout
//				<< "Usage: ./compute_descriptor image_dir descriptor_dir <detector_type = SURF> <descriptor_type = SURF> <scale = 0>"
//				<< std::endl;
//		std::cout << "image_dir is the directory containing all images."
//				<< std::endl;
//		std::cout
//				<< "descriptor_dir is the output directory containing all image descriptors."
//				<< std::endl;
//		std::cout << "detector_type can be SURF, SIFT and DENSE." << std::endl;
//		std::cout << "descriptor_type can be SURF and SIFT." << std::endl;
//		std::cout
//				<< "scale is to resize the maximum dimension of the image to this number."
//				<< std::endl;
//		std::cout
//				<< "For example: ./compute_descriptor ~/images ~/surf SURF SURF 0"
//				<< std::endl;
//		return 0;
//	}
//	if (argc >= 3) {
//		cv::initModule_nonfree();
//		image_dir = argv[1];
//		descriptor_dir = argv[2];
//		if (argc >= 3) {
//			detector_type = argv[3];
//			descriptor_type = argv[4];
//		}
//		if (argc == 6) {
//			scale = atof(argv[5]);
//		}
//		DetectorDescriptor::instance()->init(detector_type, descriptor_type);
//		boost::filesystem::create_directories(descriptor_dir);
//		File::get_files(file_vector, image_dir, true);
//		for (size_t i = 0; i < file_vector.size(); ++i) {
//			compute_descriptor(i);
//		}
//	}
	return 0;
}
