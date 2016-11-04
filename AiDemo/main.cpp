#define _CRT_SECURE_NO_WARNINGS
#define MAX_ENEMIES 3
#define MAX_HEALTH 25
#define LAUNCH_COUNTER 1750
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <string>
#include <math.h>
#include "GameObject.h"
#include "agent.h"

const int SCREEN_WIDTH = 720;
const int SCREEN_HEIGTH = 480;

bool Initialize();
void Close();
Uint32 Time();

std::queue<Tile*> shownPath;
std::vector<SDL_Point> points;
SDL_Rect explScreenArea;
SDL_Rect explBitmapArea;
SDL_Rect guiScreenArea = { 10, 10, 200, 30 };
SDL_Rect guiBitmapArea;



class Tank : public Agent, public GameObject {


	public:

		Tank(Map* m, const char* texturePath, SDL_Renderer* renderer, Vector2D target, int posX, int posY, int w, int h, float s) : GameObject(texturePath, renderer, posX, posY, w, h), Agent(target){

			map = m;
			speed = s;
			health = MAX_HEALTH;

			healthBar = *SDL_LoadBMP("../Textures/healthBar.bmp");

			healthBarTexture = SDL_CreateTextureFromSurface(renderer, &healthBar);
		}

		void setVelocity(Vector2D dest, double speed) {

			int deltaTime = Time();


			velocity = Vector2D((dest.x - collider.center.x)/1000, (dest.y - collider.center.y)/1000) * deltaTime * speed;
		}

		bool MoveTowards(Vector2D dest, double arrivalRadius) {

			if (collider.center.distance(dest) > arrivalRadius){
				

				pos = pos + (velocity );

				return false;
			}

			return true;
		}

		void UpdateState(SDL_Renderer* renderer) {

			if (state == AgentState::IDLE) {

				if (target.x > 0 && target.y > 0)
					state = AgentState::PLANNING;
			}

			if (state == AgentState::PLANNING) {

				//Clear current path
				while (!getPath()->empty())
					getPath()->pop();

				//Calculate tank path 
				calculatePath(map->quantize(getPos()), map->quantize(target), map);

				//Show path
				
				shownPath = *getPath();

				points.clear();

				int size = shownPath.size();

				for (int i = 0; i < size ; i++) {

					SDL_Point point = { shownPath.front()->center.x, shownPath.front()->center.y };

					points.push_back(point);

					shownPath.pop();
				}

				//Set current tank destination and velocity
				if (!getPath()->empty()) {
					
					destination = getPath()->front();
					setVelocity(destination->center, speed);
					state = AgentState::EXECUTING;
				}

			}

			if (state == AgentState::EXECUTING) {


				//Move tank until reaches destination...
				if (MoveTowards(destination->center, 20.0f)) {

					if (!getPath()->empty()) {

						//...then get next destination and reset velocity
						destination = getPath()->front();

						setVelocity(destination->center, speed);

						getPath()->pop();
					}

					//If no more destination in path, revert to PLANNING state
					else {

						state = AgentState::IDLE;
						target = Vector2D(-1, -1);
					}
				}
					
			}

		}


		float getHealth() {

			return health;
		}

		void setHealth(float newHealth) {

			if (newHealth <= 0)
				newHealth = 0;

			health = newHealth;
		}

		void showHealth(SDL_Renderer* renderer) {


			if (health >= MAX_HEALTH - (MAX_HEALTH / 5) * 1 && health <= MAX_HEALTH )
				guiBitmapArea = { 0,0,200,30};

			else if (health >= MAX_HEALTH - (MAX_HEALTH / 5) * 2 && health <= MAX_HEALTH - (MAX_HEALTH / 5) * 1)
				guiBitmapArea = { 205,0,200,30 };

			else if (health >= MAX_HEALTH - (MAX_HEALTH / 5) * 3 && health <= MAX_HEALTH - (MAX_HEALTH / 5) * 2)
				guiBitmapArea = { 410,0,200,30 };

			else if (health >= MAX_HEALTH - (MAX_HEALTH / 5) * 4 && health <= MAX_HEALTH - (MAX_HEALTH / 5) * 3)
				guiBitmapArea = { 615,0,200,30 };

			else if (health >= MAX_HEALTH - (MAX_HEALTH / 5) * 5 && health <= MAX_HEALTH - (MAX_HEALTH / 5) * 4)
				guiBitmapArea = { 820,0,200,30 };



			if (SDL_RenderCopy(renderer, healthBarTexture, &guiBitmapArea, &guiScreenArea) != 0) {
				std::cout << "Can't render texture: " << SDL_GetError() << std::endl;

			}
		}


		Tile* getDestination() {

			return destination;
		}

		Vector2D getVelocity() {

			return velocity;
		}


	private:

		Vector2D velocity;
		Tile* destination;
		Map* map;
		float health;
		float speed;

		SDL_Surface healthBar;
		SDL_Texture* healthBarTexture;

};


enum Direction {LEFT, RIGHT, UP, DOWN, NONE};

class AntiTank : public GameObject {


	public : 

		AntiTank() : GameObject(){		}

		AntiTank(const char* texturePath, SDL_Renderer* renderer, int posX, int posY, int w, int h) : GameObject(texturePath, renderer, posX, posY, w, h) {
		
			missile = GameObject("../Textures/missileLeft.bmp", renderer, posX, posY, 20, 10);

			/*missileDir = Direction::NONE;

			soldierDir = Direction::LEFT;*/

			maxDamage = 5.0f;

			launchCounter = LAUNCH_COUNTER;

			explosionCounter = 0;

			missileArmed = true;
		}

		float getMaxDamage() {

			return maxDamage;
		}

		void setMaxDamage(float damage) {

			maxDamage = damage;
		}

		//Update missile position
		void UpdateMissile(SDL_Surface* screen, SDL_Renderer* renderer, Tank* tank, float speed) {

			//Move missile if armed
			if (missileArmed) {
				int deltaTime = Time();
				Vector2D velocity = Vector2D((tank->getCollider().center.x - collider.center.x) / 100, (tank->getCollider().center.y - collider.center.y) / 100) * deltaTime * speed;
				missile.setPos(missile.getPos() + velocity);
				missile.Render(screen, renderer, atan2(collider.center.y - tank->getCollider().center.y, collider.center.x - tank->getCollider().center.x) * 180 * M_PI * 0.1);
				
			}

			//Reset missile if it goes out of boundaries
			if (missile.getPos().x <= 0 || missile.getPos().x >= SCREEN_WIDTH ||
				missile.getPos().y <= 0 || missile.getPos().y >= SCREEN_HEIGTH) {
				missileArmed = false;

				missile.setPos(pos);
				
			}

			//Re-arm missile at countdown
			if (launchCounter <= 0) {
				launchCounter = LAUNCH_COUNTER;
				missileArmed = true;
			}

			launchCounter--;

		}

		void HandleCollision(SDL_Renderer* renderer, Tank* tank) {

			//Reset missile if it collides with tank
			if (missile.isColliding(tank) && missileArmed) {

				missile.setPos(pos);

				//Calculate and normalize distance from soldier
				float distance = tank->getPos().distance(pos);

				float maxDistance = sqrt(powl(SCREEN_WIDTH, 2.0) + powl(SCREEN_HEIGTH, 2.0));

				float normalizedDistance = distance / maxDistance;

				//Add damage based on distance

				tank->setHealth(tank->getHealth() - (maxDamage / (pow(normalizedDistance + 1, 10.0f))));

				missileArmed = false;

				explosionCounter = 75;

			}


			if (explosionCounter > 0) {

				//Show explosion
				SDL_Surface explosionSprite = *SDL_LoadBMP("../Textures/explosion.bmp");

				SDL_Texture* texture;


				try {
					texture = SDL_CreateTextureFromSurface(renderer, &explosionSprite);

				}

				catch (std::exception e) {

					std::cout << e.what();

				}


				if (texture == NULL)
					std::cout << "Can't create texture" << std::endl;

				explScreenArea = { (int)tank->getPos().x, (int)tank->getPos().y, 141, 141 };
				explBitmapArea = { 0,0,141,141 };

				if (SDL_RenderCopy(renderer, texture, &explBitmapArea, &explScreenArea) != 0) {
					std::cout << "Can't render texture: " << SDL_GetError() << std::endl;

				}

				explosionCounter--;

			}


		}

	
	private:
		float maxDamage;
		GameObject missile;
		int launchCounter;
		bool missileArmed;
		int explosionCounter;
		
};



SDL_Texture* LoadTexture(std::string file);

SDL_Window* window = NULL;
SDL_Surface* screen = NULL;
SDL_Renderer* renderer = NULL;

Uint32 startTime;

int main(int argc, char* argv[]) {

	TTF_Init();

	if (!Initialize()) {
		std::cout << "Could not initialize";
		return -1;
	}

	int frameCounter = 0;

	bool exit = false;

	SDL_Event e;

	//Set background and screen rects

	SDL_Rect screenRectangle = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGTH };


	//Generate map of tiles
	Map map(40, SCREEN_WIDTH, SCREEN_HEIGTH);

	map.generateTiles();


	//Enemies
	std::vector<AntiTank> enemies;


	//Create tank

	Tank tank(&map, "../Textures/tank.bmp", renderer, Vector2D(700, 240), 10, 200, 143, 94, 0.25f);

	

	//Main loop
	while (!exit) {

		startTime = SDL_GetTicks();

		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT || e.key.keysym.sym == SDLK_ESCAPE)
				exit = true;

			if (e.type == SDL_MOUSEBUTTONDOWN) {

				int x;
				int y;

				//If left mouse button pressed...
				if (SDL_GetMouseState(&x, &y) & SDL_BUTTON(SDL_BUTTON_LEFT) && enemies.size() < MAX_ENEMIES) {

					//Create anti-tank soldier on clicked position
					AntiTank antiTank("../Textures/soldierleft.bmp", renderer, x, y, 30, 25);
					antiTank.setMaxDamage(10.0f);

					//Spread influence from anti-tank soldier position
					map.spreadInfluence(antiTank.getPos(), antiTank.getMaxDamage());


					//Add anti-tank soldier to enemies list
					enemies.push_back(antiTank);

					//Change tank state
					tank.setState(AgentState::PLANNING);
				}
			}
		}
 
		//Clear the buffer
		SDL_RenderClear(renderer);

		//Rendering background 
		//SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);


		if (tank.getHealth() > 0) {
			//Update tank state
			tank.UpdateState(renderer);

			tank.Render(screen, renderer, atan2(tank.getVelocity().y, tank.getVelocity().x) * 180 * M_PI * 0.1);

			//Show health bar
			tank.showHealth(renderer);
		}
		
		//Show influence map
		for (int i = 0; i < map.tiles.size(); i++)
			for (int j = 0; j < map.tiles[i].size(); j++)
			{
				SDL_SetRenderDrawColor(renderer, 255, 0, 0, (255 * map.tiles[i][j].influence) / 20);

				SDL_Rect tile;

				tile.x = map.tiles[i][j].center.x - map.getTilesSize() / 2;
				tile.y = map.tiles[i][j].center.y - map.getTilesSize() / 2;

				tile.w = map.getTilesSize();
				tile.h = map.getTilesSize();


				SDL_RenderFillRect(renderer, &tile);

			}

		//Show path
		SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
		

		SDL_RenderDrawLines(renderer, points.data(), points.size());

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);

		//Rendering enemies
		for (int i = 0; i < enemies.size(); i++) {

			enemies[i].Render(screen, renderer, atan2(enemies[i].getCollider().center.y - tank.getCollider().center.y, enemies[i].getCollider().center.x - tank.getCollider().center.x)* 180 / M_PI);
			
			enemies[i].UpdateMissile(screen, renderer, &tank, 0.1f);

			enemies[i].HandleCollision(renderer, &tank);
		}


		//Update screen with back-buffer renderings
		SDL_RenderPresent(renderer);
	}

	return 0;
}


//Initialize renderer, window and screen
bool Initialize() {

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cout << "SDL could not be initialized";
		return false;
	}

	window = SDL_CreateWindow("AI Demo",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		SCREEN_WIDTH, SCREEN_HEIGTH, SDL_WINDOW_SHOWN);

	if (window == NULL) {

		std::cout << "Window could not be created!";
	}

	screen = SDL_GetWindowSurface(window);

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	if (renderer == NULL) {

		std::cout << "Not able to create the renderer";
		return false;
	}

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	int imgFlags = IMG_INIT_PNG;

	if (!(IMG_Init(imgFlags) & imgFlags)) {

		std::cout << "SDL_image could not be initialized! SDL_image Error";
		return false;
	}

	startTime = SDL_GetTicks();

	return true;
}

void Close() {

	SDL_DestroyRenderer(renderer);

	SDL_DestroyWindow(window);

	window = NULL;

	SDL_Quit();
}

SDL_Texture* LoadTexture(std::string file) {

	SDL_Texture* newTexture = NULL;
	SDL_Surface* loadedSurface = IMG_Load(file.c_str());

	if (loadedSurface == NULL)
		std::cout << "Unable to load the image ! " << IMG_GetError;
	else {

		newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);

		if (newTexture == NULL)
			std::cout << "Unable to create the texture";

		SDL_FreeSurface(loadedSurface);
	}

	return newTexture;
}

Uint32 Time() {

	return (SDL_GetTicks() - startTime) + 1;
}