#include "Player.h"
#include <unordered_map>

Player::Player(int playerID)
	: playerID(playerID), _cup(std::make_unique<Cup>()) // Initialize _cup
{
}

void Player::reroll_cup() {
	if (_cup) {
		_cup->roll_dice(); // Assuming Cup has a reroll method
	}
}

int Player::contains_dice(int dice)
{
	if(_cup)
	{
		int count = 0;
		for (int d : _cup->dices)
		{
			if (d == 1 || d == dice)
				count++;
		}
		return count;
	}
	return 0;
}

int Player::get_cup_size()
{
	return _cup->cup_size;
}

void Player::remove_a_die()
{
	_cup->reduce_size();
}

void Player::print_dice()
{
	for (int d : _cup->dices)
		std::cout << d << " ";
}

std::vector<int> Player::get_dices() const
{
	return _cup->dices;
}

AI_Player::AI_Player(int player_num) : Player(player_num) {
	is_ai = true;
	get_most_common_die();
}

void AI_Player::evaluate_bid(int bid[2], int prev_bid[2], int dice_count, int turn_player)
{
	// Check if this will be the first move in a round
	bool is_first = prev_bid[0] == 0;

	// To figure out what are the odds for each bid, we use the following binomial formula:
	// (n x)*p^x*q^(n-x)
	// Where:
	// n = number of trials
	// x = number of succeses
	// p = probability of success (1/5) (because 1 is wild)
	// q = probability of failure (4/5) (because 1 is wild)
	// (n x) = n! / ((n - x)! * x!) (binomial coefficient aka n choose x)
	// ! = factorial function is taking a number and multiplying it with each integer below it

	// How many copies does the ai have of the last bid dice
	int has_prev_dices = _cup->how_many_of_x_dice(prev_bid[1]);

	// Number of successes for the previous bid
	int x = prev_bid[0] - has_prev_dices;

	// Initialize probability as 100% true
	float probability = 1.0f;

	// We can skip calculations if we know that the ai has more than enough of bid dice
	// or there was no previous bid
	if (x > 0 && !is_first)
	{
		// Number of trials
		int n = dice_count - get_cup_size();

		// The bid is know to be 100% false if the 
		// number of successes is higher than number of trials
		if (x > n)
		{
			return;
		}

		// Calculate probability
		calculate_probability(probability, n, x);

		// Check if the ai should call a liar and return if yes
		if (should_call_liar(probability, has_prev_dices))
		{
			std::cout << "\nCalling lie! Probability: " << probability;
			return;
		}
	}

	// Make a bid
	// Use the best dice (most copies or highest) within own dices (_dice_to_bid) 
	// but with a chance to lie
	// This gives rough estimate what the ai wants to pick
	int lie_chance = 0;
	if (is_first) {
		lie_chance = rand() % 2;  // 50% chance to lie on the first move
	}
	else {
		// Increased lie chance if AI is in a weaker position (fewer dice)
		lie_chance = (get_cup_size() < dice_count / 2) ? rand() % 3 : rand() % 4;  // More risky lies when AI has fewer dice
	}
	int target_dice = lie_chance == 0 ? rand() % 5 + 2 : _dice_to_bid;

	// Check if it's valid to copy the previous bid
	if (!is_first && lie_chance != 0)
		should_copy_bid(has_prev_dices, target_dice, prev_bid);

	// Select the number of dice AI wants to bid based on probability and previous bid
	int count;
	select_dice_count_for_bid(probability, target_dice, dice_count, prev_bid, count);

	// Calculate if making a new bid is too unrealistic
	if (!is_first) {
		float new_pob;
		calculate_own_probability(dice_count, target_dice, count, new_pob);

		// Allow riskier bids if AI has few dice left, adding a layer of risk-taking behavior
		if (new_pob < 0.4f) {
			if (get_cup_size() > dice_count / 2 || new_pob < 0.1f) {
				return;  // Only return if bid is extremely unrealistic or AI has many dice left
			}
		}
	}

	// Apply values to create the bid
	bid[0] = count;
	bid[1] = target_dice;


	//if (count > dice_count || 
	//	(count <= prev_bid[0] && target_dice <= prev_bid[1]) || 
	//	count <= 0) {
	//	std::cout << "";
	//}
}

void AI_Player::reset_ai()
{
	get_most_common_die();
}

void AI_Player::should_copy_bid(int has_prev_dices, int& target_dice, int  prev_bid[2]) const
{
	if (has_prev_dices > _dice_count_to_bid / 2 && target_dice != prev_bid[1])
	{
		// Count how many copies hypothetically could there be of 
		// the previous die
		int combined_d_count = has_prev_dices + prev_bid[0] - (rand() % 3);

		// If there could be more copies of the die
		// Chance for the ai to bid for it
		if (_dice_count_to_bid < combined_d_count)
		{
			target_dice = prev_bid[1];
		}
	}
}

void AI_Player::select_dice_count_for_bid(float probability, int target_dice, int d_count, int prev_bid[2], int& count) const
{
	if (probability < 1.0f)
	{
		// if probability is not sure 
		// increase the count for the bid only by 1 if needed
		count = target_dice <= prev_bid[1] ? prev_bid[0] + 1 : prev_bid[0];
	}
	else
	{
		// If the AI is confident, it can mean two things:
		// 1. This is the first bid (prev_bid is { 0, 0 })
		// 2. The AI knows the previous bid is 100% correct based on its own dice
		if (prev_bid[0] < 1)
		{
			// First bid scenario
			// Check how many of the target dice the AI has
			int has_dice = _cup->how_many_of_x_dice(target_dice);

			if (has_dice <= 0)
			{
				// If no dice found, make a random estimate for the bid
				count = rand() % (d_count / 2) + 1;
			}
			else
			{
				// If dice are found, bid that amount and sometimes increase it by 1
				count = has_dice + rand() % 2;
			}
		}
		else
		{
			// Not the first bid, base count on how many of the target dice the AI has
			// and sometimes increase it by 1
			count = _cup->how_many_of_x_dice(target_dice) + rand() % 2;
		}
	}
	// Ensure the count doesn't exceed the total dice count
	// or is lower or same as the previous bid
	count = std::max(prev_bid[0] + 1, std::min(count, d_count));
}

void AI_Player::calculate_own_probability(int dice_count, int target_dice, int count, float& probability)
{
	int new_n = dice_count - get_cup_size();
	int new_x = count - _cup->how_many_of_x_dice(target_dice);

	// Case 1: If AI already has enough of the target dice, probability is 1.0
	if (new_x <= 0)
	{
		probability = 1.0f;
		return;
	}

	// Case 2: If more dice are needed than exist in the unknown pool, the bid is impossible
	if (new_x > new_n)
	{
		probability = 0.0f;
		return;
	}

	// Case 3: Calculate the probability if new_x dice are still needed from new_n unknown dice
	calculate_probability(probability, new_n, new_x);
}

float AI_Player::binomial_coefficient(int& n, int& x) {
	if (x > n - x)
		x = n - x;
	float coeff = 1.0f;
	for (float i = 0; i < x; ++i) {
		coeff *= (n - i) / (i + 1);
	}
	return coeff;
}

bool AI_Player::should_call_liar(float& probability, int& has_prev_dices)
{
	int should_call = rand() % 6;
	if (probability < 0.33f)
	{
		if (has_prev_dices == 3 && should_call == 0)
			return true;

		if (has_prev_dices == 4 && (should_call >= 0 && should_call < 2))
			return true;
		
		if (has_prev_dices > 4 && should_call < 3)
			return true;
	}

	if (probability < 0.2f)
	{
		return true;
	}

	return false;
}

void AI_Player::get_most_common_die()
{
	int n = (int)_cup->dices.size();
	std::unordered_map<int, int> m;

	int highest = 2;
	int highest_count = 0;
	int count_1s = 0;
	for (int i = 0; i < n; i++) {

		if (_cup->dices[i] != 1)
		{
			m[_cup->dices[i]]++;
			if (_cup->dices[i] > highest)
			{
				highest = _cup->dices[i];
				highest_count = m[_cup->dices[i]];
			}
		}
		else
		{
			count_1s++;
		}
	}
	highest_count += count_1s;

	n = n / 2;
	for (auto& x : m) {
		if (x.second >= n) {
			_dice_count_to_bid = x.second + count_1s;
			_dice_to_bid = x.first;
			return;
		}
	}

	_dice_count_to_bid = highest_count;
	_dice_to_bid = highest;
}

void AI_Player::calculate_probability(float& probability, int n, int x)
{
	// Calculate the chance of success and failure ( 2 / 6 because 2 of the 6 faces of a die can be correct)
	float p = 2.0f / 6.0f;
	float q = 1.0f - p;

	// Compute the binomial coefficient (n choose x)
	float coeff = binomial_coefficient(n, x);

	// Compute p^x and q^(n-x)
	float p_pow_x = std::pow(p, (float)x);
	float q_pow_n_minus_x = std::pow(q, (float)(n - x));

	// Combine to get the probability
	probability = coeff * p_pow_x * q_pow_n_minus_x;

	std::cout << "\nBid probability: " << probability;
}
