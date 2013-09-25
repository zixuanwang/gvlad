/*
 * compute_gvlad.cpp
 *
 *  Created on: Jun 24, 2013
 *      Author: daniewang
 */

#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include "gvlad/lib/File.h"
#include "gvlad/lib/Serializer.h"
#include "gvlad/lib/Vocabulary.h"

std::string descriptor_dir;
std::string vocabulary;
std::string adaptation;
std::string feature_dir;
std::vector<std::string> file_vector;

bool binary_format = true;
bool quiet_mode = false;

void compute_feature(size_t i) {
	std::string stem = File::get_stem(file_vector[i]);
	GeoDescriptorVector<float> geo_descriptor_vector;
	geo_descriptor_vector.load(file_vector[i]);
	std::vector<float> feature;
	Vocabulary::instance()->compute_gvlad(feature, geo_descriptor_vector);
	if (binary_format) {
		Serializer::save(feature, feature_dir + "/" + stem + ".fea");
	} else {
		Serializer::save_text(feature, feature_dir + "/" + stem + ".txt");
	}
	if (!quiet_mode) {
		std::cout << "computing gvlad for " << file_vector[i] << "..."
				<< std::endl;
	}
}

void exit_with_help() {
	std::cout
			<< "Usage: compute_gvlad [options] descriptor_dir vocabulary adaptation feature_dir\n"
					"options:\n"
					"-f output format : set type of output features (default 0)\n"
					"\t0 -- binary\t\t(binary format, compact to store)\n"
					"\t1 -- text\t\t(text format, easy to read and debug)\n"
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
		case 'f':
			binary_format = atoi(argv[i]) == 0;
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
	if (i >= argc - 3)
		exit_with_help();
	descriptor_dir = argv[i++];
	vocabulary = argv[i++];
	adaptation = argv[i++];
	feature_dir = argv[i];
}

int main(int argc, char* argv[]) {
	parse_command_line(argc, argv);
	boost::filesystem::create_directories(feature_dir);
	Vocabulary::instance()->load(vocabulary);
	Vocabulary::instance()->load_adaptation(adaptation);
	File::get_files(file_vector, descriptor_dir);
	for (size_t i = 0; i < file_vector.size(); ++i) {
		compute_feature(i);
	}
	return 0;
}

