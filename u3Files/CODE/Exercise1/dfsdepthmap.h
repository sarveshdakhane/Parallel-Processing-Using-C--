#include <cstdint>

/**
 * For depth first search, this class performs a mapping from a configuration number to the
 * lowest depth found so far for the corresponding configuration (i.e., the minimum number of
 * moves from the initial configuration to the given configuration, which has been found until
 * now).
 */
class DFSDepthMap
{
 private:
	// The map is implemented as a two-level array (i.e., an array of arrays), where the second
	// level is only allocated as required (analogous to two-level page tables in operating
	// systems).
	// The arrays of the second level have 2^16 entries each. An access to a[i] thus is realized
	// as a[i/2^16][i%2^16], where we first check, if a[i/2^16] != NULL. If not, we will allocate
	// the second-level array. For computing the first and second index, we use inline functions.
	// The compiler will copy their code directly to the place where they are used.
	
	static const uint64_t BLOCKBITS = 16;                 // 16 Bit, arrays with 65536 int's
	static const uint64_t BLOCKSIZE = (1<<BLOCKBITS);     // Block size for allocation: 2^16
	static const uint64_t BLOCKMASK = ((1<<BLOCKBITS)-1); // Bit mask where the last 16 bits
	                                                          // are set
	
	inline uint64_t index1(uint64_t i)  { return i >> BLOCKBITS; }
	inline uint64_t index2(uint64_t i)  { return i & BLOCKMASK; }

	// Mapping from configuration number to tree depth, using a two-level array
	volatile unsigned char * volatile * depth;
	
	// Number of entries in 'depth'
	uint64_t depth_length;

	// For correctness checking: number of configurations at each tree depth
	volatile uint64_t * nConfigs;

 public:
	/**
	 * Constructor: Creates a new mapping for configuration numbers between
	 * 0 and numConf-1 and a maximum depth of 'maxDepth'.
	 */
	DFSDepthMap(uint64_t numConf, uint64_t maxDepth);
	
	/**
	 *  Destructur: deallocate memory.
	 */
	~DFSDepthMap();

	/**
	 * Checks whether there is an entry for 'conf' with a depth <= 'newDepth'.
	 * If so, return 'false', else set the depth of 'conf' in the mapping to
	 * 'newDepth' and return 'true'.
	 */
	bool lookup_and_set(uint64_t conf, uint64_t newDepth);

	/**
	 * Returns information about RAM and hard disk usage and the number of
	 * examined configurations for all depths < 'maxDepth'.
	 */
	void statistics(uint64_t maxDepth);
};
