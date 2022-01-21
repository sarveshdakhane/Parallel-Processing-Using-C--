#include <stdlib.h>

#include <string>
#include <iostream>
#include <fstream>
#include <chrono>
#include <cstdint>
#include <thread>

#include "converter.h"
#include "config.h"
#include "bfsqueue.h"
#include "dfsstack.h"
#include "dfsdepthmap.h"

/**
 * This program solves the game 'Sokoban'. The goal of the game is to push boxes
 * with a player onto target fields (storage locations) in a cartesian grid.
 * In each step only one box can be pushed.
 */

/**
  * Check whether the configuration with number 'succNo' is a successor of
  * the configuration 'conf', and which box must be moved in order to reach
  * this successor configuration.
  */
static uint64_t checkSuccessor(Config *conf, uint64_t succNo)
{
	uint64_t nBoxes = Config::numBoxes(); // Number of boxes
	for (uint64_t box = 0; box < nBoxes; box++)
	{
		for (uint64_t dir = 0; dir < 4; dir++)
		{
			if (conf->getNextConfig(box, dir, NULL) == succNo)
				return box;
		}
	}
	std::cerr << "FATAL ERROR: Invalid solution path!" << std::endl;
	std::cerr << "             This configuration's successor is illegal!" << std::endl;
	return -1;
}

/**
 * Print the path for a discovered solution, i.e., the sequence of configurations
 * that leads to the solution.
 */
static void printPath(uint64_t path[], uint64_t length)
{
	if (length > 0)
	{
		std::cerr << std::endl;
		std::cerr << "Found solution with " << (length - 1) << " pushes" << std::endl;
		std::cout << std::endl;
		uint64_t box = -1;
		for (uint64_t i = 0; i < length; i++)
		{
			Config conf(path[i]);
			std::cout << "Push " << i << ":" << std::endl;
			conf.print();
			box = (i < length - 1) ? checkSuccessor(&conf, path[i + 1]) : -1;
		}
	}
	else
	{
		std::cout << std::endl;
		std::cout << "Found NO solution" << std::endl;
		std::cout << std::endl;
	}
}

/**
 * Execute a breadth first search from the given starting configuration, in order to find a
 * solution. The search tree is examined layer by layer from top to bottom. For configurations
 * of depth 'depth-1', the possible successor configurations of depth 'depth' are determined
 * and entered into the queue for depth 'depth', if they have not already been examined
 * previously.
 */
static void doBreadthFirstSearch(Config *conf)
{
	// Create the queue for the configurations to be examined.
	// At the beginning, the queue just contains the starting configuration.
	BFSQueue *queue = new BFSQueue(Config::getNumConfigs());
	queue->lookup_and_add(conf->getConfig(), -1, 0);
	queue->pushDepth();

	uint64_t nBoxes = Config::numBoxes(); // Number of boxes
	uint64_t depth = 1;					  // Tree depth
	uint64_t length = queue->length();	  // Number of configurations at depth 'depth-1'
	uint64_t lastBox;					  // Box that was moved last

	// Pass through all layers of the tree with increasing depth until there are no
	// configurations with this depth any more.
	bool Flag_SoluFound = false;
	while (length > 0)
	{
		// Print the progress
		std::cerr << "depth " << depth << ": " << length << std::endl
				  << std::flush;
// Consider all configurations of depth 'depth-1'.

	uint64_t newBox;
#pragma omp parallel for private(lastBox,newBox)
		for (uint64_t i = 0; i < length; i++)
		{
			// Read the configuration from the queue
			Config newConf(queue->get(i, &lastBox));
			// Consider all boxes, starting with the box that was moved last
			for (uint64_t b = 0; b < nBoxes; b++)
			{
				uint64_t box = (b + lastBox) % nBoxes;
				// Consider all directions of movement
				for (uint64_t dir = 0; dir < 4 && !Flag_SoluFound; dir++)
				{
					// Determine the configuration that results from moving box
					// 'box' in direction 'dir'.
					uint64_t c = newConf.getNextConfig(box, dir, &newBox);
					// If the move is valid, check whether the resuling configuration has
					// been examined before. If not, add it to the queue
					if (c != Config::NONE)
					{
						bool CheckconfiAdded = false;
                        #pragma omp critical
						CheckconfiAdded = queue->lookup_and_add(c, i, newBox);
						if (CheckconfiAdded)
						{
							// If we found a solution: print it and terminate the search
							if (Config::isSolutionConf(c))
							{
								Flag_SoluFound = true;
								uint64_t len;
								uint64_t *path = queue->getPath(c, i, &len);
								printPath(path, len);
								delete[] path;
								queue->statistics();
								//return;
							}
						}
					}
				}
			}
		}

		// Advance the queue for the next tree depth
		depth++;
		queue->pushDepth();
		// Number of configurations in the next tree depth
		length = queue->length();
	}

	// If the loop exits normally, there is no solution
	std::cout << "No solution found!" << std::endl;
	queue->statistics();
}

/**
 * Global variable for depth first search
 * - best solution path found so far
 * - length of this path (= depth limit for the search)
 */
static uint64_t *path = NULL;
static uint64_t path_len = 0;

/**
 * Recursive depth first search. If 'conf' is a solution configuration, the
 * recursion is terminated. Otherwise, the procedure is called recursively
 * for all possible successor configurations.
 * The parameter 'lastBox' contains the number of the box that was moved last,
 * so we can preferrably move the same box with the next move. In
 * 'stack' we store the sequence of moves executed so far; 'map' is a mapping
 * that stores the lowest tree depth found so far for each configuration.
 * It is used to avoid repeated examinations of the same configuration when
 * this is not necessary.
 */
static void recDepthFirstSearch(Config *conf, uint64_t lastBox,
								DFSStack *stack, DFSDepthMap *map)
{
	// Get the configuration number and push it on the stack.
	uint64_t c = conf->getConfig();
#pragma omp critical
	stack->push(c);
	uint64_t depth = stack->length();

	// If we found a solution: remember the solution path (sequence of moves).
	if (Config::isSolutionConf(c))
	{
		delete[] path;
		path = stack->getPath(&path_len);
		std::cout << "Found solution: " << (path_len - 1) << " pushes" << std::endl;
		stack->pop();
		return;
	}

	// If the depth is larger than the length of the best solution path found so far:
	// Terminate the examination of this branch (it cannot contain a better solution
	// any more).
	if (depth >= path_len)
	{
		stack->pop();
		return;
	}

	// Consider all boxes, starting with the box that was moved last
	uint64_t nBoxes = Config::numBoxes();
	for (uint64_t b = 0; b < nBoxes; b++)
	{
		uint64_t box = (b + lastBox) % nBoxes;
		// Consider all directions of movement
		for (uint64_t dir = 0; dir < 4; dir++)
		{
			uint64_t newBox;
			// Determine the configuration that results from moving box
			// 'box' in direction 'dir'.
			c = conf->getNextConfig(box, dir, &newBox);

			// If the move is valid, check whether the resuling configuration has
			// already been found at the same or a smaller depth. If not, store
			// the new depth for this configuration.
			if (c != Config::NONE)
			{
				// Recursively continue the search on a copy of the configuration
				bool Flag_ConfigAdded = false;
#pragma omp critical
				Flag_ConfigAdded = map->lookup_and_set(c, depth + 1);
				if (Flag_ConfigAdded)
				{
					DFSStack *NewStack = new DFSStack(*stack);
#pragma omp task shared(map)
					{
						
						Config next(c);
						recDepthFirstSearch(&next, box, stack, map);
						delete NewStack;
					}
				}
			}
		}
	}
	stack->pop();
}

/**
 * Wrapper procedure for recursive depth first search. The search starts at the
 * starting configuration 'conf' and continues up to the maximum depth 'maxDepth'.
 */
static void doDepthFirstSearch(Config *conf, uint64_t maxDepth)
{
	DFSStack stack(maxDepth);
	DFSDepthMap map(Config::getNumConfigs(), maxDepth);
	map.lookup_and_set(conf->getConfig(), 1);
	path_len = maxDepth;
	
	#pragma omp parallel
	#pragma omp single nowait
	recDepthFirstSearch(conf, 0, &stack, &map);
	map.statistics(path_len);
	printPath(path, path_len);
	delete[] path;
}

/**
 * Main program. Invocation:
 *    sokoban <level-file> [<max-depth>]
 * If 'max-depth' is give, a depth first search up to a maximum depth of 'max-depth'
 * is performed, otherwise a breadth first search.
 */
int main(int argc, char **argv)
{
	std::cout << "sizeof(uint64_t): " << sizeof(uint64_t) << std::endl;
	if ((argc < 2) || (argc > 3))
	{
		std::cerr << "Usage: sokoban <level-file> [<max-depth>]" << std::endl;
		exit(1);
	}

	// Initialize the configuration with the starting configuration (level) from the file
	Config *conf = Config::init(argv[1]);

	auto ta = std::chrono::high_resolution_clock::now();
	if (argc > 2)
	{
		// depth first search
		uint64_t maxDepth = atoi(argv[2]);
		doDepthFirstSearch(conf, maxDepth + 1);
	}
	else
	{
		// breadth first search
		doBreadthFirstSearch(conf);
	}
	auto te = std::chrono::high_resolution_clock::now();

	// Print the run time
	std::cout << std::endl;
	std::chrono::duration<float> time = te - ta;
	std::cout << "Total time (s): " << time / std::chrono::seconds(1) << std::endl;

	return 0;
}
