/*
 * File.h
 *
 *  Created on: May 22, 2013
 *      Author: daniewang
 */

#ifndef FILE_H_
#define FILE_H_

#include <boost/filesystem.hpp>
#include <vector>

class File {
public:
	File();
	~File();
    static void get_files(std::vector<std::string>& file_vector, const std::string& directory, bool recursive = false, const std::string& extension = "");
    static std::string get_parent_path(const std::string& filepath); // /usr/bin/foo.txt returns /usr/bin
    static std::string get_parent_name(const std::string& filepath); // /usr/bin/foo.txt returns bin
    static std::string get_stem(const std::string& filepath); // /usr/bin/foo.txt returns foo
    static std::string get_extension(const std::string& filepath); // /usr/bin/foo.txt returns .txt
    static std::string get_stem_path(const std::string& filepath); // /usr/bin/foo.txt returns /usr/bin/foo
    static std::string get_name(const std::string& filepath); // /usr/bin/foo.txt returns foo.txt
};

#endif /* FILE_H_ */
