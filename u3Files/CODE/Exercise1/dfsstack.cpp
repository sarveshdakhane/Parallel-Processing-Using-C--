#include <string>
#include <iostream>

#include "dfsstack.h"

/**
 * This class implements a stack of configurations for depth first search. It keeps track of
 * the path from the initial configuration to the currently examined configuration.
 */


/**
 * Constructor: Creates a stack with maximum depth 'maxDepth'.
 */
DFSStack::DFSStack(uint64_t maxDepth)
{
	stack_length = maxDepth;
	stack = new uint64_t[stack_length];
	sp = 0;
}

/**
 * Copy constructor: Creates a copy of the given stack.
 */
DFSStack::DFSStack(DFSStack &from)
{
	stack_length = from.stack_length;
	stack = new uint64_t[stack_length];
	sp = from.sp;
	for (uint64_t i=0; i<sp; i++)
		stack[i] = from.stack[i];
}
	
/**
 * Destructur: deallocate memory.
 */
DFSStack::~DFSStack()
{
	delete[] stack;
}

/**
 * Pushes the given configuration number onto the stack.
 */
void DFSStack::push(uint64_t conf)
{
	stack[sp++] = conf;
}

/**
 * Pops the topmost configuration number from the stack.
 */
void DFSStack::pop()
{
	sp--;
}

/**
 * Return the current depth of the stack.
 */
uint64_t DFSStack::length()
{
	return sp;
}

/**
 * Returns the solution path as an array of configurations. In '*path_length'
 * the length of this path is returned. The result is allocated dynamically and
 * should be deallocated using delete[].
 */
uint64_t * DFSStack::getPath(uint64_t * path_length)
{
	uint64_t * path = new uint64_t[sp];
	for (uint64_t i=0; i<sp; i++)
		path[i] = stack[i];
	*path_length = sp;
	return path;
}
	
