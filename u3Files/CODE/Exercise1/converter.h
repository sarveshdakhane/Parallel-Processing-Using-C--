#include <cstdint>

/**
 * This class (with only static attributes and methods) converts a configuration of boxes,
 * i.e., an array containing the positions of the boxes, into an integer (the configuration
 * number), and vice versa.
 */
class Converter
{
 public:
	/** Initialize the class. Arguments:
	 *   n = number of fields
	 *   k = number of boxes
	 */
	static void init(uint64_t n, uint64_t k);
	
	/** Return the number of possible box configurations. */
	static uint64_t getNumConfigs();
	
	/** Determine the configuration number from the box positions in 'boxpos'. */
	static uint64_t configToNo(uint64_t boxpos[]);
	
	/** Determine the box positions corresponding to the specified configuration number. */
	static void noToConfig(uint64_t no, uint64_t * bospos);
	
 private:
	static uint64_t maxN;              // Number of fields
	static uint64_t maxK;              // Number of boxes
	static uint64_t ** cacheNoverK;   // cacheNoverK[n][k] contains (n+1) over (k+1)
	static uint64_t *** cacheConfNo;  // cacheConfNo[n][k][s] contains the number
	                                       // of the first configuration where the first
                                           // box is on field 's'.
	
	// Initialize the array cacheNoverK.
	static void initNoverK();
	
	// Returns the value of the binomial coefficient 'n over k' (n k).
	static uint64_t nOverK(uint64_t n, uint64_t k);
	
	// Initialize the array cacheConfNo.
	static void initConfNo();
	
	// Return the number of the first configuration where the first
	// box is on field 's'.
	static uint64_t confNo(uint64_t n, uint64_t k, uint64_t s);
	
	// Find the largest entry <= 'no' in the array 'ary[lo:hi-1]' and return its index.
	static uint64_t find(uint64_t ary[], uint64_t lo, uint64_t hi,
							 uint64_t no);
	
	// In a situation with 'n' remaining fields and 'k' remaining boxes for a configuration
	// number 'no', search the position 's' of the first remaining box and the number of the
	// first configuration, where this box is on field 's'.
	static uint64_t findPos(uint64_t n, uint64_t k, uint64_t no,
								uint64_t* val);
};
