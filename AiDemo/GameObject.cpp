#include <iostream>
#include <exception>
#include "GameObject.h"


GameObject::GameObject()
{
	width = 0;
	height = 0;

	pos.x = 0;
	pos.y = 0;

	//sprite = NULL;
}

GameObject::GameObject(const char* texturePath, SDL_Renderer* renderer, int posX, int posY, int w, int h)
{

	width = w;
	height = h;

	sprite = *SDL_LoadBMP(texturePath);


	pos = Vector2D(posX, posY);

	bitmapArea = { 0,0,width,height };
	screenArea = { (int)pos.x, (int)pos.y, width, height };


	collider.center = Vector2D(pos.x + w / 2, pos.y + h / 2);

	collider.width = w;
	collider.heigth = h;


	try {
		texture = SDL_CreateTextureFromSurface(renderer, &sprite);

	}

	catch (std::exception e) {

		std::cout << e.what();

	}
}


GameObject::~GameObject(){}

void GameObject::Render(SDL_Surface * screen, SDL_Renderer * renderer, double angle)
{

	screenArea = { (int)pos.x, (int)pos.y, width, height };
	bitmapArea = { 0,0,width,height };
	collider.center = Vector2D(pos.x + width / 2, pos.y + height/ 2);
	pivot = {0 + width / 2, 0 + height / 2 };

	if (SDL_RenderCopyEx(renderer, texture, &bitmapArea, &screenArea, angle, &pivot, SDL_FLIP_NONE) != 0) {
		std::cout << "Can't render texture: " << SDL_GetError() << std::endl;

	}
}

Vector2D GameObject::getPos()
{
	return pos;
}

void GameObject::setPos(Vector2D newPos)
{
	pos = newPos;
}

void GameObject::setSprite(const char * path)
{
	sprite = *SDL_LoadBMP(path);
}

Collider GameObject::getCollider()
{
	return collider;
}

bool GameObject::isColliding(GameObject * obj)
{

	return (abs(collider.center.x - obj->getCollider().center.x) * 2 < (collider.width + obj->getCollider().width)) &&
		   (abs(collider.center.y - obj->getCollider().center.y) * 2 < (collider.heigth + obj->getCollider().heigth));
}

int GameObject::getWidth()
{
	return width;
}

int GameObject::getHeight()
{
	return height;
}
