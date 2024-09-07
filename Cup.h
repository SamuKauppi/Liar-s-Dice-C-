#pragma once
#include <random>
#include <vector>

class Cup
{
public:
	// How many die can the cup contain
	int cup_size = 5;
	// Dices in cup
	std::vector<int> dices;

	Cup();
	/// <summary>
	/// Rolls all the dices in the cup
	/// </summary>
	void roll_dice();
	/// <summary>
	/// Reduced the number of dices in cup by one
	/// </summary>
	void reduce_size();
	/// <summary>
	/// Checks how many copies of given dice does teh cup have
	/// </summary>
	/// <param name="dice"></param>
	/// <returns></returns>
	int how_many_of_x_dice(int dice);
};