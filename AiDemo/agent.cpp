#include "agent.h"
#include <algorithm>
#include <iostream>
#define MAX_INFLUENCE 20
#define DISTANCE_THRESHOLD 50.0f

const int influenceWeight;
const int heuristicWeight;

Agent::Agent(Vector2D target) : target(target){

	state = AgentState::IDLE;

	std::cout << "Set Influence weight (1-10): " << std::endl;

	std::cin >> influenceWeight;

	std::cout << "Set Heuristic weight (1-10): " << std::endl;

	std::cin >> heuristicWeight;
}



//Calculate path 
void Agent::calculatePath(Tile* start, Tile* destination, Map* map) {

	//Generate graph from map of tiles
	Graph graph;
	generateGraph(map, &graph);

	//Get nodes corresponding to start and destination tiles
	Node* startNode = &graph.nodes[start->ID];
	Node* endNode = &graph.nodes[destination->ID];

	//A* path as vector of connections
	std::vector<Connection*> pathConns = AStar(startNode, endNode, &graph, map);

	//Get path as vector of tiles
	for (int i = 0; i < pathConns.size(); i++) 
		for (int j = 0; j + i < pathConns.size(); j++)
			if (i == 0 || i == pathConns.size() - 1 || getTile(pathConns[i]->end, map)->center.distance(getTile(pathConns[i + j]->end, map)->center) >= DISTANCE_THRESHOLD) {
				path.push(getTile(pathConns[i]->end, map));
				i += j;
				break;
			}


}


void Agent::generateGraph(Map * map, Graph * graph){


	//Generate nodes
	for (int i = 0; i < map->tiles.size(); i++)
		for (int j = 0; j < map->tiles[i].size(); j++) {

			//Create a node with current tile ID

			graph->addNode(map->tiles[i][j].ID);

		}

	//Generate connections
	for (int i = 0; i < map->tiles.size(); i++)
		for (int j = 0; j < map->tiles[i].size(); j++) {

			//Create a connection to each neighbouring tile of current tile

			//Right

			if (j + 1 < map->tiles[i].size()) {


				//Calculate tactical cost based on influence of end tile
				float baseCost = 1.0f;
				float influence = map->tiles[i][j + 1].influence;
				float normalizedInfluence = (influence * 10) / MAX_INFLUENCE;

				float cost = baseCost + (influenceWeight * normalizedInfluence);


				//Create start and end node, and add connection between them
				Node* start = &graph->nodes[map->tiles[i][j].ID];
				Node* end = &graph->nodes[map->tiles[i][j+1].ID];

				graph->addConnection(start->ID,cost, start, end);
			}

			//Left
			if (j -1 >= 0) {

				//Calculate tactical cost based on influence of end tile
				float baseCost = 1.0f;
				float influence = map->tiles[i][j - 1].influence;
				float normalizedInfluence = (influence * 10) / MAX_INFLUENCE;

				float cost = baseCost + (influenceWeight * normalizedInfluence);

				//Create start and end node, and add connection between them
				Node* start = &graph->nodes[map->tiles[i][j].ID];
				Node* end = &graph->nodes[map->tiles[i][j - 1].ID];

				graph->addConnection(start->ID , cost, start, end);
			}

			//Up
			if (i - 1 >= 0) {

				//Calculate tactical cost based on influence of end tile
				float baseCost = 1.0f;
				float influence = map->tiles[i - 1][j].influence;
				float normalizedInfluence = (influence * 10) / MAX_INFLUENCE;

				float cost = baseCost + (influenceWeight * normalizedInfluence);

				//Create start and end node, and add connection between them
				Node* start = &graph->nodes[map->tiles[i][j].ID];
				Node* end = &graph->nodes[map->tiles[i-1][j].ID];

				graph->addConnection(start->ID, cost, start, end);
			}

			//Down
			if (i + 1 < map->tiles.size()) {

				//Calculate tactical cost based on influence of end tile
				float baseCost = 1.0f;
				float influence = map->tiles[i + 1][j].influence;
				float normalizedInfluence = (influence * 10) / MAX_INFLUENCE;

				float cost = baseCost + (influenceWeight * normalizedInfluence);

				//Create start and end node, and add connection between them
				Node* start = &graph->nodes[map->tiles[i][j].ID];
				Node* end = &graph->nodes[map->tiles[i + 1][j].ID];

				graph->addConnection(start->ID, cost, start, end);
			}
				
		}
			
}


//f = g() + h()
bool compare(NodeRecord* a, NodeRecord* b) {

	return a->costSoFar + (heuristicWeight * a->heuristic) < b->costSoFar + (heuristicWeight * b->heuristic);
}

//Get record of start node in connection, from specified vector of records
NodeRecord* getStartRecord(std::vector<NodeRecord*>& vector, Connection* connection ) {

	std::vector<NodeRecord*>::iterator recordClosedIndex =
		std::find_if(vector.begin(), vector.end(),
			[connection](NodeRecord* r) { return r->node == connection->start; });

	if (recordClosedIndex != vector.end())
		return *recordClosedIndex;

	return NULL;
}


std::vector<Connection*> Agent::AStar(Node* start, Node* goal, Graph* graph, Map* map)
{

	std::vector<Connection*>  path;

	//Already discovered nodes
	std::vector<NodeRecord*> closed;

	//Nodes visited but to be discovered (only start node is known at the beginning)
	std::vector<NodeRecord*> open;

	NodeRecord startRecord;
	startRecord.node = start;
	open.push_back(&startRecord);

	//Current node record
	NodeRecord* currentRecord = new NodeRecord;
	
	//While open list is not empty
	while (!open.empty()) {

		//Get current node as node with lower f value in open list
		std::sort(open.begin(), open.end(), compare);

		currentRecord = open[0];

		//If current is goal, we have arrived!  
		if (currentRecord->node == goal) {

			//Work out path from current node record connection, untile arrived to start 
			
			while (currentRecord->node != start) {

				path.push_back(currentRecord->pathConnection);

				//Search start node record from current record connection
				
				NodeRecord* record;

				record = getStartRecord(closed, currentRecord->pathConnection);

				currentRecord = record;

			}

			std::reverse(path.begin(), path.end());

			return path;
		}

		//Otherwise, get node connections
		for (int i = 0; i < graph->connections[currentRecord->node->ID].size(); i++) {

			Connection* currentConnection = &graph->connections[currentRecord->node->ID][i];

			//End node
			Node* endNode = currentConnection->end;
			NodeRecord* endNodeRecord = new NodeRecord;

			//Cost estimate to end node in current connection
			float endNodeCost = currentRecord->costSoFar + currentConnection->cost;

			//If the node is closed...
			
			std::vector<NodeRecord*>::iterator recordClosedIndex = 
				std::find_if(closed.begin(), closed.end(), [endNode](NodeRecord* r) { return r->node == endNode; });

			std::vector<NodeRecord*>::iterator recordOpenIndex = 
				std::find_if(open.begin(), open.end(), [endNode](NodeRecord* r) { return r->node == endNode; });;

			

			if (recordClosedIndex !=closed.end())
			{
				endNodeRecord = *recordClosedIndex;

				//If this connection is not a shortest path to the end node, skip the end node
				if (endNodeRecord->costSoFar <= endNodeCost)
					continue;
				
				//Else, remove it from closed 
				closed.erase(recordClosedIndex);
			}


			//Else if the node is open..
	

			else if (recordOpenIndex != open.end()) {

				endNodeRecord = *recordOpenIndex;

				//If this is not a shortest path to the end node, skip it
				if (endNodeRecord->costSoFar <= endNodeCost)
					continue;

			}

				
			//Else, this is an unvisited node, so add a record for it in open list
			else {

				endNodeRecord->node = endNode;
				endNodeRecord->heuristic = heuristic(endNode, goal, map);
			}



							/*NODE UPDATES*/

			//Update cost from start to node
			endNodeRecord->costSoFar = endNodeCost;

			//Update path connection as current connection
			endNodeRecord->pathConnection = currentConnection;

			//If record not in open, add record to open
			if (recordOpenIndex == open.end())
				open.push_back(endNodeRecord);

		}

		
		//Connections analysis ended: add current node record in closed, and remove it from open

		open.erase(open.begin());
		closed.push_back(currentRecord);
	}

	delete currentRecord;

	return path;
}

std::queue<Tile*>* Agent::getPath()
{
	return &path;
}

AgentState Agent::getState()
{
	return state;
}

void Agent::setState(AgentState s)
{
	state = s;
}

//Heuclidean heuristic: distance between map tiles
double Agent::heuristic(Node * node, Node * goal, Map * map)
{
	Vector2D nodePos;
	Vector2D goalPos;


	for (int i = 0; i < map->tiles.size(); i++)
		for (int j = 0; j < map->tiles[i].size(); j++) {
			
			if (map->tiles[i][j].ID == node->ID) {

				nodePos = map->localize(&map->tiles[i][j]);
			}

			if (map->tiles[i][j].ID == goal->ID) {

				goalPos = map->localize(&map->tiles[i][j]);
			}
		}

	//Normalize distance between 0 and 10
	double maxDistance = sqrt(pow(map->getWidth(), 2.0) + pow(map->getHeigth(),2.0));
	double normalizedDistance = (nodePos.distance(goalPos) * 10) / maxDistance;

	return normalizedDistance;
}

Tile * Agent::getTile(Node * node, Map* map)
{
	for (int j = 0; j < map->tiles.size(); j++)
		for (int k = 0; k < map->tiles[j].size(); k++)
			if (map->tiles[j][k].ID == node->ID) {
				return &map->tiles[j][k];
			}

	return NULL;
}
