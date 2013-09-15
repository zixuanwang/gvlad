/*
 * KnnClassifier.cpp
 *
 *  Created on: May 24, 2013
 *      Author: daniewang
 */

#include "KnnClassifier.h"

KnnClassifier::KnnClassifier(bool use_ann) :
		m_use_ann(use_ann) {
}

KnnClassifier::~KnnClassifier(void) {
}

void KnnClassifier::build() {
	if (m_data.empty()) {
		return;
	}
	if (m_mat.empty()) {
		int sample_count = (int) std::max(m_int_label_vector.size(),
				m_string_label_vector.size());
		int dimension = (int) m_data.size() / sample_count;
		m_mat = cv::Mat(sample_count, dimension, CV_32FC1);
		std::copy(m_data.begin(), m_data.end(), m_mat.ptr<float>(0));
		std::vector<float>().swap(m_data); // release memory.
	}
	if (m_use_ann) {
		m_index = boost::shared_ptr<cv::flann::Index>(
				new cv::flann::Index(m_mat, cv::flann::KDTreeIndexParams(8)));
	} else {
		m_index = boost::shared_ptr<cv::flann::Index>(
				new cv::flann::Index(m_mat, cv::flann::LinearIndexParams())); // brute force.
	}
}

void KnnClassifier::query(std::vector<int>& label_vector,
		const std::vector<float>& sample, int n) {
	label_vector.clear();
	if (sample.empty()) {
		return;
	}
	label_vector.reserve(n);
	std::vector<int> index_vector;
	query_nn(index_vector, sample, n);
	for (size_t i = 0; i < index_vector.size(); ++i) {
		label_vector.push_back(m_int_label_vector[index_vector[i]]);
	}
}

void KnnClassifier::query(std::vector<std::string>& label_vector,
		const std::vector<float>& sample, int n) {
	label_vector.clear();
	if (sample.empty()) {
		return;
	}
	label_vector.reserve(n);
	std::vector<int> index_vector;
	query_nn(index_vector, sample, n);
	for (size_t i = 0; i < index_vector.size(); ++i) {
		label_vector.push_back(m_string_label_vector[index_vector[i]]);
	}
}

void KnnClassifier::query(std::vector<int>& label_vector,
		std::vector<float>& distance_vector, const std::vector<float>& sample,
		int n) {
	label_vector.clear();
	distance_vector.clear();
	if (sample.empty()) {
		return;
	}
	label_vector.reserve(n);
	std::vector<int> index_vector;
	query_nn(index_vector, distance_vector, sample, n);
	for (size_t i = 0; i < index_vector.size(); ++i) {
		label_vector.push_back(m_int_label_vector[index_vector[i]]);
	}
}

void KnnClassifier::query(std::vector<std::string>& label_vector,
		std::vector<float>& distance_vector, const std::vector<float>& sample,
		int n) {
	label_vector.clear();
	distance_vector.clear();
	if (sample.empty()) {
		return;
	}
	label_vector.reserve(n);
	std::vector<int> index_vector;
	query_nn(index_vector, distance_vector, sample, n);
	for (size_t i = 0; i < index_vector.size(); ++i) {
		label_vector.push_back(m_string_label_vector[index_vector[i]]);
	}
}

int KnnClassifier::majority(const std::vector<int>& array) {
	int maxCount = 0;
	int entry = -1;
	for (size_t i = 0; i < array.size(); ++i) {
		int count = (int) std::count(array.begin(), array.end(), array[i]);
		if (count > maxCount) {
			maxCount = count;
			entry = array[i];
		}
	}
	return entry;
}

void KnnClassifier::save(const std::string& file_prefix) {
	Classifier::save(file_prefix);
	if (m_index.get() != NULL) {
		m_index->save(file_prefix + ".index");
	}
}

void KnnClassifier::load(const std::string& file_prefix) {
	Classifier::load(file_prefix);
	if (!m_mat.empty()) {
		m_index = boost::shared_ptr<cv::flann::Index>(
				new cv::flann::Index(m_mat,
						cv::flann::SavedIndexParams(file_prefix + ".index")));
	}
}

void KnnClassifier::query_nn(std::vector<int>& index_vector,
		const std::vector<float>& sample, int n) {
	std::vector<float> distance_vector;
	query_nn(index_vector, distance_vector, sample, n);
}

void KnnClassifier::query_nn(std::vector<int>& index_vector,
		std::vector<float>& distance_vector, const std::vector<float>& sample,
		int n) {
	index_vector.assign(n, 0);
	distance_vector.assign(n, 0.f);
	m_index->knnSearch(sample, index_vector, distance_vector, n,
			cv::flann::SearchParams(128));
}
