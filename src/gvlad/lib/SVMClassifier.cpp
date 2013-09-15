/*
 * SVMClassifier.cpp
 *
 *  Created on: Aug 5, 2013
 *      Author: daniewang
 */

#include "SVMClassifier.h"

SVMClassifier::SVMClassifier() {

}

SVMClassifier::~SVMClassifier() {

}

void SVMClassifier::build() {
	if (m_data.empty()) {
		return;
	}
	if (m_mat.empty()) {
		int sample_count = (int) m_int_label_vector.size();
		int dimension = (int) m_data.size() / sample_count;
		m_mat = cv::Mat(sample_count, dimension, CV_32FC1);
		std::copy(m_data.begin(), m_data.end(), m_mat.ptr<float>(0));
		std::vector<float>().swap(m_data); // release memory.
	}
	CvSVMParams params;
	params.svm_type = CvSVM::C_SVC;
	params.kernel_type = CvSVM::LINEAR;
	params.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 100, 1e-6);
	std::vector<float> label_array(m_int_label_vector.begin(),
			m_int_label_vector.end());
	cv::Mat label_mat(label_array);
	m_svm.train(m_mat, label_mat, cv::Mat(), cv::Mat(), params);
}

float SVMClassifier::query(const std::vector<float>& sample) {
	cv::Mat sample_mat(sample);
	sample_mat = sample_mat.reshape(0, 1);
	return m_svm.predict(sample_mat, true);
}

void SVMClassifier::save(const std::string& filename) {
	m_svm.save(filename.c_str());
}

void SVMClassifier::load(const std::string& filename) {
	m_svm.load(filename.c_str());
}
