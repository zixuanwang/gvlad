/*
 * crop_voc.cpp
 *
 *  Created on: Aug 1, 2013
 *      Author: daniewang
 */

#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/unordered_map.hpp>
#include <fstream>
#include "gvlad/lib/File.h"
#include "gvlad/lib/rapidxml/rapidxml.hpp"
#include <opencv2/opencv.hpp>
#include <streambuf>

std::string image_dir;
std::string xml_dir;
std::string crop_dir;
std::vector<std::string> file_vector;
boost::unordered_map<std::string, std::string> bb_map;

void crop_image(size_t i) {
	std::string stem = File::get_stem(file_vector[i]);
	cv::Mat image = cv::imread(file_vector[i]);
	cv::Mat crop = image;
	std::ifstream f((xml_dir + "/" + stem + ".xml").c_str());
	std::string xml_str((std::istreambuf_iterator<char>(f)),
			std::istreambuf_iterator<char>());
	rapidxml::xml_document<> doc;
	doc.parse<0>((char*) xml_str.c_str());
	rapidxml::xml_node<> *node = doc.first_node("annotation");
	node = node->first_node("object");
	int j = 0;
	do {
		if (node) {
			rapidxml::xml_node<> *name_node = node->first_node("name");
			rapidxml::xml_node<> *difficult_node = node->first_node("difficult");
			rapidxml::xml_node<> *bndbox_node = node->first_node("bndbox");
			rapidxml::xml_node<> *xmin_node = bndbox_node->first_node("xmin");
			rapidxml::xml_node<> *ymin_node = bndbox_node->first_node("ymin");
			rapidxml::xml_node<> *xmax_node = bndbox_node->first_node("xmax");
			rapidxml::xml_node<> *ymax_node = bndbox_node->first_node("ymax");
			int difficult = atoi(difficult_node->value());
			if(difficult != 0) continue; // ignore all difficult bounding boxes.
			int x = atoi(xmin_node->value());
			int y = atoi(ymin_node->value());
			int width = atoi(xmax_node->value()) - x;
			int height = atoi(ymax_node->value()) - y;
			crop = image(cv::Rect(x, y, width, height));
			std::string filename = stem + "_" + name_node->value() + "_"
					+ boost::lexical_cast<std::string>(j) + ".jpg";
			cv::imwrite(crop_dir + "/" + filename, crop);
			node = node->next_sibling("object");
			++j;
		}
	} while (node);
	std::cout << file_vector[i] << std::endl;
}

int main(int argc, char* argv[]) {
	if (argc == 1) {
		std::cout << "Usage: ./crop_voc image_dir xml_dir crop_dir"
				<< std::endl;
		std::cout << "image_dir is the directory of the images in VOC dataset." << std::endl;
		std::cout << "xml_dir is the directory of xml annotations." << std::endl;
		std::cout << "crop_dir is the output directory of cropped images." << std::endl;
		std::cout << "For example: ./crop_voc ~/JPEGImages ~/Annotations ~/crop" << std::endl;
		return 0;
	}
	if (argc == 4) {
		image_dir = argv[1];
		xml_dir = argv[2];
		crop_dir = argv[3];
		boost::filesystem::create_directories(crop_dir);
		File::get_files(file_vector, image_dir, true);
		for(size_t i = 0; i < file_vector.size(); ++i){
			crop_image(i);
		}
	}
	return 0;
}
