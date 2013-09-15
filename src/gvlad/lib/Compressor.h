/*
 * Compressor.h
 *
 *  Created on: Jul 8, 2013
 *      Author: zxwang
 */

#ifndef COMPRESSOR_H_
#define COMPRESSOR_H_

#include "File.h"
#include "GlobalConfig.h"
#include "Math.h"
#include <opencv2/opencv.hpp>
#include "Serializer.h"

class Compressor {
public:
	static Compressor* instance();
	void compress(std::vector<float>& compressed,
			const std::vector<float>& feature);
	void train(const std::string& train_dir);
	// serialization.
	void load(const std::string& path);
	void save(const std::string& path);
private:
	Compressor();
	Compressor(const Compressor&);
	Compressor& operator=(const Compressor&);
	static Compressor* p_instance;
	cv::Mat m_means;
	cv::Mat m_eigenvalues;
	cv::Mat m_eigenvectors;
	cv::PCA m_pca;
};

#endif /* COMPRESSOR_H_ */
