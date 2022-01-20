#include <iostream>
#include <cstdint>

/**
 * Data structure for supporting the breadth first search. The data structure primarily implements
 * a queue for configurations, connected with a set (bit set) storing the configurations that have
 * already been visited.
 */
class BFSQueue
{
 private:
	/*
	 * Class for an entry in the queue. Each entry contains:
	 * - the number of the configuration
	 * - the relative position of the predecessor configuration in the queue
	 *   (this is needed to determine the solution path when a solution has been found)
	 * - the number of the box that was moved to reach this configuration
	 *   (this is used to preferrably move the same box with the next move)
	 */
	class Entry {
	public:
		uint64_t config;
		uint64_t pred;
		uint64_t box;
		
		inline void set(uint64_t aconfig, uint64_t apred, uint64_t abox) volatile {
			config = aconfig;
			pred = apred;
			box = abox;
		}
	};

	// Split queue. When processing tree depth X
	// - the configurations of depth X-1 which are to be examined will be read from queue[(X-1)%2], and
	// - the successor configurations of depth X will be written into queue[X%2].
	Entry * volatile * queue[2];

	// Number of entries in queue[0] and queue[1], respectively
	uint64_t queue_length;

	// Position of the next free entry in the write queue
	volatile uint64_t wrPos;

	// Length of the read queue
	uint64_t rdLength;

	// Current tree depth
	uint64_t depth;

	// This bit set stores all configurations that already have been examined, in order to
	// avoid (1) cycles and (2) multiple examinations of the same configuration.
	volatile uint64_t * volatile * bitset;

	// Maximum number of entries in the bit set
	uint64_t bitset_length;

	// Swap file. In order to save main memory, only the information for the current tree depth
	// X and the tree depth X-1 are kept in main memory. The entries of the queues for smaller
	// tree depths are exported to a temporary file. When we found a solution, they are needed
	// again to determine the path which lead to the solution.
	std::fstream         file;

	// Number of entries in the swap file
	uint64_t   file_length;


	// The queues and bit sets are dynamically allocated block by block, and only when
	// necessary. For this purpose a two-level array (i.e., an array of arrays) is used instead
	// of a simple array. The second level is only allocated as required (analogous to
	// two-level page tables in operating systems).
	// The arrays of the second level have 2^16 entries each. An access to a[i] thus is realized
	// as a[i/2^16][i%2^16], where we first check, if a[i/2^16] != NULL. If not, we will allocate
	// the second-level array. For computing the first and second index, we use inline functions.
	// The compiler will copy their code directly to the place where they are used.
	
	static const uint64_t BLOCKBITS = 16;                 // 16 Bit, arrays with 65536 int's
	static const uint64_t BLOCKSIZE = (1<<BLOCKBITS);     // Block size for allocation: 2^16
	static const uint64_t BLOCKMASK = ((1<<BLOCKBITS)-1); // Bit mask where the last 16 bits
	                                                          // are set
	
	inline uint64_t qIndex1(uint64_t i)  { return i >> BLOCKBITS; }
	inline uint64_t qIndex2(uint64_t i)  { return i & BLOCKMASK; }

	// The bit set is implemented as a two-level array of 32-bit values. For a given configuration
	// number, the function bsBitPos returns the bit position within an array element.
	
	static const uint64_t WORDBITS = 5;                 // 5 Bit = 0..31, bits in one int
	static const uint64_t WORDMASK = ((1<<WORDBITS)-1); // Bit mask where the last 5 Bits
	                                                        // are set
	
	inline uint64_t bsIndex1(uint64_t i) { return i >> (WORDBITS + BLOCKBITS); }
	inline uint64_t bsIndex2(uint64_t i) { return (i >> WORDBITS) & BLOCKMASK; }
	inline uint64_t bsBitPos(uint64_t i) { return i & WORDMASK; }

 public:
	/**
	 * Constructor: Create a queue/bit set for configuration numbers between
	 * 0 and numConf-1.
	 */
	BFSQueue(uint64_t numConf);

	/**
	 * Destructur: deallocate memory.
	 */
	~BFSQueue();

	/**
	 * Increase the tree depth by one. The previous write queue becomes the read queue for the
	 * new tree depth. The old read queue is stored in a temporary file to determine the solution
	 * path at the end.
	 */
	void pushDepth();

	/**
	 * Checks if the given configuration is already contained in the bit set. If not, the
	 * configuration is entered in the bit set and the configuration, the index of the predecessor
	 * configuration and the number of the moved box are added to the write queue.
	 */
	bool lookup_and_add(uint64_t conf, uint64_t predIndex, uint64_t box);

	/**
	 * Return the length of the read queue.
	 */
	uint64_t length();
	
	/**
	 * Return the i-th entry in the read queue (configuration as return value;
	 * moved box in *box).
	 */
	uint64_t get(uint64_t i, uint64_t * box);

	/**
	 * Return the solution path as an array of configurations. The parameter conf is the
	 * solution configuration, predIndex the index of the predecessor configuration. In *path_length
	 * the length of the path is returned. The result is allocated dynamically and should be 
	 * deallocated using delete[].
	 */
	uint64_t * getPath(uint64_t conf, uint64_t predIndex, uint64_t * path_length);

	/**
	 * Returns information about RAM and hard disk usage.
	 */
	void statistics();
};

