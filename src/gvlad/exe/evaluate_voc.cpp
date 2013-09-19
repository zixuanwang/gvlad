/*
 * evaluate_voc.cpp
 *
 *  Created on: Sep 16, 2013
 *      Author: daniewang
 */

#include <boost/algorithm/string.hpp>
#include <boost/unordered_map.hpp>
#include "gvlad/lib/File.h"
#include "gvlad/lib/Serializer.h"

std::string train_dir;
std::string feature_dir;
std::string class_array[] = { "aeroplane", "bicycle", "bird", "boat", "bottle",
		"bus", "car", "cat", "chair", "cow", "diningtable", "dog", "horse",
		"motorbike", "person", "pottedplant", "sheep", "sofa", "train",
		"tvmonitor" };

void compute_training(const std::string& train_dir,
		const std::string& feature_dir, const std::string& output) {
	boost::unordered_map<std::string, int> label_map;
	for (int i = 0; i < 20; ++i) {
		std::string train_filepath = train_dir + "/" + class_array[i]
				+ "_train.txt";
		std::ifstream in_stream;
		in_stream.open(train_filepath.c_str());
		if (in_stream.good()) {
			std::string line;
			while (getline(in_stream, line)) {
				if (!line.empty()) {
					std::stringstream ss;
					ss << line;
					std::string stem;
					int label;
					ss >> stem >> label;
					if (label == 1) {
						label_map[stem] = i;
					}
				}
			}
			in_stream.close();
		}
	}
	std::ofstream out_stream;
	out_stream.open(output.c_str());
	if (out_stream.good()) {
		for (boost::unordered_map<std::string, int>::iterator iter =
				label_map.begin(); iter != label_map.end(); ++iter) {
			std::vector<float> feature;
			Serializer::load(feature, feature_dir + "/" + iter->first + ".feature");
			out_stream << iter->second << " ";
			for (size_t i = 0; i < feature.size() - 1; ++i) {
				out_stream << i + 1 << ":" << feature[i] << " ";
			}
			out_stream << feature.size() << ":" << feature.back() << "\n";
		}
		out_stream.close();
	}
}

void compute_testing(const std::string& val_dir, const std::string& output) {
	std::vector<std::string> file_vector;
	File::get_files(file_vector, val_dir);
	if(!file_vector.empty()){
		std::ifstream in_stream;
		in_stream.open(file_vector[0].c_str());
		std::ofstream out_stream;
		out_stream.open(output.c_str());
		if (in_stream.good() && out_stream.good()) {
			std::string line;
			while (getline(in_stream, line)) {
				boost::trim(line);
				if (!line.empty()) {
					std::stringstream ss;
					ss << line;
					std::string stem;
					ss >> stem;
					std::vector<float> feature;
					Serializer::load(feature, feature_dir + "/" + stem + ".feature");
					out_stream << 0 << " ";
					for (size_t i = 0; i < feature.size() - 1; ++i) {
						out_stream << i + 1 << ":" << feature[i] << " ";
					}
					out_stream << feature.size() << ":" << feature.back() << "\n";
				}
			}
			in_stream.close();
			out_stream.close();
		}	
	}
}

void compute_confidence(const std::string& val_dir,
		const std::string& output_path, const std::string& output_dir) {
	std::vector<std::string> file_vector;
	File::get_files(file_vector, val_dir);
	std::vector<std::string> val_vector;
	if(!file_vector.empty()){
		std::ifstream val_stream;
		val_stream.open(file_vector[0].c_str());
		if(val_stream.good()){
			std::string line;
			while (getline(val_stream, line)) {
				boost::trim(line);
				if (!line.empty()) {
					std::stringstream ss;
					ss << line;
					std::string stem;
					ss >> stem;
					val_vector.push_back(stem);
				}
			}
			val_stream.close();
		}
	}
	boost::unordered_map<std::string, std::vector<float> > confidence_map;
	std::ifstream output_stream;
	output_stream.open(output_path.c_str());
	if(output_stream.good()){
		std::string line;
		getline(output_stream, line);
		boost::trim(line);
		std::vector<std::string> token_vector;
		boost::split(token_vector, line, boost::is_any_of(" "));
		if(token_vector.size() == 1){
			// no probability output
			do 
			{
				boost::trim(line);
				std::stringstream ss;
				ss << line;
				int class_id;
				ss >> class_id;
				std::string class_name = class_array[class_id];
				for(int j = 0; j < 20; ++j){
					std::string class_name = class_array[j];
					if(j == class_id){
						confidence_map[class_name].push_back(1.f);
					}else{
						confidence_map[class_name].push_back(-1.f);
					}
				}
			} while (getline(output_stream, line));
		}
		if(token_vector.size() > 1){
			// probability output
			std::vector<int> class_order_vector;
			for (size_t i = 1; i < token_vector.size(); ++i) {
				class_order_vector.push_back(atoi(token_vector[i].c_str()));
			}
			while (getline(output_stream, line)) {
				boost::trim(line);
				if (!line.empty()) {
					std::vector<std::string> conf_vector;
					boost::split(conf_vector, line, boost::is_any_of(" "));
					for (int j = 0; j < 20; ++j) {
						std::string class_name = class_array[class_order_vector[j]];
						confidence_map[class_name].push_back(
							atof(conf_vector[j + 1].c_str()));
					}
				}
			}
		}
		output_stream.close();
	}
	for (boost::unordered_map<std::string, std::vector<float> >::iterator iter =
			confidence_map.begin(); iter != confidence_map.end(); ++iter) {
		std::string output = output_dir + "/comp1_cls_val_" + iter->first
				+ ".txt";
		std::ofstream out_stream(output.c_str());
		for (size_t j = 0; j < iter->second.size(); ++j) {
			out_stream << val_vector[j] << " " << iter->second[j] << std::endl;
		}
		out_stream.close();
	}
}

int main(int argc, char* argv[]) {
	if (argc == 1) {
		std::cout
				<< "Usage: ./evaluate_voc train_dir val_dir feature_dir train.txt test.txt"
				<< std::endl;
		std::cout << "It generates train.txt and test.txt for libsvm." << std::endl;
		std::cout << "For example, ./evaluate_voc ~/train ~/val ~/feature ~/train.txt ~/test.txt" << std::endl;
		std::cout << "Usage: ./evaluate_voc val_dir output.txt output_dir"
				<< std::endl;
		std::cout << "It uses the output of libsvm and generates evaluation files for matlab" << std::endl;
		std::cout << "For example, ./evaluate ~/val_dir ~/output.txt ~/VOCdevkit/results/Main" << std::endl;
		return 0;
	}
	if (argc == 4) {
		compute_confidence(argv[1], argv[2], argv[3]);
	}
	if (argc == 6) {
		train_dir = argv[1];
		feature_dir = argv[3];
		// generate train.txt for libsvm.
		compute_training(train_dir, feature_dir, argv[4]);
		// generate test.txt for libsvm
		compute_testing(argv[2], argv[5]);
	}
	return 0;
}
