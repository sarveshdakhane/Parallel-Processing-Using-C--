#include <iostream>
#include <stdlib.h>

#include "converter.h"
#include "config.h"

uint64_t Config::nBoxConfigs;
uint64_t Config::solutionConfNo;


// ==================================================================

static uint64_t log2(uint64_t num)
{
	unsigned res = 0;
	for (; num > 0; num >>= 1)
		res++;
	return res;
}

// ==================================================================

/**
 * Initialization: The file 'fname' contains a string representation of the Sokoban level,
 * i.e., the initial configuration. The return value is the start configuration.
 */
Config* Config::init(const char* fname)
{
	Playfield::init(fname);
	Converter::init(Playfield::nPos, Playfield::nBox);
	nBoxConfigs = Converter::getNumConfigs();
	solutionConfNo = Converter::configToNo(Playfield::goalPos);

	std::cerr << "#Configs: " << getNumConfigs() << " (2^" << log2(getNumConfigs()) << ") "
		<< "#BoxConfigs: " << nBoxConfigs << " (2^" << log2(nBoxConfigs) << ") "
		<< std::endl << std::endl;

	return new Config();
}

/**
 * Does the specified configuration number represent a solution, i.e., are all boxes on
 * a target?
 */
bool Config::isSolutionConf(uint64_t conf)
{
	return (conf % nBoxConfigs) == solutionConfNo;
}

/**
 * Returns the maximum amount of configuration numbers. Thus, the configuration numbers
 * all are in the range 0...getNumConfigs()-1.
 */
uint64_t Config::getNumConfigs()
{
	return (1 + 3 * Playfield::nBox) * nBoxConfigs;
}

/**
 * Return the number of boxes.
 */
uint64_t Config::numBoxes()
{
	return Playfield::nBox;
}

// ==================================================================

/**
 * Default constructor: initial configuration
 */
Config::Config()
{
	boxPos = new uint64_t[Playfield::nBox];
	comp = new unsigned short[Playfield::nFields];
	for (uint64_t i = 0; i < Playfield::nBox; i++)
		boxPos[i] = Playfield::initialBoxPos[i];
	initBoxesBitSet();
	setComponents(comp);
	configNo = Converter::configToNo(boxPos) + comp[Playfield::initialPlayerPos] * nBoxConfigs;
}

/**
 * Constructor: creates a configuration with the specified configuration number.
 */
Config::Config(uint64_t confNo)
{
	boxPos = new uint64_t[Playfield::nBox];
	comp = new unsigned short[Playfield::nFields];
	setConfig(confNo);
}

/**
 * Destructur: deallocate memory.
 */
Config::~Config()
{
	delete[] comp;
	delete[] boxPos;
}

/**
 * Returns the configuration number for this configuration.
 */
uint64_t Config::getConfig()
{
	return configNo;
}

/**
 * Updates this configuration to the one with the specified number.
 */
void Config::setConfig(uint64_t confNo)
{
	Converter::noToConfig(confNo % nBoxConfigs, boxPos);
	initBoxesBitSet();
	setComponents(comp);
	configNo = confNo;
}

/**
 * If a valid successor configuration can be reached from the current configuration by moving
 * the box 'box' into direction 'dir', the number of the new configuration is returned, else 'NONE'.
 * 'box' is the number of the box to be moved (0...numBoxes()-1)
 * 'dir' is the direction of movement (0...3)
 * *newBox returns the (new) number of the moved box.
 */
uint64_t Config::getNextConfig(uint64_t box, uint64_t dir, uint64_t* newBox)
{
	uint64_t pos = boxPos[box];
	uint64_t playerPos = Playfield::neighbor[dir ^ 2][pos];
	uint64_t newBoxPos = Playfield::neighbor[dir][pos];
	uint64_t result = NONE;

	if (isReachable(playerPos)
		&& Playfield::isValid(newBoxPos) && hasNoBox(newBoxPos)
		&& !Playfield::isDead(newBoxPos)) {
		box = moveBox(box, newBoxPos); // Execute the move
		// Check whether the box is on a target or can be removed again. If not, the move
		// leads to a dead-end and is not executed.
		if (Playfield::isGoal(newBoxPos) || canBeEmptied(newBoxPos, 0L)) {
			uint64_t confNo = Converter::configToNo(boxPos);
			unsigned short* lcomp = new unsigned short[Playfield::nFields];
			setComponents(lcomp);
			uint64_t playerComp = lcomp[pos];
			result = confNo + playerComp * nBoxConfigs;
			if (newBox != NULL)
				*newBox = box;
		}
		moveBox(box, pos); // Undo the move
	}
	return result;
}

/**
 * Can the player reach the field 'pos' of the playing field?
 */
bool Config::isReachable(uint64_t pos)
{
	uint64_t playerComp = configNo / nBoxConfigs;
	return Playfield::isValid(pos) && (comp[pos] == playerComp);
}

/**
 * Print the configuration 'graphically'.
 */
void Config::print()
{
	std::cout << "Config " << configNo << "(" << (configNo / nBoxConfigs) << ","
		<< (configNo % nBoxConfigs) << "/" << nBoxConfigs << ")" << std::endl;
	Playfield::print(this);
}

// ==================================================================

// Computes 'boxes' from 'boxPos'.
void Config::initBoxesBitSet()
{
	boxes = 0;
	for (uint64_t p = 0; p < Playfield::nBox; p++) {
		if (boxPos[p] >= 64) {
			std::cerr << "Boxes: pos >= 64!" << std::endl;
			exit(1);
		}
		boxes |= ((uint64_t)1 << boxPos[p]);
	}
}

// Move the 'box'-th box to the field with number 'newPos' and return the new
// number of the box (since the boxes are always sorted according to their
// position on the playing field).
uint64_t Config::moveBox(uint64_t box, uint64_t newPos)
{
	uint64_t oldPos = boxPos[box];
	uint64_t* newBoxPos = new uint64_t[Playfield::nBox];
	uint64_t j = 0;
	uint64_t newBox = -1;
	for (uint64_t i = 0; i < Playfield::nBox; i++) {
		if (j == box)
			j++;
		if ((j < Playfield::nBox) && ((boxPos[j] < newPos) || (newBox != -1))) {
			newBoxPos[i] = boxPos[j++];
		}
		else {
			newBoxPos[i] = newPos;
			newBox = i;
		}
	}
	for (uint64_t i = 0; i < Playfield::nBox; i++)
		boxPos[i] = newBoxPos[i];
	boxes &= ~((uint64_t)1 << oldPos);
	boxes |= ((uint64_t)1 << newPos);
	delete[] newBoxPos;
	return newBox;
}

// Compute the connected components. See attribute 'comp'.
void Config::setComponents(uint16_t comp[])
{
	uint64_t* queue = new uint64_t[Playfield::nFields];
	uint64_t in = 0;
	uint64_t out = 0;
	uint64_t cn = 0;
	const unsigned short none = -1;

	for (uint64_t i = 0; i < Playfield::nFields; i++)
		comp[i] = none;

	for (uint64_t i = 0; i < Playfield::nFields; i++) {
		if ((comp[i] == none) && hasNoBox(i)) {
			comp[i] = (uint16_t)cn;
			queue[in++] = i;
			while (out < in) {
				uint64_t pos = queue[out++];
				for (uint64_t dir = 0; dir < 4; dir++) {
					uint64_t n = Playfield::neighbor[dir][pos];
					if (Playfield::isValid(n) && (comp[n] == none) && hasNoBox(n)) {
						comp[n] = (uint16_t)cn;
						queue[in++] = n;
					}
				}
			}
			cn++;
		}
	}
	delete[] queue;
}

// Can position 'pos' of the playing field be emptied? The argument 'path' is a
// bit set to avoid cycles during the search. It is initialized with 0.
bool Config::canBeEmptied(uint64_t pos, uint64_t path)
{
	if (!Playfield::isValid(pos))
		return false;
	if (hasNoBox(pos))
		return true;
	if ((path & ((uint64_t)1 << pos)) != 0)
		return false;
	path |= ((uint64_t)1 << pos);
	return (canBeEmptied(Playfield::neighbor[0][pos], path)
		&& canBeEmptied(Playfield::neighbor[2][pos], path))
		|| (canBeEmptied(Playfield::neighbor[1][pos], path)
			&& canBeEmptied(Playfield::neighbor[3][pos], path));
}

