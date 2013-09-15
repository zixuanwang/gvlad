/*
 * Serializer.cpp
 *
 *  Created on: May 24, 2013
 *      Author: daniewang
 */

#include "Serializer.h"

Serializer::Serializer() {

}

Serializer::~Serializer() {

}

void Serializer::load(cv::Mat& mat, const std::string& path) {
	mat = cv::Mat();
	std::ifstream inStream;
	inStream.open(path.c_str(), std::ios::binary);
	if (inStream.good()) {
		int rows;
		int cols;
		int type;
		inStream.read((char*) &rows, sizeof(rows));
		inStream.read((char*) &cols, sizeof(cols));
		inStream.read((char*) &type, sizeof(type));
		mat = cv::Mat(rows, cols, type);
		size_t length = (size_t) rows * mat.step;
		inStream.read((char*) mat.data, length);
		inStream.close();
	}
}

void Serializer::save(const cv::Mat& mat, const std::string& path) {
	if (mat.empty()) {
		return;
	}
	std::ofstream outStream;
	outStream.open(path.c_str(), std::ios::binary);
	if (outStream.good()) {
		int rows = mat.rows;
		int cols = mat.cols;
		int type = mat.type();
		outStream.write((char*) &rows, sizeof(rows));
		outStream.write((char*) &cols, sizeof(cols));
		outStream.write((char*) &type, sizeof(type));
		size_t length = (size_t) rows * mat.step;
		outStream.write((char*) mat.data, length);
		outStream.close();
	}
}

void Serializer::load_matlab(cv::Mat& mat, const std::string& path) {
	mat = cv::Mat();
	std::ifstream in_stream;
	in_stream.open(path.c_str());
	if (in_stream.good()) {
		std::string line;
		std::vector<float> data;
		int rows = 0;
		while (getline(in_stream, line)) {
			boost::trim(line);
			std::vector<std::string> token_vector;
			boost::split(token_vector, line, boost::is_any_of("\t "),
					boost::token_compress_on);
			for (size_t i = 0; i < token_vector.size(); ++i) {
				data.push_back(boost::lexical_cast<float>(token_vector[i]));
			}
			++rows;
		}
		mat = cv::Mat(data, true); // copy the data.
		mat = mat.reshape(0, rows);
		in_stream.close();
	}
}

void Serializer::load_svm(cv::Mat& mat, std::vector<int>& label,
		const std::string& path) {
	mat = cv::Mat();
	label.clear();
	std::vector<float> data;
	std::ifstream in_stream;
	in_stream.open(path.c_str());
	if (in_stream.good()) {
		std::string line;
		while (getline(in_stream, line)) {
			boost::trim(line);
			if (!line.empty()) {
				std::vector<std::string> token_vector;
				boost::split(token_vector, line, boost::is_any_of(" "));
				if (!token_vector.empty()) {
					label.push_back(boost::lexical_cast<int>(token_vector[0]));
					for (size_t i = 1; i < token_vector.size(); ++i) {
						size_t pos = token_vector[i].find_first_of(":");
						data.push_back(
								boost::lexical_cast<float>(
										token_vector[i].substr(pos + 1)));
					}
				}
			}
		}
		mat = cv::Mat(data, true);
		mat = mat.reshape(0, label.size());
		in_stream.close();
	}
}
