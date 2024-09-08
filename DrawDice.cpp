#include "DrawDice.h"

void DrawDice::draw_shape(SDL_Renderer* renderer, int type_id) const
{
	// Clear old draw
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_Rect rect = {_relative_x + 1, _relative_y + 1, _dice_w - 2, _dice_h - 2};
	SDL_RenderFillRect(renderer, &rect);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	// Draw new
	switch (type_id)
	{
	case 1:
		draw_dot(renderer, 1, 1);
		break;
	case 2:
		draw_dot(renderer, 0, 0);
		draw_dot(renderer, 2, 2);
		break;
	case 3:
		draw_dot(renderer, 2, 0);
		draw_dot(renderer, 1, 1);
		draw_dot(renderer, 0, 2);
		break;
	case 4:
		draw_dot(renderer, 0, 0);
		draw_dot(renderer, 0, 2);
		draw_dot(renderer, 2, 0);
		draw_dot(renderer, 2, 2);
		break;
	case 5:
		draw_dot(renderer, 0, 0);
		draw_dot(renderer, 0, 2);
		draw_dot(renderer, 2, 0);
		draw_dot(renderer, 2, 2);
		draw_dot(renderer, 1, 1);
		break;
	case 6:
		draw_dot(renderer, 0, 0);
		draw_dot(renderer, 0, 1);
		draw_dot(renderer, 0, 2);
		draw_dot(renderer, 2, 0);
		draw_dot(renderer, 2, 1);
		draw_dot(renderer, 2, 2);
		break;
	default:
		draw_unknown(renderer);
		break;
	}
}

void DrawDice::draw_dot(SDL_Renderer* renderer, int x_i, int y_i) const
{
	int x_pos, y_pos;
	get_dot_relative_pos(x_pos, x_i, _relative_x, _dice_w);
	get_dot_relative_pos(y_pos, y_i, _relative_y, _dice_h);
	SDL_Rect dot = { x_pos + _relative_x, y_pos + _relative_y, _dice_w / 6, _dice_h / 6 };
	SDL_RenderFillRect(renderer, &dot);
}

void DrawDice::get_dot_relative_pos(int& pos, const int& i, int axis, int size) const
{
	int padding = size / _padding_div;
	pos = padding + i * (size / 3);
}

void DrawDice::draw_unknown(SDL_Renderer* renderer) const
{
	SDL_Rect rect = { _relative_x, _relative_y, _dice_w, _dice_h };
	SDL_SetRenderDrawColor(renderer, 75, 75, 75, 255);  // Gray for the face
	SDL_RenderFillRect(renderer, &rect);

	// Draw an "X"
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // Black for the lines
	SDL_RenderDrawLine(renderer, _relative_x, _relative_y, _relative_x + _dice_w, _relative_y + _dice_h);
	SDL_RenderDrawLine(renderer, _relative_x, _relative_y + _dice_h, _relative_x + _dice_w, _relative_y);
}



