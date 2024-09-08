#define SDL_MAIN_HANDLED
#include <iostream>
#include <string>
#include <random>
#include <time.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <array>
#include "Gamestate.h"
#include <memory>
#include "DrawDice.h"
#include "Render_text.h"


// Using to make code cleaner
using std::cout;
using std::cin;
using std::string;

// Define sdl colors
static const SDL_Color COLOR_W = { 255, 255, 255, 255 };
static const SDL_Color COLOR_B = { 0, 0, 0, 255 };

// Define a type alias for clarity
using DrawSlot = std::array<int, 4>;

/// <summary>
/// Draws dices either on player or ai side
/// </summary>
/// <param name="renderer">Reference to renderer</param>
/// <param name="is_player">Are the dices drawn to the player</param>
/// <param name="reveal">Should the dices drawn to the ai be revealed</param>
/// <param name="dices">Reference to the dice values</param>
static void draw_dices(SDL_Renderer* renderer, bool is_player, bool reveal, const std::vector<int>& dices)
{
	for (int i = 0; i < dices.size(); i++)
	{
		// Create rect
		SDL_Rect temp{};
		temp.w = DICE_SIZE;
		temp.h = DICE_SIZE;
		temp.x = 10 + temp.w * i + i * 5;
		temp.y = is_player ? SCREEN_HEIGHT - temp.h - 10 : 10;

		// Draw it white
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderDrawRect(renderer, &temp);

		// Draw the inside of the dice
		DrawDice dice(temp.x, temp.y, DICE_SIZE, DICE_SIZE, 10);
		if (is_player || reveal)
		{
			// Draw dice value if player
			dice.draw_shape(renderer, dices[i]);
		}
		else
		{
			// Draw unknown if not
			dice.draw_shape(renderer, 0);
		}
	}
}

/// <summary>
/// Confirms that the player input is inside button
/// </summary>
/// <param name="x_pos">x position of mouse</param>
/// <param name="y_pos">y position of mouse</param>
/// <param name="x">x position of button</param>
/// <param name="y">x position of button</param>
/// <param name="w">width of button</param>
/// <param name="h">height of button</param>
/// <returns>is x_pos and y_pos inside area</returns>
static bool is_in_area(int x_pos, int y_pos, int x, int y, int w, int h)
{
	bool is_x = x_pos >= x && x_pos <= w + x;
	bool is_y = y_pos >= y && y_pos <= h + y;

	if (is_x && is_y)
		return true;

	return false;
}

/// <summary>
/// Generates slots where the previous bids are drawn into
/// </summary>
/// <param name="x_pos">x position of the first slot</param>
/// <param name="y_pos">y position of the first slot</param>
/// <param name="width">width of the slots</param>
/// <param name="height">height of the slot</param>
/// <param name="count">how many will be generated</param>
/// <returns>created slots</returns>
static std::vector<DrawSlot> generate_draw_slots(int x_pos, int y_pos, int width, int height, int count) {
	std::vector<DrawSlot> actions;
	for (int i = 0; i < count; i++) {
		actions.push_back({ x_pos, y_pos + height * i / 2 + 5, width, height });
	}
	return actions;
}

/// <summary>
/// Handles logic deciding who won
/// </summary>
/// <param name="state">Current game state</param>
/// <param name="renderer">SDL renderer</param>
/// <param name="turn_player">pointer to turnplayer</param>
/// <param name="playerID">int of the player id</param>
/// <param name="winner_id">where winner id is stored</param>
/// <param name="loser_id">where loser id is stored</param>
static void handle_calling_liar(GameState& state, 
	SDL_Renderer* renderer,
	std::shared_ptr<Player>& turn_player,
	int playerID, 
	int& winner_id, 
	int& loser_id)
{

	// Reveal player dices
	for (auto& p : state.players)
	{
		draw_dices(renderer, !p->is_ai, true, p->get_dices());
		cout << "\n";
		p->print_dice();
	}
	SDL_RenderPresent(renderer);
	SDL_Delay(1500);

	// count the amount of correct dices and check who won
	int count = state.call();
	if (count < state.d_last_bid[0])
	{
		// Player correctly called liar
		cout << "\nPlayer " << playerID << " correctly called liar\n";
		winner_id = turn_player->playerID;
		loser_id = state.get_previous_player_id();
	}
	else
	{
		// Player incorrectly called liar
		cout << "\nPlayer " << playerID << " incorreclty called liar\n";
		loser_id = turn_player->playerID;
		winner_id = state.get_previous_player_id();
	}
}
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

	// Define sdl ttf
	if (TTF_Init() != 0) {
		std::cerr << "TTF_Init Error: " << TTF_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}

	// Load font
	TTF_Font* font = TTF_OpenFont("advanced_pixel-7.ttf", 24); // Ensure 'arial.ttf' is in the same directory as your executable or provide the full path
	if (font == nullptr) {
		std::cerr << "TTF_OpenFont Error: " << TTF_GetError() << std::endl;
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		TTF_Quit();
		SDL_Quit();
		return 1;
	}

	// Create positions for the input buttons
	SDL_Rect c_up = { 10, BUTTON_POS, 50, 20 };
	SDL_Rect c_down = { 10, BUTTON_POS + 100, 50, 20 };
	SDL_Rect d_up = { 100, BUTTON_POS, 50, 20 };
	SDL_Rect d_down = { 100, BUTTON_POS + 100, 50, 20 };
	SDL_Rect liar = { SCREEN_WIDTH - 140, BUTTON_POS + 50, 120, 60 };
	SDL_Rect confirm = { 180, BUTTON_POS + 50, 120, 60 };

	// Create positions for the bidding text
	const int plr_bid_txt_a[4] = { 10, BUTTON_POS + 20, 140, 80 };
	const int ai_bid_txt_a[4] = { 10, 100, 140, 80 };

	// Create slots where text for previous bids goes
	std::vector<DrawSlot> plr_actions = generate_draw_slots(180, 100, 120, 40, 20);
	int plr_act_id = 0;
	std::vector<DrawSlot> ai_actions = generate_draw_slots(SCREEN_WIDTH - 140, 105, 120, 40, 20);
	int ai_act_id = 0;

	// Create gamestate
	GameState state(2);
	std::shared_ptr<Player> turn_player;
	std::shared_ptr<AI_Player> ai_player;
	bool game_loop = true;
	bool round_loop;
	bool turn_loop;
	int winner_id = -1;
	int loser_id = -1;

	// Remember last bid made by the player for clarity
	int last_count, last_dice;

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
		last_count = 1;
		last_dice = 2;

		// Clear old screen
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		// Draw dices
		for (const auto& p : state.players)
		{
			draw_dices(renderer, !p->is_ai, false, p->get_dices());
		}

		// Draw input buttons
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderFillRect(renderer, &c_up);
		SDL_RenderFillRect(renderer, &c_down);
		SDL_RenderFillRect(renderer, &d_up);
		SDL_RenderFillRect(renderer, &d_down);
		SDL_RenderFillRect(renderer, &liar);
		SDL_RenderFillRect(renderer, &confirm);

		// Render text
		render_text(renderer, font, "Call Liar!", COLOR_B, liar.x, liar.y, liar.w, liar.h);
		render_text(renderer, font, "Confirm", COLOR_B, confirm.x, confirm.y, confirm.w, confirm.h);

		while (round_loop)
		{
			turn_player = state.turnplayer;
			int playerID = turn_player->playerID + 1;

			// Turn loop
			while (turn_loop)
			{
				// Make count and dice
				int count = last_count;
				int dice = last_dice;

				// Check if it's a player
				if (turn_player->is_ai)
				{
					// Cast the turnplayer as ai and have it give a bid (00 = call liar)
					ai_player = std::dynamic_pointer_cast<AI_Player>(turn_player);
					int bid[2] = { 0, 0 };
					ai_player->evaluate_bid(bid, state.d_last_bid, state.d_count, turn_player->playerID);
					count = bid[0];
					dice = bid[1];

					// Render text
					SDL_Delay(500);
				}
				else
				{
					// Render text
					clear_area(renderer, COLOR_B, plr_bid_txt_a[0], plr_bid_txt_a[1], plr_bid_txt_a[2], plr_bid_txt_a[3]);
					render_text(renderer, font, std::to_string(count) + " x " + std::to_string(dice),
						COLOR_W, plr_bid_txt_a[0], plr_bid_txt_a[1], plr_bid_txt_a[2], plr_bid_txt_a[3]);
					SDL_RenderPresent(renderer);

					// Detect player input
					bool continue_turn = true;
					while (continue_turn)
					{
						SDL_Event e;
						while (SDL_PollEvent(&e) != 0)
						{
							if (e.type == SDL_QUIT)
							{
								TTF_CloseFont(font);
								SDL_DestroyRenderer(renderer);
								SDL_DestroyWindow(window);
								TTF_Quit();
								SDL_Quit();
							}

							if (e.type == SDL_MOUSEBUTTONDOWN)
							{
								int mouseX, mouseY;
								SDL_GetMouseState(&mouseX, &mouseY);

								// Increse count
								if (is_in_area(mouseX, mouseY, c_up.x, c_up.y, c_up.w, c_up.h))
								{
									count = count >= 10 ? 1 : count + 1;
								}
								// Decrease count
								if (is_in_area(mouseX, mouseY, c_down.x, c_down.y, c_down.w, c_down.h))
								{
									count = count <= 1 ? 10 : count - 1;
								}
								// Increse dice value
								if (is_in_area(mouseX, mouseY, d_up.x, d_up.y, d_up.w, d_up.h))
								{
									dice = dice >= 6 ? 2 : dice + 1;
								}
								// Decrese dice value
								if (is_in_area(mouseX, mouseY, d_down.x, d_down.y, d_down.w, d_down.h))
								{
									dice = dice <= 2 ? 6 : dice - 1;
								}
								// Confirm selection
								if (is_in_area(mouseX, mouseY, confirm.x, confirm.y, confirm.w, confirm.h))
								{
									continue_turn = false;
								}
								// Call liar
								if (is_in_area(mouseX, mouseY, liar.x, liar.y, liar.w, liar.h))
								{
									continue_turn = false;
									count = 0;
									dice = 0;
								}
								// Draw changes
								clear_area(renderer, COLOR_B, plr_bid_txt_a[0], plr_bid_txt_a[1], plr_bid_txt_a[2], plr_bid_txt_a[3]);
								render_text(renderer, font, std::to_string(count) + " x " + std::to_string(dice),
									COLOR_W, plr_bid_txt_a[0], plr_bid_txt_a[1], plr_bid_txt_a[2], plr_bid_txt_a[3]);
								SDL_RenderPresent(renderer);
							}
						}
						// Update players last bid
						last_count = count;
						last_dice = dice;
					}
				}

				// count was 0, it means that the player wants to call
				if (count == 0 && state.d_last_bid[0] > 0)
				{
					// Create the text "Liar!" in the action slots
					int act_id = turn_player->is_ai ? ai_act_id : plr_act_id;
					const auto& actions = turn_player->is_ai ? ai_actions : plr_actions;
					string bid = "Liar!";
					render_text(renderer, font, bid, COLOR_W,
						actions[act_id][0],
						actions[act_id][1],
						actions[act_id][2],
						actions[act_id][3]);

					// Handle counting the winner
					handle_calling_liar(state, renderer, turn_player, playerID, winner_id, loser_id);
					clear_area(renderer, COLOR_B, 0, SCREEN_HEIGHT / 4, SCREEN_WIDTH, SCREEN_HEIGHT / 2);
					string winner_txt = winner_id == 0 ? "You win! (click to continue)" : "You Lose! (click to continue)";
					render_text(renderer, font, winner_txt, COLOR_W, 0, SCREEN_HEIGHT / 3, SCREEN_WIDTH, SCREEN_HEIGHT / 5);
					SDL_RenderPresent(renderer);

					SDL_Event e;
					bool waiting = true;

					while (waiting) {
						if (SDL_WaitEvent(&e) && e.type == SDL_MOUSEBUTTONDOWN) {
							waiting = false; // Exit the loop when the mouse button is clicked
						}
						if (e.type == SDL_QUIT)
						{
							TTF_CloseFont(font);
							SDL_DestroyRenderer(renderer);
							SDL_DestroyWindow(window);
							TTF_Quit();
							SDL_Quit();
						}
					}

					// Reset variables
					turn_loop = false;
					round_loop = false;
					plr_act_id = 0;
					ai_act_id = 0;
				}
				else if (state.make_bid(count, dice, turn_player->playerID))
				{

					// Render the bid into thebid list
					int act_id = turn_player->is_ai ? ai_act_id : plr_act_id;
					const auto& actions = turn_player->is_ai ? ai_actions : plr_actions;
					string bid = std::to_string(count) + " x " + std::to_string(dice);
					render_text(renderer, font, bid, COLOR_W,
						actions[act_id][0],
						actions[act_id][1],
						actions[act_id][2],
						actions[act_id][3]);
					SDL_RenderPresent(renderer);

					plr_act_id++;
					ai_act_id++;

					// pass turn
					state.next_player();
					cout << "\nPlayer " << playerID << " made the bid: " << count << " x " << dice;
					break;
				}
			}
		}
	}

	cout << "\n\nGame is over!";
	cout << "\nWinner is: Player " << state.players[winner_id]->playerID + 1 << ".\n";

	// Draw the end screen
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	string winner_txt = winner_id == 0 ? "You win the game!" : "You lose the game!";
	render_text(renderer, font, winner_txt, COLOR_W, 0, SCREEN_HEIGHT / 3, SCREEN_WIDTH, SCREEN_HEIGHT / 5);
	SDL_RenderPresent(renderer);

	// Wait for player click
	SDL_Event e;
	bool wait = true;
	while (wait) {
		if (SDL_WaitEvent(&e) && e.type == SDL_MOUSEBUTTONDOWN) {
			wait = false; // Exit the loop when the mouse button is clicked
		}
		if (e.type == SDL_QUIT)
		{
			TTF_CloseFont(font);
			SDL_DestroyRenderer(renderer);
			SDL_DestroyWindow(window);
			TTF_Quit();
			SDL_Quit();
		}
	}

	// Clean screen
	TTF_CloseFont(font);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	TTF_Quit();
	SDL_Quit();
	return 0;
}


