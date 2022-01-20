#include <stdlib.h>

#include "converter.h"


uint64_t  Converter::maxN;             // Number of fields
uint64_t  Converter::maxK;             // Number of boxes
uint64_t ** Converter::cacheNoverK;   // cacheNoverK[n][k] contains (n+1) over (k+1)
uint64_t *** Converter::cacheConfNo;  // cacheConfNo[n][k][s] contains the number
                                           // of the first configuration where the first
                                           // box is on field 's'.

// Initialize the array cacheNoverK.
void Converter::initNoverK()
{
	// (n k) = n*(n-1)*...*(n-k+1) / k*(k-1)*...*1
	// (n k) = (n-1 k) * n / (n-k)
	// (n k) = (n k-1) * (n-k+1) / k
	for (uint64_t n=1; n<=maxN; n++) {
		cacheNoverK[n-1][0] = n;
		for (uint64_t k=2; k<=maxK; k++) {
			cacheNoverK[n-1][k-1] = cacheNoverK[n-1][k-2] * (n-k+1) / k;
		}
	}
}

// Returns the value of the binomial coefficient 'n over k' (n k).
uint64_t Converter::nOverK(uint64_t n, uint64_t k)
{
	return k == 0 ? 1 : cacheNoverK[n-1][k-1];
}

// Initialize the array cacheConfNo.
void Converter::initConfNo()
{
	for (uint64_t n=1; n<maxN; n++) {
		for (uint64_t k=0; k<=n && k<maxK; k++) {
			cacheConfNo[n][k][0] = 0;
			for (uint64_t i=1; i<=n-k; i++) {
				cacheConfNo[n][k][i] = cacheConfNo[n][k][i-1] + nOverK(n-i+1,k);
			}
		}
	}
}

// Return the number of the first configuration where the first
// box is on field 's'.
uint64_t Converter::confNo(uint64_t n, uint64_t k, uint64_t s)
{
	return cacheConfNo[n-1][k-1][s];
}

// Find the largest entry <= 'no' in the array 'ary[lo:hi-1]' and return its index.
uint64_t Converter::find(uint64_t ary[], uint64_t lo, uint64_t hi,
							 uint64_t no)
{
	while (lo < hi) {
		uint64_t m = (lo+hi+1)/2;
		if (ary[m] == no)
			return m;
		if (ary[m] < no) {
			lo = m;
		}
		else {
			hi = m-1;
		}
	}
	return lo;
}

// In a situation with 'n' remaining fields and 'k' remaining boxes for a configuration
// number 'no', search the position 's' of the first remaining box and the number of the
// first configuration, where this box is on field 's'.
uint64_t Converter::findPos(uint64_t n, uint64_t k, uint64_t no, uint64_t* val)
{
	uint64_t * ary = cacheConfNo[n-1][k-1];
	uint64_t pos = find(ary, 0, n-k, no);
	*val = ary[pos];
	return pos;
}

// =========================================================
	
/** Initialize the class. Arguments:
 *   n = number of fields
 *   k = number of boxes
 */
void Converter::init(uint64_t n, uint64_t k)
{
	maxN = n;
	maxK = k;
	
	cacheNoverK = new uint64_t*[n];
	for (uint64_t i=0; i<n; i++)
		cacheNoverK[i] = new uint64_t[k]();
	initNoverK();
	
	cacheConfNo = new uint64_t**[n];
	for (uint64_t i=0; i<n; i++) {
		cacheConfNo[i] = new uint64_t*[k];
		for (uint64_t j=0; j<k; j++)
			cacheConfNo[i][j] = new uint64_t[n]();
	}
	initConfNo();
}

/** Return the number of possible box configurations. */
uint64_t Converter::getNumConfigs()
{
	return nOverK(maxN, maxK);
}

/** Determine the configuration number from the box positions in 'boxpos'. */
uint64_t Converter::configToNo(uint64_t boxpos[])
{
	uint64_t no = 0;
	uint64_t startpos = 0;
	for (uint64_t i=0; i<maxK; i++) {
		no += confNo(maxN-startpos, maxK-i, boxpos[i]-startpos);
		startpos = boxpos[i] + 1;
	}
	return no;
}

/** Determine the box positions corresponding to the specified configuration number. */
void Converter::noToConfig(uint64_t no, uint64_t * boxpos)
{
	uint64_t startpos = 0;
	for (uint64_t i=0; i<maxK; i++) {
		uint64_t val;
		uint64_t pos = findPos(maxN-startpos, maxK-i, no, &val);
		boxpos[i] = startpos + pos;
		no -= val;
		startpos = boxpos[i] + 1;
	}
}
