#include <string>
#include <iostream>

#include "dfsdepthmap.h"


/**
 * For depth first search, this class performs a mapping from a configuration number to the
 * lowest depth found so far for the corresponding configuration (i.e., the minimum number of
 * moves from the initial configuration to the given configuration, which has been found until
 * now).
 */


/**
 * Constructor: Creates a new mapping for configuration numbers between
 * 0 and numConf-1 and a maximum depth of 'maxDepth'.
 */
DFSDepthMap::DFSDepthMap(uint64_t numConf, uint64_t maxDepth)
{
	depth_length = index1(numConf-1) + 1;
	depth = new volatile unsigned char*[depth_length]();
	nConfigs = new volatile uint64_t[maxDepth+1]();
}

/**
 *  Destructur: deallocate memory.
 */
DFSDepthMap::~DFSDepthMap()
{
	for (uint64_t i=0; i<depth_length; i++) {
		delete[] depth[i];
	}
	delete[] depth;
}

/**
 * Checks whether there is an entry for 'conf' with a depth <= 'newDepth'.
 * If so, return 'false', else set the depth of 'conf' in the mapping to
 * 'newDepth' and return 'true'.
 */
bool DFSDepthMap::lookup_and_set(uint64_t conf, uint64_t newDepth)
{
	uint64_t i1 = index1(conf);
	uint64_t i2 = index2(conf);

	// If necessary, allocate an array at the second level and initialize it with 0
	if (depth[i1] == NULL)
		depth[i1] = new unsigned char[BLOCKSIZE]();
	
	// If there is an entry with equal or smaller depth: we are done
	unsigned char old = depth[i1][i2];
	if ((old != 0) && (old <= (unsigned char)newDepth))
		return false;

	// Enter the depth and update the number of configurations for this depth
	depth[i1][i2] = (unsigned char)newDepth;
	if (old != 0)
		nConfigs[old]--;
	nConfigs[newDepth]++;
		
	return true;
}

/**
 * Returns information about RAM and hard disk usage and the number of
 * examined configurations for all depths < 'maxDepth'.
 */
void DFSDepthMap::statistics(uint64_t maxDepth)
{
	for (uint64_t i=1; i<maxDepth; i++)
		std::cerr << "depth " << i << ": " << nConfigs[i] << std::endl;
			 
	uint64_t size = depth_length/1024*sizeof(uint64_t);
	for (uint64_t i=0; i<depth_length; i++) {
		if (depth[i] != NULL)
			size += BLOCKSIZE/1024;
	}
	std::cout << "Used " << size << " KBytes for arrays" << std::endl;
}

