#include "Render_text.h"

void render_text(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, SDL_Color color, int x, int y, int w, int h)
{
	// Create a surface with the text
	SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
	if (surface == nullptr) {
		SDL_Log("TTF_RenderText_Solid Error: %s", TTF_GetError());
		return;
	}

	// Create a texture from the surface
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (texture == nullptr) {
		SDL_Log("SDL_CreateTextureFromSurface Error: %s", SDL_GetError());
		SDL_FreeSurface(surface);
		return;
	}

	// Set the rendering area
	SDL_Rect dstRect = { x, y, w, h };

	// Copy the texture to the renderer
	SDL_RenderCopy(renderer, texture, nullptr, &dstRect);

	// Free the resources
	SDL_DestroyTexture(texture);
	SDL_FreeSurface(surface);
}

void clear_area(SDL_Renderer* renderer, SDL_Color color, int x, int y, int w, int h)
{
	SDL_Rect clearRect = { x, y, w, h };
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	SDL_RenderFillRect(renderer, &clearRect);
}
