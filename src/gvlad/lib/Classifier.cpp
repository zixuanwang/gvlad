/*
 * Classifier.cpp
 *
 *  Created on: May 24, 2013
 *      Author: daniewang
 */

#include "Classifier.h"

Classifier::Classifier(void) {
}

Classifier::~Classifier(void) {
}

void Classifier::add_sample(const std::vector<float>& sample, int label) {
	if (sample.empty()) {
		return;
	}
	std::copy(sample.begin(), sample.end(), std::back_inserter(m_data));
	m_int_label_vector.push_back(label);
}

void Classifier::add_sample(const std::vector<float>& sample, const std::string& label){
	if (sample.empty()) {
		return;
	}
	std::copy(sample.begin(), sample.end(), std::back_inserter(m_data));
	m_string_label_vector.push_back(label);
}

int Classifier::get_sample_count(){
	return m_mat.rows;
}

void Classifier::load(const std::string& file_prefix) {
	Serializer::load(m_mat, file_prefix + ".matrix");
	Serializer::load(m_int_label_vector, file_prefix + ".ilabel");
	Serializer::load(m_string_label_vector, file_prefix + ".slabel");
}

void Classifier::save(const std::string& file_prefix) {
	Serializer::save(m_mat, file_prefix + ".matrix");
	if(!m_int_label_vector.empty())
		Serializer::save(m_int_label_vector, file_prefix + ".ilabel");
	if(!m_string_label_vector.empty())
		Serializer::save(m_string_label_vector, file_prefix + ".slabel");
}
