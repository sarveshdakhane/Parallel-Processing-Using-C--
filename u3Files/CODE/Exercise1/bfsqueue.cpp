#include <stdlib.h>

#include <string>
#include <iostream>
#include <cstdio>
#include <fstream>
#include <atomic>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#include <Windows.h>
#endif

#include "bfsqueue.h"



using namespace std;

/**
 * Data structure for supporting the breadth first search. The data structure primarily implements
 * a queue for configurations, connected with a set (bit set) storing the configurations that have
 * already been visited.
 */


/**
 * Constructor: Create a queue/bit set for configuration numbers between
 * 0 and numConf-1.
 */
BFSQueue::BFSQueue(uint64_t numConf)
	: file("sokoban.tmp", ios::out|ios::in|ios::trunc|ios::binary) // open a temporary file
{
	if (!file.is_open()) {
		cerr << "Cannot open tmp file 'sokoban.tmp'\n";
		exit(1);
	}
	// Delete the file. However, it stays accessible until it is closed.
	// When using the Windows OS, you may need to delete this statement.
	std::remove("sokoban.tmp");

	// Allocate arrays and initialize them with NULL. This initialization is caused by the
	// empty pair of parentheses () at the end of the 'new' operator.
	queue_length = qIndex1(numConf-1) + 1;
	queue[0] = new Entry *[queue_length]();
	queue[1] = new Entry *[queue_length]();
	bitset_length = bsIndex1(numConf-1) + 1;
	bitset = new volatile uint64_t*[bitset_length]();
	wrPos = 0;
	rdLength = 0;
	depth = 0;
	file_length = 0;
}

/**
 * Destructur: deallocate memory.
 */
BFSQueue::~BFSQueue()
{
	for (uint64_t i=0; i<queue_length; i++) {
		delete[] queue[0][i];
		delete[] queue[1][i];
	}
	for (uint64_t i=0; i<bitset_length; i++) {
		delete[] bitset[i];
	}
	delete[] queue[0];
	delete[] queue[1];
	delete[] bitset;
	file.close();
}

/**
 * Increase the tree depth by one. The previous write queue becomes the read queue for the
 * new tree depth. The old read queue is stored in a temporary file to determine the solution
 * path at the end.
 */
void BFSQueue::pushDepth()
{
	uint64_t wr = depth % 2;
	uint64_t n1 = qIndex1(wrPos);
	uint64_t n2 = qIndex2(wrPos);

	// Export the old read queue to a file.
	for (uint64_t i=0; i<n1; i++)
		file.write((char *)queue[wr][i], BLOCKSIZE * sizeof(Entry));
	if (n2 > 0)
		file.write((char *)queue[wr][n1], n2 * sizeof(Entry));
	file_length += ((uint64_t)BLOCKSIZE) * n1 + n2;
	
	depth++;
	rdLength = wrPos;
	wrPos = 0;
}

/**
 * Checks if the given configuration is already contained in the bit set. If not, the
 * configuration is entered in the bit set and the configuration, the index of the predecessor
 * configuration and the number of the moved box are added to the write queue.
 */
bool BFSQueue::lookup_and_add(uint64_t conf, uint64_t predIndex, uint64_t box)
{
	uint64_t bitmask = (uint64_t)1 << bsBitPos(conf);
	uint64_t i1 = bsIndex1(conf);
	uint64_t i2 = bsIndex2(conf);
	
	// If necessary, allocate an array at the second level and initialize it with 0
	if (bitset[i1] == NULL)
		bitset[i1] = new uint64_t[BLOCKSIZE]();

	// If the configuration is in the bit set: we are done
	if ((bitset[i1][i2] & bitmask) != 0)
		return false;

	// add the configuration to the bit set
	bitset[i1][i2] |= bitmask;

	// Append the configuration, the index of the predecessor configuration and the
	// number of the moved box at the end of the write queue.
	
	uint64_t wr = depth % 2;
	uint64_t n1 = qIndex1(wrPos);
	uint64_t n2 = qIndex2(wrPos);

	// If necessary, allocate an array at the second level and initialize it
	if (queue[wr][n1] == NULL)
		queue[wr][n1] = new Entry[BLOCKSIZE]();

	// Write the new entry at position wrPos into the write queue
	queue[wr][n1][n2].set(conf, wrPos + (rdLength - predIndex), box);

	// Increment the write position
	wrPos++;
	
	return true;
}

/**
 * Return the length of the read queue.
 */
uint64_t BFSQueue::length()
{
	return rdLength;
}

/**
 * Return the i-th entry in the read queue (configuration as return value;
 * moved box in *box).
 */
uint64_t BFSQueue::get(uint64_t i, uint64_t * box)
{
	uint64_t rd = (depth-1) % 2;
	Entry *e = &queue[rd][qIndex1(i)][qIndex2(i)];
	if (box != NULL)
		*box = e->box;
	return e->config;
}

/**
 * Return the solution path as an array of configurations. The parameter conf is the
 * solution configuration, predIndex the index of the predecessor configuration. In *path_length
 * the length of the path is returned. The result is allocated dynamically and should be 
 * deallocated using delete[].
 */
uint64_t * BFSQueue::getPath(uint64_t conf, uint64_t predIndex,
								  uint64_t * path_length)
{
	uint64_t * path = new uint64_t[depth+1];
	path[depth] = conf;
	uint64_t pos = rdLength - predIndex - 1;
	int64_t abspos = file_length * sizeof(Entry); // XX
	
	// Iterate the path in reversed order
	for (int64_t k = depth-1; k>=0; k--) {
		Entry e;
		// Search the entry for the predecessor configuration in the file and load it
		file.seekg(-((int)pos+1)*sizeof(Entry), ios::cur);
		file.read((char *)&e, sizeof(Entry));
		path[k] = e.config;
		pos = e.pred;
	}
	*path_length = depth+1;
	return path;
}

/**
 * Returns information about RAM and hard disk usage.
 */
void BFSQueue::statistics()
{
	uint64_t size = 2*queue_length*sizeof(Entry *)/1024;
	for (uint64_t i=0; i<queue_length; i++) {
		if (queue[0][i] != NULL)
			size += BLOCKSIZE/1024*sizeof(Entry);
		if (queue[1][i] != NULL)
			size += BLOCKSIZE/1024*sizeof(Entry);
	}
	cout << "Used " << size << " KBytes for arrays\n";
	
	size = bitset_length*sizeof(uint64_t *)/1024;
	for (uint64_t i=0; i<bitset_length; i++) {
		if (bitset[i] != NULL)
			size += BLOCKSIZE/1024*sizeof(uint64_t);
	}
	cout << "Used " << size << " KBytes for bit set\n";
	
	size = file_length*sizeof(Entry)/1024;
	cout << "Used " << size << " KBytes for temp file\n";
}

