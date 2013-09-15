/*
 * KnnClassifier.h
 *
 *  Created on: May 24, 2013
 *      Author: daniewang
 */

#ifndef KNNCLASSIFIER_H_
#define KNNCLASSIFIER_H_

#include <boost/shared_ptr.hpp>
#include "Classifier.h"
#include "RankItem.h"

class KnnClassifier: public Classifier {
public:
	KnnClassifier(bool use_ann = true);
	~KnnClassifier(void);
	// create the ann structure for fast nearest neighbor search.
	void build();
	// return n nearest neighbors.
	void query(std::vector<int>& label_vector, const std::vector<float>& sample,
			int n);
	void query(std::vector<std::string>& label_vector,
			const std::vector<float>& sample, int n);
	// return n nearest neighbors and distances.
	void query(std::vector<int>& label_vector,
			std::vector<float>& distance_vector,
			const std::vector<float>& sample, int n);
	void query(std::vector<std::string>& label_vector,
			std::vector<float>& distance_vector,
			const std::vector<float>& sample, int n);
	// serialization.
	void save(const std::string& file_prefix);
	void load(const std::string& file_prefix);
	int majority(const std::vector<int>& array);
private:
	void query_nn(std::vector<int>& index_vector,
			const std::vector<float>& sample, int n);
	void query_nn(std::vector<int>& index_vector,
			std::vector<float>& distance_vector,
			const std::vector<float>& sample, int n);
	boost::shared_ptr<cv::flann::Index> m_index;
	bool m_use_ann;
};

#endif /* KNNCLASSIFIER_H_ */
