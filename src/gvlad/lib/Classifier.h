/*
 * Classifier.h
 *
 *  Created on: May 24, 2013
 *      Author: daniewang
 */

#ifndef CLASSIFIER_H_
#define CLASSIFIER_H_

#include "Serializer.h"

class Classifier
{
public:
    Classifier();
    virtual ~Classifier();
    // add a training sample to the classifier.
    void add_sample(const std::vector<float>& sample, int label);
    void add_sample(const std::vector<float>& sample, const std::string& label);
    // build the classifier structure. called after adding samples.
    virtual void build() = 0;
	// get sample count.
	int get_sample_count();
    // serialize the data structures.
	virtual void load(const std::string& file_prefix);
    virtual void save(const std::string& file_prefix);
protected:
    std::vector<float> m_data;
    cv::Mat m_mat; // each sample is stored in one row.
    std::vector<int> m_int_label_vector;
    std::vector<std::string> m_string_label_vector;
};

#endif /* CLASSIFIER_H_ */
