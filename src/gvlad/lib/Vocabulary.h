/*
 * Vocabulary.h
 *
 *  Created on: May 22, 2013
 *      Author: daniewang
 */

#ifndef VOCABULARY_H_
#define VOCABULARY_H_

#include "Angle.h"
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>
#include <fstream>
#include "GeoDescriptorVector.h"
#include "GlobalConfig.h"
#include "Math.h"
#include <opencv2/opencv.hpp>

class Vocabulary {
public:
	// return the pointer to the singleton.
	static Vocabulary* instance();
	// compute cluster centers of samples. now k-means is used.
	void build(const cv::Mat& sample_mat, int cluster_count);
	// quantize the descriptor to the nearest visual word.
	// the index of the visual word is returned.
	int quantize(const std::vector<float>& descriptor);
	// find the closest cluster center for each sample and returns the frequency histogram.
	// the returned vector is l2 normalized.
	// each sample is stored in one row.
	void compute_bow(std::vector<float>& vector, const cv::Mat& descriptor_mat);
	// compute vlad representation. the returned vector is l2 normalized.
	// this implements 10' CVPR paper.
	void compute_vlad(std::vector<float>& vector,
			const GeoDescriptorVector<float>& geo_descriptor_vector);
	// compute angle bins vlad with vocabulary adaptation.
	// row normalization + z normalization.
	// this is the implementation of the paper.
	void compute_gvlad(std::vector<float>& vector,
			const GeoDescriptorVector<float>& geo_descriptor_vector);
	// serialize cluster centers.
	void load(const std::string& vocabulary_path,
			const std::string& index_path = "");
	void save(const std::string& vocabulary_path,
			const std::string& index_path = "");
	// serialize adaptation cluster centers.
	void load_adaptation(const std::string& vocabulary_path);
	void save_adaptation(const std::string& vocabulary_path);
	// return the vocabulary.
	cv::Mat get_vocabulary() {
		return m_mat;
	}
	// return the number of visual words.
	int rows() {
		return m_mat.rows;
	}
	// return the descriptor dimensions.
	int cols() {
		return m_mat.cols;
	}
	// load the cluster centers trained from flickr using the format from
	// http://lear.inrialpes.fr/~jegou/data.php
	void load_flickr_cluster(const std::string& file);
private:
	Vocabulary();
	Vocabulary(const Vocabulary&);
	Vocabulary& operator=(const Vocabulary&);
	static Vocabulary* p_instance;
	cv::Mat m_mat;
	cv::Mat m_adaptation_mat;
	std::vector<cv::Mat> m_adaptation_mat_vector;
	boost::shared_ptr<cv::flann::Index> m_index;
};

#endif /* VOCABULARY_H_ */
