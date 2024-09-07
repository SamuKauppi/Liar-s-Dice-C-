#include "Gamestate.h"

GameState::GameState(int player_count) : d_count(0) {

	// Create a human player
	players.push_back(new Player(0));
	d_count += players.back()->get_cup_size();
	_d_values.reserve(player_count);
	_d_values.push_back(0);

	// Create AI players
	for (int i = 1; i < player_count; i++) {
		players.push_back(new AI_Player(i));
		d_count += players[i]->get_cup_size();
		_d_values.push_back(0);
	}

	// Set the turn player
	turnplayer = players[0];
}

GameState::~GameState()
{
	for (Player* p : players)
	{
		delete p;
	}
}

void GameState::start_new_round(int winner_id, int loser_id)
{
	// Which player will strat the new round
	int start_player;

	// Check if there was a winner
	if (winner_id != -1 && loser_id != -1)
	{
		// Winner gets to remove on die
		players[winner_id]->remove_a_die();
		// Loser gets to start the next round
		start_player = loser_id;
		// Reset the total coun of dices
		d_count = 0;
		for (Player* p : players)
		{
			// Reroll player cup
			p->reroll_cup();
			if (p->is_ai)
			{
				// Reset ai players
				AI_Player* ap = static_cast<AI_Player*>(p);
				ap->reset_ai();
			}
			// Accumulate total count of dices
			d_count += p->get_cup_size();
		}
	}
	// If no winner then this is the first round and the starting player is chosen randomly
	else
	{
		//start_player = 1;
		start_player = rand() % 2;
	}

	// Get new turnplayer
	turnplayer = players[start_player];
	// Update the turnplayer index
	_current_player_i = start_player;
	// Reset last bid
	d_last_bid[0] = 0;
	d_last_bid[1] = 0;
	// Increse round counter
	_round_num++;
}

bool GameState::make_bid(int count, int dice, int playerID)
{
	// Validate dice value
	if (dice < 2 || dice > 6) {
		return false;
	}

	// Check count limits
	if (count > d_count || count < d_last_bid[0] || count <= 0) {
		return false;
	}

	if (count == d_last_bid[0]) {
		// A higher dice value was used this count already
		for (int i : _d_values)
		{
			if (dice <= i)
				return false;
		}
	}
	else {

		// The dice count was risen, reset variables
		for (int& d : _d_values)
			d = 0;
	}

	// Make a bid
	d_last_bid[0] = count;
	d_last_bid[1] = dice;

	// Add the used dice to _d_values
	_d_values[playerID] = dice;

	return true;
}

int GameState::call()
{
	// Get the last bid
	int count = d_last_bid[0];
	int dice = d_last_bid[1];

	// Calculate dice count
	int total_count = 0;
	for (Player* p : players)
	{
		total_count += p->contains_dice(dice);
	}

	return total_count;
}

void GameState::next_player()
{
	_current_player_i = (_current_player_i + 1) % (int)players.size();
	turnplayer = players[_current_player_i];
}

int GameState::get_previous_player_id()
{
	_current_player_i = _current_player_i <= 0 ? (int)players.size() - 1 : _current_player_i - 1;
	return _current_player_i;
}

bool GameState::is_game_over() const
{
	for (Player* p : players)
	{
		if (p->get_cup_size() < 1)
			return true;
	}
	return false;
}
