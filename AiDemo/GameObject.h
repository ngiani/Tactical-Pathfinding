#include <SDL.h>
#include <SDL_image.h>

#pragma once
#include "map.h"


struct Collider {

	Vector2D center;
	int width;
	int heigth;
};

class GameObject
{
	public:
		GameObject();
		GameObject(const char* texturePath, SDL_Renderer* renderer, int posX, int posY, int w, int h);
		~GameObject();
		void Render(SDL_Surface* screen, SDL_Renderer* renderer, double angle);
		Vector2D getPos();
		void setPos(Vector2D);
		void setSprite(const char* path);
		Collider getCollider();
		bool isColliding(GameObject* obj);
		int getWidth();
		int getHeight();

	protected:
		Vector2D pos;
		int width;
		int height;

		SDL_Rect bitmapArea;
		SDL_Rect screenArea;
		SDL_Surface sprite;
		SDL_Texture* texture;
		SDL_Point pivot;
		
		Collider collider;
};

