#pragma once
#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <vector>

// Define screen
static const int SCREEN_WIDTH = 480;
static const int SCREEN_HEIGHT = 640;
static const int DICE_SIZE = 88;
static const int BUTTON_POS = 400;

/// <summary>
/// Renders text to given location
/// </summary>
/// <param name="renderer">Reference to renderer</param>
/// <param name="font">Reference to font</param>
/// <param name="text">What will be written</param>
/// <param name="color">What color will it be written</param>
/// <param name="x">x position</param>
/// <param name="y">y position</param>
/// <param name="w">width</param>
/// <param name="h">height</param>
void render_text(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, SDL_Color color, int x, int y, int w, int h);

/// <summary>
/// Clears given area with given color
/// </summary>
/// <param name="renderer">Reference to renderer</param>
/// <param name="color">Color that will be rendered</param>
/// <param name="x">x position</param>
/// <param name="y">y position</param>
/// <param name="w">width</param>
/// <param name="h">height</param>
void clear_area(SDL_Renderer* renderer, SDL_Color color, int x, int y, int w, int h);