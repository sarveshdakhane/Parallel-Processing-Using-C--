#include <cstdint>
/**
 * This class implements a stack of configurations for depth first search. It keeps track of
 * the path from the initial configuration to the currently examined configuration.
 */
class DFSStack
{
 private:
	// Stack: array of configuration numbers
	uint64_t * stack;
	// Length of the 'stack' array
	uint64_t stack_length;
	// Stack pointer
	uint64_t sp;

 public:
	/**
	 * Constructor: Creates a stack with maximum depth 'maxDepth'.
	 */
	DFSStack(uint64_t maxDepth);

	/**
	 * Copy constructor: Creates a copy of the given stack.
	 */
	DFSStack(DFSStack &from);

	/**
	 * Destructur: deallocate memory.
	 */
	~DFSStack();

	/**
	 * Pushes the given configuration number onto the stack.
	 */
	void push(uint64_t conf);

	/**
	 * Pops the topmost configuration number from the stack.
	 */
	void pop();

	/**
	 * Return the current depth of the stack.
	 */
	uint64_t length();

	/**
	 * Returns the solution path as an array of configurations. In '*path_length'
	 * the length of this path is returned. The result is allocated dynamically and
	 * should be deallocated using delete[].
	 */
	uint64_t * getPath(uint64_t * path_length);
};
