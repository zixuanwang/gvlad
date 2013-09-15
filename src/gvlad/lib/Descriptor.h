/*
 * Descriptor.h
 *
 *  Created on: May 23, 2013
 *      Author: daniewang
 */

#ifndef DESCRIPTOR_H_
#define DESCRIPTOR_H_

#include <fstream>
#include <iostream>
#include <vector>

template<class T>
class Descriptor {
public:
	Descriptor() {
	}
	~Descriptor() {
	}
	// binary serialization.
	void load(std::istream& is) {
		int desdim;
		is.read((char*) &desdim, sizeof(desdim));
		component.assign(desdim, (T) 0);
		if (desdim > 0)
			is.read((char*) &component[0], desdim * sizeof(T));
	}
	void save(std::ostream& os) const {
		int desdim = dimension();
		os.write((char*) &desdim, sizeof(desdim));
		if (desdim > 0)
			os.write((char*) &component[0], desdim * sizeof(T));
	}
	// return the storage in bytes.
	int byte_size() const {
		return sizeof(T) * component.size();
	}
	// return the dimension of the descriptor.
	int dimension() const {
		return (int) component.size();
	}
	// copy data from different type.
	template<typename U> void copy_from(const Descriptor<U>& descriptor) {
		component.clear();
		component.reserve(descriptor.component.size());
		for (size_t i = 0; i < descriptor.component.size(); ++i) {
			component.push_back(static_cast<T>(descriptor.component[i]));
		}
	}
	// store the data in the vector.
	std::vector<T> component;
};

// print the descriptor for debug.
template<typename T>
std::ostream& operator<<(std::ostream& os, const Descriptor<T>& descriptor) {
	for (size_t i = 0; i < descriptor.component.size(); ++i) {
		os << (float) descriptor.component[i] << " ";
	}
	os << std::endl;
	return os;
}

#endif /* DESCRIPTOR_H_ */
