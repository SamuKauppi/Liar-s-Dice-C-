#pragma once
#include "Player.h"
#include <vector>
#include <memory>

/// <summary>
/// Game state handles the entire game
/// </summary>
/// <param name="player_count">How many players can be created (I originally planned there to be multiple)</param>
class GameState
{
public:
	// Players
	std::vector<std::shared_ptr<Player>> players;
	// Turnplayer
	std::shared_ptr<Player> turnplayer;
	// Number of dices total between all players
	int d_count = 0;
	// Last bid (x copies of x dice)
	int d_last_bid[2] = { 0, 0 };

	/// <summary>
	/// Creates game state with given players players
	/// </summary>
	/// <param name="player_count"></param>
	GameState(int player_count);
	~GameState() = default;

	/// <summary>
	/// Start a new round of liar's dice. Removes a die from the winner if there was one
	/// </summary>
	/// <param name="winner_id">Who was the winner of last round (-1 = no winner)</param>
	/// <param name="loser_id">Who was the loser of the last round (-1 = no loser)</param>
	void start_new_round(int winner_id, int loser_id);

	/// <summary>
	/// Evaluates if the bid is legal and returns if it's legal
	/// </summary>
	/// <param name="count">number of dices bid</param>
	/// <param name="dice">dice value bid</param>
	/// <param name="playerID">player making the bid</param>
	/// <returns>if the bid is legal</returns>
	bool make_bid(int count, int dice, int playerID);


	/// <summary>
	/// Calculates the how many copies of last bid dice there was
	/// </summary>
	/// <returns>number of dices found</returns>
	int call();

	/// <summary>
	/// Updates the turnplayer
	/// </summary>
	void next_player();

	/// <summary>
	/// Get the index of the previous player
	/// </summary>
	/// <returns>index of the previous player</returns>
	int get_previous_player_id();

	/// <summary>
	/// Check if game is over
	/// </summary>
	/// <returns>if game is over</returns>
	bool is_game_over() const;

private:


	// Current player index
	int _current_player_i = 0;
	// How many rounds have been played
	int _round_num = 0;
	// _d_values prevents players of making the same exact bid
	std::vector<int> _d_values;
};

