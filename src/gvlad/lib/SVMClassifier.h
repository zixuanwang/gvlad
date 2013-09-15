/*
 * SVMClassifier.h
 *
 *  Created on: Aug 5, 2013
 *      Author: daniewang
 */

#ifndef SVMCLASSIFIER_H_
#define SVMCLASSIFIER_H_

#include "Classifier.h"

class SVMClassifier: public Classifier {
public:
	SVMClassifier();
	~SVMClassifier();
	// create the svm from the data.
	void build();
	// query. the distance is returned.
	float query(const std::vector<float>& sample);
	// serialization.
    void save(const std::string& filename);
    void load(const std::string& filename);
private:
	CvSVM m_svm;
};

#endif /* SVMCLASSIFIER_H_ */
