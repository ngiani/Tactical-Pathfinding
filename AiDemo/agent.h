#pragma once
#include <queue>
#include "dstructures.h"
#include "map.h"


enum AgentState{
	IDLE,
	PLANNING,
	EXECUTING
};

class Agent {

	public:
		
		Agent(Vector2D target);
		void calculatePath(Tile* start, Tile* destination, Map* map);
		std::queue<Tile*>* getPath();
		std::queue<Tile*>* getSmoothedPath();
		AgentState getState();
		void setState(AgentState state);
		
	private:
		void generateGraph(Map* map, Graph* graph);
		std::vector<Connection*> AStar(Node* start, Node* goal, Graph* graph, Map* map);
		
		static double heuristic(Node* node, Node* goal, Map* map);
		static Tile* getTile(Node* node, Map* map);
		std::queue<Tile*> path;
		std::queue<Tile*> smoothedPath;
	
	protected:	
		std::queue<Tile*> processPath(std::queue<Tile*> path, float distanceThreshold);
		AgentState state;
		Vector2D target;
};

