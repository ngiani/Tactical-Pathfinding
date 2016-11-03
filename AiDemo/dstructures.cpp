#include "dstructures.h"


Vector2D::Vector2D() : x(0), y(0) {}

Vector2D::Vector2D(const double x, const double y) : x(x), y(y) {}

Vector2D Vector2D::operator+(const Vector2D addend) {

	Vector2D result;

	result.x = x + addend.x;
	result.y = y + addend.y;

	return result;
}

Vector2D Vector2D::operator*(const double mult) {

	Vector2D result;

	result.x = x * mult;
	result.y = y * mult;

	return result;
}

Vector2D Vector2D::operator/(const double div) {

	Vector2D result;

	result.x = x / div;
	result.y = y / div;

	return result;
}

double Vector2D::distance(Vector2D target) {

	return sqrt(pow(target.x - x, 2) + pow(target.y - y, 2));
}

void Graph::addNode(int ID) {

	nodes.insert(nodes.end(),std::pair<int, Node>(ID,Node(ID)));
}

void Graph::addConnection(int ID, float cost, Node* start, Node* end)
{

	std::vector<Connection> newNodeCons;

	Connection conn;
	
	conn.cost = cost;
	conn.start = start;
	conn.end = end;

	newNodeCons.push_back(conn);

	if (connections[ID].size() > 0)
		connections[ID].push_back(conn);
		
	else {
		connections[ID].push_back(conn);
	}
}


Node::Node(){}

Node::Node(int id) : ID(id){}
