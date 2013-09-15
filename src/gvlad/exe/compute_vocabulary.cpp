/*
 * compute_vocabulary.cpp
 *
 *  Created on: May 25, 2013
 *      Author: daniewang
 */
#include "gvlad/lib/File.h"
#include "gvlad/lib/Vocabulary.h"

// compute the vocabulary based on all descriptors in one folder.
int main(int argc, char* argv[]) {
	if (argc == 1) {
		std::cout
				<< "Usage: ./compute_vocabulary descriptor_dir k vocabulary_path"
				<< std::endl;
		std::cout << "descriptor_dir is the directory contains image descriptors." << std::endl;
		std::cout << "k is the number of desired visual words." << std::endl;
		std::cout << "vocabulary_path is the output path of the vocabulary." << std::endl;
		std::cout << "For example: ./compute_vocabulary ~/surf 256 ~/vocabulary.xml" << std::endl;
		return 0;
	}
	if (argc == 4) {
		std::string descriptor_dir = argv[1];
		int k = atoi(argv[2]);
		std::string vocabulary_path = argv[3];
		boost::filesystem::create_directories(
				File::get_parent_path(vocabulary_path));
		std::vector<std::string> file_vector;
		File::get_files(file_vector, descriptor_dir);
		std::vector<float> sample_vector;
		int sample_count = 0;
		for (size_t i = 0; i < file_vector.size(); ++i) {
			GeoDescriptorVector<float> geo_descriptor_vector;
			geo_descriptor_vector.load(file_vector[i]);
			int count = geo_descriptor_vector.descriptor_count();
			for (int j = 0; j < count; ++j) {
				GeoDescriptor<float>& geo_descriptor =
						geo_descriptor_vector.geo_descriptor_vector[j];
				for (size_t k = 0;
						k < geo_descriptor.descriptor.component.size(); ++k) {
					sample_vector.push_back(
							(float) geo_descriptor.descriptor.component[k]);
				}
			}
			sample_count += count;
		}
		int dim = sample_vector.size() / sample_count;
		cv::Mat sample_mat(sample_count, dim, CV_32FC1);
		std::copy(sample_vector.begin(), sample_vector.end(),
				sample_mat.ptr<float>(0));
		std::vector<float>().swap(sample_vector);
		std::cout << sample_count << " samples. run clustering..." << std::endl;
		Vocabulary::instance()->build(sample_mat, k);
		Vocabulary::instance()->save(vocabulary_path);
	}
	return 0;
}

