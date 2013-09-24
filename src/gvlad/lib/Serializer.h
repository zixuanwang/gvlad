/*
 * Serializer.h
 *
 *  Created on: May 24, 2013
 *      Author: daniewang
 */

#ifndef SERIALIZER_H_
#define SERIALIZER_H_

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <fstream>
#include <opencv2/opencv.hpp>

class Serializer {
public:
	Serializer();
	~Serializer();
	static void load(cv::Mat& mat, const std::string& path);
	static void save(const cv::Mat& mat, const std::string& path);
	// the format is compatible with libsvm
	static void load_svm(cv::Mat& mat, std::vector<int>& label,
			const std::string& path);
	// the format is compatible with save('s.txt', 'S', '-ASCII'); in matlab.
	static void load_matlab(cv::Mat& mat, const std::string& path);
	template<class T> static void load(std::vector<T>& array,
			const std::string& path) {
		array.clear();
		std::ifstream in_stream;
		in_stream.open(path.c_str(), std::ios::binary);
		if (in_stream.good()) {
			size_t size;
			in_stream.read((char*) &size, sizeof(size));
			array.assign(size, 0);
			in_stream.read((char*) &array[0], sizeof(T) * size);
			in_stream.close();
		}
	}
	template<class T> static void save(const std::vector<T>& array,
			const std::string& path) {
		if (array.empty()) {
			return;
		}
		std::ofstream out_stream;
		out_stream.open(path.c_str(), std::ios::binary);
		if (out_stream.good()) {
			size_t size = array.size();
			out_stream.write((char*) &size, sizeof(size));
			out_stream.write((char*) &array[0], sizeof(T) * size);
			out_stream.close();
		}
	}
	template<class T> static void load_text(std::vector<T>& array,
			const std::string& path) {
		array.clear();
		std::ifstream in_stream;
		in_stream.open(path.c_str());
		if (in_stream.good()) {
			std::string line;
			while (getline(in_stream, line)) {
				boost::trim(line);
				if (!line.empty()) {
					array.push_back(boost::lexical_cast<T>(line));
				}
			}
			in_stream.close();
		}
	}
	template<class T> static void save_text(const std::vector<T>& array,
			const std::string& path) {
		if (array.empty()) {
			return;
		}
		std::ofstream out_stream;
		out_stream.open(path.c_str());
		if (out_stream.good()) {
			for (size_t i = 0; i < array.size(); ++i) {
				out_stream << array[i] << "\n";
			}
			out_stream.close();
		}
	}
};

template<> inline void Serializer::load<std::string>(
		std::vector<std::string>& array, const std::string& path) {
	load_text(array, path);
}

template<> inline void Serializer::save<std::string>(
		const std::vector<std::string>& array, const std::string& path) {
	save_text(array, path);
}

#endif /* SERIALIZER_H_ */
