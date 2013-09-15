/*
 * GlobalConfig.h
 *
 *  Created on: Jun 24, 2013
 *      Author: daniewang
 */

#ifndef GLOBALCONFIG_H_
#define GLOBALCONFIG_H_

class GlobalConfig {
public:
	GlobalConfig();
	~GlobalConfig();
	static int angle_bins;
	static float angle_offset;
	static int pca_dimension;
};

#endif /* GLOBALCONFIG_H_ */
