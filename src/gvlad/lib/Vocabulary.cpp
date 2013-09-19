/*
 * Vocabulary.cpp
 *
 *  Created on: May 22, 2013
 *      Author: daniewang
 */

#include "Vocabulary.h"

Vocabulary* Vocabulary::p_instance = NULL;

Vocabulary::Vocabulary() {

}

Vocabulary* Vocabulary::instance() {
	if (p_instance == NULL) {
		p_instance = new Vocabulary;
	}
	return p_instance;
}

void Vocabulary::build(const cv::Mat& sample_mat, int cluster_count) {
	// k-means is temporarily used here
	cv::Mat clusters;
	cv::kmeans(sample_mat, cluster_count, clusters,
			cv::TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 100, 1e-6), 1,
			cv::KMEANS_PP_CENTERS, m_mat);
	m_index = boost::shared_ptr<cv::flann::Index>(
			new cv::flann::Index(m_mat, cv::flann::KDTreeIndexParams(8)));
}

int Vocabulary::quantize(const std::vector<float>& descriptor) {
	std::vector<int> indices(1);
	std::vector<float> dists(1);
	m_index->knnSearch(descriptor, indices, dists, 1,
			cv::flann::SearchParams(64));
	return indices[0];
}

void Vocabulary::compute_bow(std::vector<float>& vector,
		const cv::Mat& descriptor_mat) {
	if (descriptor_mat.empty()) {
		vector.clear();
		return;
	}
	cv::Mat index_mat;
	cv::Mat distance_mat;
	m_index->knnSearch(descriptor_mat, index_mat, distance_mat, 1,
			cv::flann::SearchParams(64));
	cv::Mat histogram_mat(m_mat.rows, 1, CV_32FC1, cv::Scalar(0));
	for (int i = 0; i < index_mat.rows; ++i) {
		int* ptr = index_mat.ptr<int>(i);
		float* histogram_mat_ptr = histogram_mat.ptr<float>(*ptr);
		*histogram_mat_ptr += 1.f;
	}
	cv::normalize(histogram_mat, histogram_mat);
	vector.assign(histogram_mat.ptr<float>(0),
			histogram_mat.ptr<float>(0) + histogram_mat.rows);
}

void Vocabulary::compute_vlad(std::vector<float>& vector,
		const GeoDescriptorVector<float>& geo_descriptor_vector) {
	if (!vector.empty())
		vector.clear();
	if (geo_descriptor_vector.geo_descriptor_vector.empty()) {
		return;
	}
	cv::Mat descriptor_mat;
	geo_descriptor_vector.convert(descriptor_mat);
	cv::Mat index_mat;
	cv::Mat distance_mat;
	m_index->knnSearch(descriptor_mat, index_mat, distance_mat, 1,
			cv::flann::SearchParams(64));
	cv::Mat vlad_mat(m_mat.rows, m_mat.cols, CV_32FC1, cv::Scalar(0));
	for (int i = 0; i < index_mat.rows; ++i) {
		int* ptr = index_mat.ptr<int>(i);
		vlad_mat.row(*ptr) += descriptor_mat.row(i) - m_mat.row(*ptr);
	}
	// normalization
	Math::power_normalize(vlad_mat);
//	Math::row_normalize(vlad_mat);
//	Math::z_normalize(vlad_mat);
	// l2 normalization
	cv::normalize(vlad_mat, vlad_mat);
	vector.assign(vlad_mat.ptr<float>(0),
			vlad_mat.ptr<float>(0) + vlad_mat.rows * vlad_mat.cols);
}

void Vocabulary::compute_gvlad(std::vector<float>& vector,
		const GeoDescriptorVector<float>& geo_descriptor_vector) {
	if (!vector.empty())
		vector.clear();
	if (geo_descriptor_vector.geo_descriptor_vector.empty()) {
		return;
	}
	cv::Mat descriptor_mat;
	geo_descriptor_vector.convert(descriptor_mat);
	cv::Mat index_mat;
	cv::Mat distance_mat;
	m_index->knnSearch(descriptor_mat, index_mat, distance_mat, 1,
			cv::flann::SearchParams(64));
	cv::Mat vlad_mat(m_mat.rows, m_mat.cols * GlobalConfig::angle_bins,
	CV_32FC1, cv::Scalar(0));
	for (int i = 0; i < index_mat.rows; ++i) {
		int angle_index = Angle::get_index(
				geo_descriptor_vector.geo_descriptor_vector[i].angle);
		int word_index = index_mat.at<int>(i, 0);
		cv::Mat row = vlad_mat(
				cv::Rect(angle_index * m_mat.cols, word_index, m_mat.cols, 1));
		row += descriptor_mat.row(i)
				- m_adaptation_mat_vector[angle_index].row(word_index);
	}
	Math::row_normalize(vlad_mat);
	Math::z_normalize(vlad_mat);
	cv::normalize(vlad_mat, vlad_mat);
	vector.assign(vlad_mat.ptr<float>(0),
			vlad_mat.ptr<float>(0) + vlad_mat.rows * vlad_mat.cols);
}

void Vocabulary::load(const std::string& vocabulary_path,
		const std::string& index_path) {
	cv::FileStorage f(vocabulary_path, cv::FileStorage::READ);
	f["mat"] >> m_mat;
	f.release();
	if (index_path.empty()) {
		m_index = boost::shared_ptr<cv::flann::Index>(
				new cv::flann::Index(m_mat, cv::flann::LinearIndexParams()));
	} else {
		m_index = boost::shared_ptr<cv::flann::Index>(
				new cv::flann::Index(m_mat,
						cv::flann::SavedIndexParams(index_path)));
	}
}

void Vocabulary::save(const std::string& vocabulary_path,
		const std::string& index_path) {
	cv::FileStorage f(vocabulary_path, cv::FileStorage::WRITE);
	f << "mat" << m_mat;
	f.release();
	if (!index_path.empty())
		m_index->save(index_path);
}

void Vocabulary::load_adaptation(const std::string& vocabulary_path) {
	cv::FileStorage f(vocabulary_path, cv::FileStorage::READ);
	f["mat"] >> m_adaptation_mat;
	int bins = m_adaptation_mat.cols / m_mat.cols;
	m_adaptation_mat_vector.clear();
	for (int i = 0; i < bins; ++i) {
		m_adaptation_mat_vector.push_back(
				m_adaptation_mat(
						cv::Rect(i * m_mat.cols, 0, m_mat.cols, m_mat.rows)));
	}
	f.release();
}

void Vocabulary::save_adaptation(const std::string& vocabulary_path) {
	cv::FileStorage f(vocabulary_path, cv::FileStorage::WRITE);
	f << "mat" << m_adaptation_mat;
	f.release();
}

void Vocabulary::load_flickr_cluster(const std::string& file) {
	std::ifstream in_stream;
	in_stream.open(file.c_str(), std::ios::binary);
	if (in_stream.good()) {
		in_stream.seekg(0, in_stream.end);
		int length = in_stream.tellg();
		in_stream.seekg(0, in_stream.beg);
		if (length > 0) {
			int cluster_count = length / 516;
			std::vector<float> cluster_vector(128 * cluster_count);
			for (int i = 0; i < cluster_count; ++i) {
				int desdim;
				in_stream.read((char*) &desdim, sizeof(desdim));
				in_stream.read((char*) &cluster_vector[i * desdim],
						sizeof(float) * desdim);
			}
			m_mat = cv::Mat(cluster_vector, true);
			m_mat = m_mat.reshape(0, cluster_count);
			m_index = boost::shared_ptr<cv::flann::Index>(
					new cv::flann::Index(m_mat,
							cv::flann::KDTreeIndexParams(8)));
		}
		in_stream.close();
	}
}

