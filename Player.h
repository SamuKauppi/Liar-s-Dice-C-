#pragma once
#include <random>
#include <iostream>

#include "Cup.h"

class Player
{
public:
	// Is this player an ai
	bool is_ai = false;
	// Id of the player
	int playerID = 0;

	/// <summary>
	/// Constructor for a player
	/// </summary>
	/// <param name="player_num"></param>
	Player(int playerID);

	/// <summary>
	/// Rerolls dices in cup
	/// </summary>
	void reroll_cup();

	/// <summary>
	/// Returns the number of dices that this player has
	/// Counts 1's as the target dice
	/// </summary>
	/// <param name="dice">the dice looked for</param>
	/// <returns>Count of dices that the cup has</returns>
	int contains_dice(int dice);

	/// <summary>
	/// Get the number of dices in cup
	/// </summary>
	/// <returns>number of dices in cup</returns>
	int get_cup_size();

	/// <summary>
	/// Removes a die from the cup
	/// </summary>
	void remove_a_die();

	/// <summary>
	/// Prints the die that are in the cup
	/// </summary>
	void print_dice();

	std::vector<int> get_dices() const;

	virtual ~Player();

protected:
	// Contains die and methods to interact with the die
	Cup* _cup;
};

class AI_Player : public Player
{
public:
	using Player::Player;
	/// <summary>
	/// Creates player but sets is_ai = true
	/// </summary>
	/// <param name="player_num"></param>
	AI_Player(int player_num);;

	/// <summary>
	/// Evaluates the last bid and makes a new bid or returns false to call a liar
	/// </summary>
	/// <param name="bid">new bid</param>
	/// <param name="prev_bid">previous bid</param>
	/// <param name="dice_count">how many dices are left</param>
	/// <returns>If ai will make a bid</returns>
	void evaluate_bid(int bid[2], int prev_bid[2], int dice_count, int turn_player);

	/// <summary>
	/// Resets ai components
	/// </summary>
	void reset_ai();

private:
	// Dice that the ai likes to bid this round
	int _dice_to_bid = 0;
	// Count of the die that the ai likes to bid this round
	int _dice_count_to_bid = 0;

	/// <summary>
	/// This function decides if the ai should copy the bid made by the previous player
	/// </summary>
	/// <param name="has_prev_dices">How many dices the ai has of the previous bid</param>
	/// <param name="target_dice">Dice that ai is thinking of picking</param>
	/// <param name="prev_bid">Previous bid containing previous count and dice</param>
	void should_copy_bid(int has_prev_dices, int& target_dice, int  prev_bid[2]) const;

	/// <summary>
	/// Handles logic of selecting how many die should the ai pick for the new bid
	/// </summary>
	/// <param name="probability">probability of how true the previous bid is</param>
	/// <param name="target_dice">The die that the ai wants to use</param>
	/// <param name="d_count">Total amount of die in this round</param>
	/// <param name="prev_bid">Previous bid containing previous count and dice</param>
	/// <param name="count">Reference where count will be stored</param>
	void select_dice_count_for_bid(float probability, int target_dice, int d_count, int  prev_bid[2], int& count) const;

	/// <summary>
	/// Counts the number of ways to form an 
	/// unordered collection of k items chosen 
	/// from a collection of n distinct items
	/// </summary>
	/// <param name="n">Number of trials</param>
	/// <param name="x">Number of successes</param>
	/// <returns>Binomial coefficient</returns>
	float binomial_coefficient(int& n, int& x);

	/// <summary>
	/// Evaluates probability and handles logic deciding if the ai should call a liar
	/// </summary>
	/// <param name="probability"></param>
	/// <param name="has_prev_dices"></param>
	/// <returns></returns>
	bool should_call_liar(float& probability, int& has_prev_dices);

	/// <summary>
	/// Uses hash-map to find the marjority element
	/// If no clear majority is found, use the highest value in _cup
	/// </summary>
	void get_most_common_die();

	/// <summary>
	/// Calculates the probability of their own bid before doing it
	/// If the new probability is too low, ai decides call liar rather than making a
	/// too unbelivable bid
	/// </summary>
	/// <param name="dice_count">Total number of die in this round</param>
	/// <param name="target_dice">Die that the ai wants to bid for</param>
	/// <param name="count">Count that the ai wants to bid for</param>
	/// <param name="probability">Reference where the probability will be stored</param>
	void calculate_own_probability(int dice_count, int target_dice, int count, float& probability);

	/// <summary>
	/// Calculates the probability of truth
	/// </summary>
	/// <param name="probability"></param>
	/// <param name="x"></param>
	/// <param name="n"></param>
	void calculate_probability(float& probability, int x, int n);
};


