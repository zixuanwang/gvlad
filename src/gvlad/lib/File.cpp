/*
 * File.cpp
 *
 *  Created on: May 22, 2013
 *      Author: daniewang
 */

#include "File.h"

File::File() {

}

File::~File() {

}

void File::get_files(std::vector<std::string>& file_vector,
		const std::string& directory, bool recursive,
		const std::string& extension) {
	if (!boost::filesystem::exists(directory)) {
		return;
	}
	boost::filesystem::directory_iterator end_itr;
	for (boost::filesystem::directory_iterator itr(directory); itr != end_itr;
			++itr) {
		if (recursive && boost::filesystem::is_directory(itr->status())) {
			get_files(file_vector, itr->path().string(), recursive);
		} else {
			if (extension.empty()
					|| itr->path().extension().string() == extension) {
				if (get_name(itr->path().string())[0] != '.') { // ignore hidden files
					file_vector.push_back(itr->path().string());
				}
			}
		}
	}
}

std::string File::get_parent_path(const std::string& filepath) {
	boost::filesystem::path bst_filepath = filepath;
	return bst_filepath.parent_path().string();
}

std::string File::get_parent_name(const std::string& filepath) {
	boost::filesystem::path bst_filepath = filepath;
	return bst_filepath.filename().string();
}

std::string File::get_stem(const std::string& filepath) {
	boost::filesystem::path bst_filepath = filepath;
	return bst_filepath.stem().string();
}

std::string File::get_extension(const std::string& filepath) {
	boost::filesystem::path bst_filepath = filepath;
	return bst_filepath.extension().string();
}

std::string File::get_stem_path(const std::string& filepath) {
	boost::filesystem::path bst_filepath = filepath;
	return (bst_filepath.parent_path() / bst_filepath.stem()).string();
}

std::string File::get_name(const std::string& filepath) {
	boost::filesystem::path bst_filepath = filepath;
	return bst_filepath.filename().string();
}
