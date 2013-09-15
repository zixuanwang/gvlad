/*
 * Compressor.cpp
 *
 *  Created on: Jul 8, 2013
 *      Author: zxwang
 */

#include "Compressor.h"

Compressor* Compressor::p_instance = NULL;

Compressor::Compressor() {

}

Compressor* Compressor::instance() {
	if (p_instance == NULL) {
		p_instance = new Compressor;
	}
	return p_instance;
}

void Compressor::compress(std::vector<float>& compressed,
		const std::vector<float>& feature) {
	compressed.clear();
	if (feature.empty())
		return;
	float* eigen_value_ptr = m_eigenvalues.ptr<float>(0);
	cv::Mat sample(feature, false);
	sample = sample.reshape(0, 1);
	cv::Mat pca_sample(1, GlobalConfig::pca_dimension, CV_32FC1, cv::Scalar(0));
	m_pca.project(sample, pca_sample);
	compressed.assign(pca_sample.ptr<float>(0),
			pca_sample.ptr<float>(0) + pca_sample.cols);
	// do whitening.
	for (size_t j = 0; j < compressed.size(); ++j) {
		compressed[j] /= sqrtf(eigen_value_ptr[j]);
	}
	Math::l2_normalize(compressed);
}

void Compressor::train(const std::string& train_dir) {
	std::vector<std::string> file_vector;
	File::get_files(file_vector, train_dir);
	std::vector<float> sample_vector;
	int sample_count = (int) file_vector.size();
	if (sample_count > 0) {
		std::vector<float> feature;
		Serializer::load(feature, file_vector[0]);
		int cols = (int) feature.size();
		cv::Mat sample_mat(sample_count, cols, CV_32FC1, cv::Scalar(0));
		for (int i = 0; i < sample_count; ++i) {
			Serializer::load(feature, file_vector[i]);
			if ((int) feature.size() != cols)
				std::cout << "wrong feature dimension." << std::endl;
			std::copy(feature.begin(), feature.end(), sample_mat.ptr<float>(i));
		}
		m_pca(sample_mat, cv::Mat(), CV_PCA_DATA_AS_ROW,
				GlobalConfig::pca_dimension);
		m_means = m_pca.mean.clone();
		m_eigenvalues = m_pca.eigenvalues.clone();
		m_eigenvectors = m_pca.eigenvectors.clone();
	}
}

void Compressor::load(const std::string& path) {
	cv::FileStorage f(path, cv::FileStorage::READ);
	f["mean"] >> m_means;
	f["eigenvalue"] >> m_eigenvalues;
	f["eigenvectors"] >> m_eigenvectors;
	f.release();
	m_pca.mean = m_means;
	m_pca.eigenvalues = m_eigenvalues;
	m_pca.eigenvectors = m_eigenvectors;
}

void Compressor::save(const std::string& path) {
	cv::FileStorage f(path, cv::FileStorage::WRITE);
	f << "mean" << m_means;
	f << "eigenvalue" << m_eigenvalues;
	f << "eigenvectors" << m_eigenvectors;
	f.release();
}

