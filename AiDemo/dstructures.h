#pragma once
#include <vector>
#include <map>

class Vector2D
{
public:
	Vector2D();
	Vector2D(double x, double y);
	Vector2D operator+(const Vector2D addend);
	Vector2D operator*(const double mult);
	Vector2D operator/(const double div);
	double distance(Vector2D target);

	double x;
	double y;

};


//Directed graph

struct Connection;

class Node {
	

	public:
		int ID;
		Node();
		Node(int);
		

};


struct Connection {

	float cost;
	Node* start;
	Node* end;
};



class Graph {

	public:
		void addNode(int);
		void addConnection(int, float, Node*, Node*);

		std::map<int, Node> nodes;
		std::map<int, std::vector<Connection>> connections;
		
};


//Record with node info for search algorithm
struct NodeRecord {

	Node* node;

	float costSoFar = 0.0f; //g()
	float heuristic = 0.0f;//h()

	Connection* pathConnection;
};
