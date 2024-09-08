#define SDL_MAIN_HANDLED
#include <iostream>
#include <string>
#include <random>
#include <time.h>
#include <SDL.h>
#include "Gamestate.h"
#include "DrawDice.h"

using std::cout;
using std::cin;
using std::string;

static const int SCREEN_WIDTH = 480;
static const int SCREEN_HEIGHT = 640;

void draw_dices(SDL_Renderer* renderer, bool is_player, const std::vector<int>& dices, int dice_size);

/// <summary>
/// Main function with the gameloop
/// </summary>
/// <returns></returns>
int main()
{
	// Set random seed
	srand(time(NULL));

	// Define SDL screen
	SDL_SetMainReady();
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("error initializing SDL: %s\n", SDL_GetError());
	}
	SDL_Renderer* renderer = nullptr;
	SDL_Window* window = nullptr;
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);

	// Create game state
	GameState state(2);
	Player* turn_player;
	AI_Player* ai_player;
	bool game_loop = true;
	bool round_loop = true;
	bool turn_loop = true;
	int winner_id = -1;
	int loser_id = -1;

	while (game_loop)
	{
		// Start a new round
		state.start_new_round(winner_id, loser_id);

		// Check if game has ended
		game_loop = !state.is_game_over();
		if (!game_loop)
		{
			break;
		}

		// Update variables
		round_loop = game_loop;
		turn_loop = game_loop;

		// Clear old screen
		SDL_RenderClear(renderer);
		// Draw dices
		for (Player* p : state.players)
		{
			draw_dices(renderer, !p->is_ai, p->get_dices(), 88);
		}
		// TODO: draw player controls

		// Present
		SDL_RenderPresent(renderer);

		while (round_loop)
		{
			turn_player = state.turnplayer;
			int playerID = turn_player->playerID + 1;

			while (turn_loop)
			{
				// Make count and dice
				int count = -1, dice = -1;

				while (count == -1 || dice == -1)
				{
					// Check if it's a player
					if (turn_player->is_ai)
					{
						ai_player = static_cast<AI_Player*>(turn_player);
						int bid[2] = { 0, 0 };
						ai_player->evaluate_bid(bid, state.d_last_bid, state.d_count, turn_player->playerID);
						count = bid[0];
						dice = bid[1];
					}
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
			}
		}
	}

	cout << "\n\nGame is over!";
	cout << "\nWinner is: Player " << state.players[winner_id]->playerID + 1 << ".\n";
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}

void draw_dices(SDL_Renderer* renderer, bool is_player, const std::vector<int>& dices, int dice_size)
{
	for (int i = 0; i < dices.size(); i++)
	{
		// Create rect
		SDL_Rect temp{};
		temp.w = dice_size;
		temp.h = dice_size;
		temp.x = 10 + temp.w * i + i * 5;
		temp.y = is_player ? SCREEN_HEIGHT - temp.h - 10 : 10;

		// Draw it white
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderDrawRect(renderer, &temp);

		// Draw the inside of the dice
		DrawDice dice(temp.x, temp.y, dice_size, dice_size, 10);
		if (is_player)
		{
			dice.draw_shape(renderer, dices[i]);
		}
		else
		{
			dice.draw_shape(renderer, 0);
		}
	}
}
