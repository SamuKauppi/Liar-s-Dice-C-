#pragma once
#include <SDL.h>

/// <summary>
/// Class used to render dice values
/// </summary>
class DrawDice {
public:
    DrawDice(int relative_x, int relative_y, int dice_w, int dice_h, int padding = 10)
        : _relative_x(relative_x), _relative_y(relative_y),
        _dice_w(dice_w), _dice_h(dice_h), _padding_div(padding) {};

    void draw_shape(SDL_Renderer* renderer, int type_id) const;

private:
    int _relative_x;
    int _relative_y;
    int _dice_w;
    int _dice_h;
    int _padding_div;

    void draw_dot(SDL_Renderer* renderer, int x_i, int y_i) const;
    void get_dot_relative_pos(int& pos, const int& i, int axis, int size) const;
    void draw_unknown(SDL_Renderer* renderer) const;
};
