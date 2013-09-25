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

int angle_bins;
float offset;
int word_count;
int dimension;
cv::Mat adaptation_mat;
std::vector<std::string> file_vector;
std::vector<int64_t> adaptation_count;
boost::mutex mutex;

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

// compute the adaptation centers.
int main(int argc, char* argv[]) {
	if (argc == 1) {
		std::cout
				<< "Usage: compute_adaptation vocabulary_path descriptor_dir adaptation_path"
				<< std::endl;
		std::cout << "vocabulary_path : the xml file generated from compute_vocabulary." << std::endl;
		std::cout << "descriptor_dir : the directory contains image descriptors." << std::endl;
		std::cout << "adaptation_path : the path of the output xml file." << std::endl;
		std::cout << "For example: ./compute_adaptation ~/vocabulary.xml ~/surf ~/adaptation.xml" << std::endl;
		return 0;
	}
	if (argc == 4) {
		std::string vocabulary_path = argv[1];
		std::string descriptor_dir = argv[2];
		std::string adaptation_path = argv[3];
		angle_bins = GlobalConfig::angle_bins;
		offset = GlobalConfig::angle_offset;
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
	}
	return 0;
}

