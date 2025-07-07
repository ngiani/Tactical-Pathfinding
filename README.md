**Tactical-Pathfinding** 🧽

A C++ demo of tactical pathfinding using influence maps and A* algorithm, complete with a basic UI built with the SenselessGameFramework.

## 📌 Overview

This project demonstrates a tactical pathfinding approach on a 2D grid. The agent selects routes by balancing physical distance and "influence" (e.g., dangerous or strategic zones), using a custom A* implementation.

## Key Features:

A* implementation with dynamic edge weights.

Influence map to model tactical cost of tiles.

Adjustable weighting: distance (heuristicWeight) and influence (influenceWeight).

Basic graphical UI using SenselessGameFramework.

## 🧱 Repository Structure
```
/
├── AiDemo/              # Visual Studio demo project
│   ├── agent.cpp/.h     # Agent + A* algorithm with influence
│   ├── map.cpp/.h       # Grid and influence representation
│   ├── dstructures.*    # Graph, Node, Connection structures
│   ├── GameObject.*     # UI/demo game objects
│   ├── main.cpp         # Entry point and demo loop
│   ├── AiDemo.sln/.vcxproj
│   ├── Fonts/, Textures/
│   └── … assets
└── .gitignore
```
## 🚀 Running the Demo

Clone the repository:

git clone https://github.com/ngiani/Tactical-Pathfinding.git
cd Tactical-Pathfinding/AiDemo

Open AiDemo.sln in Visual Studio (2017+).

Build in Debug/x64 configuration.

Run AiDemo.exe: the program will prompt for influenceWeight and heuristicWeight (values from 1 to 10).

Observe the agent navigating the grid, with a target selected via mouse input.

## ⚙️ Algorithm Details

A*: the optimal path is computed as cost-so-far (g) + heuristic (h) × heuristicWeight.

Tactical Influence: each tile has an influence value; edge cost = 1 + influenceWeight * normalized_influence.

Heuristic: Euclidean distance, normalized and scaled by heuristicWeight.

This enables tactical behavior: the agent can prefer safer or faster routes depending on parameter values.

## 📚 Dependencies
SDL library and
Visual Studio (2017 or newer) on Windows.

(No CMake/NuGet dependencies required.)
