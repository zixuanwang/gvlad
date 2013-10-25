/*
 * compute_adaptation.cpp
 *
 *  Created on: Jun 24, 2013
 *      Author: daniewang
 */

#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/thread/mutex.hpp>
#include "gvlad/lib/File.h"
#include "gvlad/lib/GlobalConfig.h"
#include "gvlad/lib/Vocabulary.h"

int word_count;
int dimension;
cv::Mat adaptation_mat;
std::vector<std::string> file_vector;
std::vector<int64_t> adaptation_count;
boost::mutex mutex;

std::string vocabulary_path;
std::string descriptor_dir;
std::string adaptation_path;


void compute_adaptation(size_t i) {
	GeoDescriptorVector<float> geo_descriptor_vector;
	geo_descriptor_vector.load(file_vector[i]);
	int count = geo_descriptor_vector.descriptor_count();
	for (int j = 0; j < count; ++j) {
		int angle_index = Angle::get_index(
			geo_descriptor_vector.geo_descriptor_vector[j].angle);
		int word_index = Vocabulary::instance()->quantize(
			geo_descriptor_vector.geo_descriptor_vector[j].descriptor.component);
		boost::mutex::scoped_lock(mutex);
		double* ptr = adaptation_mat.ptr<double>(word_index)
				+ angle_index * dimension;
		for (int k = 0; k < dimension; ++k) {
			ptr[k] +=
					(double) geo_descriptor_vector.geo_descriptor_vector[j].descriptor.component[k];
		}
		adaptation_count[angle_index * word_count + word_index] += 1;
	}
}

void exit_with_help() {
	std::cout
		<< "Usage: compute_adaptation [options] vocabulary_path descriptor_dir adaptation_path\n"
		"options:\n"
		"-n -- number of angle bins (default 4)\n"
		"-o -- angle offset (default 45)\n";
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
		case 'n':
			GlobalConfig::angle_bins = atoi(argv[i]);
			break;
		case 'o':
			GlobalConfig::angle_offset = atof(argv[i]);
			break;
		default:
			std::cerr << "Unknown option: " << argv[i - 1][1] << std::endl;
			exit_with_help();
		}
	}
	if (i >= argc - 2)
		exit_with_help();
	vocabulary_path = argv[i++];
	descriptor_dir = argv[i++];
	adaptation_path = argv[i];
}

// compute the adaptation centers.
int main(int argc, char* argv[]) {
	parse_command_line(argc, argv);
	int angle_bins = GlobalConfig::angle_bins;
	float offset = GlobalConfig::angle_offset;
	boost::filesystem::create_directories(
		File::get_parent_path(adaptation_path));
	Vocabulary::instance()->load(vocabulary_path);
	word_count = Vocabulary::instance()->rows();
	dimension = Vocabulary::instance()->cols();
	adaptation_mat = cv::Mat(word_count, dimension * angle_bins, CV_64FC1,
		cv::Scalar(0));
	adaptation_count.assign(word_count * angle_bins, 0);
	File::get_files(file_vector, descriptor_dir);
	for (size_t i = 0; i < file_vector.size(); ++i) {
		compute_adaptation(i);
	}
	cv::Mat adaptation_mat_float(word_count, dimension * angle_bins,
		CV_32FC1, cv::Scalar(0));
	for (int i = 0; i < word_count; ++i) {
		for (int j = 0; j < angle_bins; ++j) {
			for (int k = 0; k < dimension; ++k) {
				adaptation_mat_float.at<float>(i, j * dimension + k) =
					adaptation_mat.at<double>(i, j * dimension + k)
					/ (double) adaptation_count[j * word_count
					+ i];
			}
		}
	}
	cv::FileStorage f(adaptation_path, cv::FileStorage::WRITE);
	f << "mat" << adaptation_mat_float;
	f.release();
	return 0;
}

