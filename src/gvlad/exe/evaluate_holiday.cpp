/*
 * evalute_holiday.cpp
 *
 *  Created on: Oct 24, 2013
 *      Author: zxwang
 */

#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include "gvlad/lib/File.h"
#include "gvlad/lib/KnnClassifier.h"
#include "gvlad/lib/RankItem.h"
#include "gvlad/lib/Vocabulary.h"

boost::unordered_map<std::string, boost::unordered_set<std::string> > ground_truth_map;
boost::unordered_set<std::string> holiday_set;

void load_ground_truth(const std::string& ground_truth,
		const std::string& holiday_image) {
	std::ifstream in_stream;
	in_stream.open(ground_truth.c_str());
	if (in_stream.good()) {
		std::string line;
		while (getline(in_stream, line)) {
			boost::trim(line);
			if (!line.empty()) {
				std::vector<std::string> string_vector;
				boost::split(string_vector, line, boost::is_any_of(" "));
				int count = ((int) string_vector.size() - 1) / 2;
				for (int i = 0; i < count; ++i) {
					ground_truth_map[File::get_stem(string_vector[0])].insert(
							File::get_stem(string_vector[2 * i + 2]));
				}
			}
		}
		in_stream.close();
	}
	in_stream.open(holiday_image.c_str());
	if (in_stream.good()) {
		std::string line;
		while (getline(in_stream, line)) {
			boost::trim(line);
			if (!line.empty()) {
				holiday_set.insert(File::get_stem(line));
			}
		}
		in_stream.close();
	}
}

void evaluate_holiday(const std::string& feature_dir,
		const std::string& result) {
	std::vector<std::string> feature_vector;
	File::get_files(feature_vector, feature_dir);
	if (feature_vector.empty()) {
		std::cerr << "feature_dir is empty" << std::endl;
		return;
	}
	boost::filesystem::create_directories(File::get_parent_path(result));
	std::string extension = File::get_extension(feature_vector[0]);
	KnnClassifier knn(false);
	for (size_t i = 0; i < feature_vector.size(); ++i) {
		std::string stem = File::get_stem(feature_vector[i]);
		std::vector<float> feature;
		Serializer::load(feature, feature_vector[i]);
		knn.add_sample(feature, stem);
	}
	knn.build();
	std::ofstream out_stream;
	out_stream.open(result.c_str());
	if (out_stream.good()) {
		for (boost::unordered_map<std::string, boost::unordered_set<std::string> >::iterator iter =
				ground_truth_map.begin(); iter != ground_truth_map.end();
				++iter) {
			std::set<std::string> positive_set(iter->second.begin(),
					iter->second.end());
			std::set<std::string> top_ten_set;
			std::vector<float> query_feature;
			Serializer::load(query_feature,
					feature_dir + "/" + iter->first + extension);
			out_stream << iter->first << ".jpg ";
			if (query_feature.empty()) {
				out_stream << std::endl;
				continue;
			}
			std::vector<std::string> result_vector;
			knn.query(result_vector, query_feature, knn.get_sample_count());
			// output format conforms the example
			// first output top 10
			for (int i = 0; i < 10; ++i) {
				if (holiday_set.find(result_vector[i]) != holiday_set.end())
					out_stream << i << " " << result_vector[i] << ".jpg ";
				top_ten_set.insert(result_vector[i]);
			}
			// then output the rest
			std::set<std::string> rest_set;
			std::set_difference(positive_set.begin(), positive_set.end(),
					top_ten_set.begin(), top_ten_set.end(),
					std::inserter(rest_set, rest_set.begin()));
			std::vector<RankItem<std::string, int> > rank_list;
			for (std::set<std::string>::iterator it = rest_set.begin();
					it != rest_set.end(); ++it) {
				std::vector<std::string>::iterator i = std::find_if(
						result_vector.begin(), result_vector.end(),
						std::bind2nd(std::equal_to<std::string>(), *it));
				RankItem<std::string, int> item;
				item.index = *it;
				item.value = i - result_vector.begin();
				rank_list.push_back(item);
			}
			std::sort(rank_list.begin(), rank_list.end());
			for (size_t i = 0; i < rank_list.size(); ++i) {
				if (holiday_set.find(rank_list[i].index) != holiday_set.end())
					out_stream << rank_list[i].value << " "
							<< rank_list[i].index << ".jpg ";
			}
			out_stream << std::endl;
		}
		out_stream.close();
	}
}

// evaluate the holiday dataset.
int main(int argc, char* argv[]) {
	if (argc == 1) {
		std::cout
				<< "Usage: Usage: ./evaluate_holiday perfect_result.dat holidays_images.dat feature_dir result.txt"
				<< std::endl;
		std::cout
				<< "perfect_result.dat and holidays_images.dat are provided in the evaluation package."
				<< std::endl;
		std::cout << "feature_dir : the folder storing image feature."
				<< std::endl;
		std::cout
				<< "After run this program, run python holidays_map.py result.dat"
				<< std::endl;
		return 0;
	}
	if (argc == 5) {
		load_ground_truth(argv[1], argv[2]);
		evaluate_holiday(argv[3], argv[4]);
	}
	return 0;
}
