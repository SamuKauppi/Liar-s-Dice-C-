#include <iostream>
#include <string>
#include <random>
#include <time.h>
#include "Gamestate.h"

using std::cout;
using std::cin;
using std::string;

int main()
{
	srand(time(NULL));

	GameState state(2);
	Player* turn_player;
	AI_Player* ai_player;
	bool game_loop = true;
	bool round_loop = true;
	bool turn_loop = true;
	int winner_id = -1;
	int loser_id = -1;
	cout << "\nGame starts!\n";
	while (game_loop)
	{
		state.start_new_round(winner_id, loser_id);

		// Check if game has ended
		game_loop = !state.is_game_over();
		round_loop = game_loop;
		turn_loop = game_loop;

		while (round_loop)
		{
			turn_player = state.turnplayer;
			int playerID = turn_player->playerID + 1;
			cout << "\n\nPlayer " << playerID << ". It's your turn!";

			while (turn_loop)
			{
				cout << "\nChoose action (make a bid e.g. 3,3 or call (0,0)";
				// Make count and dice
				int count, dice;

				// Check if it's a player
				if (!turn_player->is_ai)
				{
					cout << "\nYour dices are: ";
					turn_player->print_dice();
					cout << "\n";
					cin >> count;
					cin >> dice;
				}
				// If not cast turn player as ai
				else
				{
					ai_player = static_cast<AI_Player*>(turn_player);
					int bid[2] = { 0, 0 };
					ai_player->evaluate_bid(bid, state.d_last_bid, state.d_count, turn_player->playerID);
					count = bid[0];
					dice = bid[1];
				}

				// count was 0, it means that the player wants to call
				if (count == 0 && state.d_last_bid[0] > 0)
				{
					// count the amount of correct dices
					int count = state.call();
					turn_loop = false;
					round_loop = false;

					// Print player dices
					cout << "\nPlayer " << playerID << " is calling liar!\n";
					for (Player* p : state.players)
					{
						p->print_dice();
						cout << "\n";
					}

					// Validate results
					if (count < state.d_last_bid[0])
					{
						cout << "Player " << playerID << " correctly called liar\n";
						winner_id = turn_player->playerID;
						loser_id = state.get_previous_player_id();
					}
					else
					{
						cout << "Player " << playerID << " incorreclty called liar\n";
						loser_id = turn_player->playerID;
						winner_id = state.get_previous_player_id();
					}
				}
				else if (state.make_bid(count, dice, turn_player->playerID))
				{
					// bid
					state.next_player();
					cout << "\nPlayer " << playerID << " made the bid: " << count << " x " << dice;
					break;
				}
				else
				{
					cout << "\nInvalid action of: " << count << " x " << dice;
				}
			}
		}
		cout << "\nGame starts!\n";
	}

	cout << "\n\nGame is over!";
	cout << "\nWinner is: Player " << state.players[winner_id]->playerID + 1 << ".\n";
	return 0;
}