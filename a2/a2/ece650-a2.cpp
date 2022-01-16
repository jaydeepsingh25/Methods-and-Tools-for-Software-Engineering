// Compile with c++ ece650-a2cpp -std=c++11 -o ece650-a2
#include <algorithm>
#include <iostream>
#include <iterator>
#include <queue>
#include <regex>
#include <sstream>
#include <string>

using namespace std;

// reset all values in the graph
void resetGraph(vector<vector<bool>> &matrix) {

  int vertices = (int)matrix.size();
  // Set all values in the matrix to false
  for (int row = 0; row < vertices; row++) {
    for (int col = 0; col < vertices; col++) {
      matrix[row][col] = false;
    }
  }
}

// construct the edges in the graph matrix according to the specified input
void constructEdges(string inputEdges, vector<vector<bool>> &matrix) {
  int size = (int)matrix.size();
  // 'r' in the string 'inputEdges'. 'm' is flag for determining matching
  // behavior.
  regex r("<.*?>");
  smatch m;
  bool invalidVertex = false;
  for (sregex_iterator iterator =
           sregex_iterator(inputEdges.begin(), inputEdges.end(), r);
       iterator != sregex_iterator(); iterator++) {
    smatch match;
    match = *iterator;
    string edges;
    edges = match.str(0);
    for (int i = 0; i < match.length(); i++) {
      // traverse through each edge to extract vertices
      string edge = match[i];
      int edge_array[2] = {-1, -1};
      int vertex_index = 0;
      if (edge.length() > 0) {
        string edge_vertices = edge.substr(1, edge.length() - 2);
        char *edges_pointer = &edge_vertices[0];

        char *token = strtok(edges_pointer, ",");

        while (token != NULL) {
          edge_array[vertex_index] = stoi(token);
          vertex_index++;
          token = strtok(NULL, ",");
        }
        /**
         * Create an entry from i to j and j to i in the adjacency matrix to
         * denote edge
         */
        if ((edge_array[0] >= 0 && edge_array[1] >= 0) &&
            (edge_array[0] < size && edge_array[1] < size &&
             (edge_array[0] != edge_array[1]))) {
          matrix[edge_array[0]][edge_array[1]] = true;
          matrix[edge_array[1]][edge_array[0]] = true;
        } else {
          invalidVertex = true;
          break;
        }
      }
    }
  }
  if (invalidVertex) {
    cout << "Error: Invalid Edge specified" << endl;
    // Remove all the edges in case of invalid input edges
    resetGraph(matrix);
  }
};

// initialise the graph matrix with the number of vertices
void initialiseGraph(int vertices, vector<vector<bool>> &matrix) {
  int numVertices = (int)matrix.size();
  // Resize the matrix to size vertex * vertex
  for (int i = 0; i < numVertices; i++)
    matrix[i].resize(vertices);
  matrix.resize(vertices, vector<bool>(vertices));

  // Set all values in the matrix to false
  for (int row = 0; row < vertices; row++) {
    for (int col = 0; col < vertices; col++) {
      matrix[row][col] = false;
    }
  }
};

// computeShortestPath calculates shortest path between startNode and endNode
// using BFS
void computeShortestPath(int startNode, int endNode,
                         vector<vector<bool>> &matrix) {
  int numVertices = (int)matrix.size();
  if (startNode < numVertices && endNode < numVertices) {
    queue<int> q;
    // array to keep track if a vertex has been visited or not
    vector<int> visited(numVertices, false);
    // array to keep track of previous node, prev[i] indicates the parent node
    // of node i
    vector<int> prev(numVertices, -1);

    // push the source node in the queue
    q.push(startNode);

    visited[startNode] = true;

    while (q.size() != 0) {
      // get the element at the start of the queue
      int node = q.front();
      // remove the element from queue
      q.pop();
      for (int col = 0; col < (int)matrix[node].size(); col++) {
        if (matrix[node][col] == 1 && !visited[col]) {
          q.push(col);
          visited[col] = true;
          prev[col] = node;
        }
      }
    }
    // now, prev array can be used to construct the path from startNode to
    // endNode
    int pathNode = endNode;
    vector<int> path;
    while (pathNode != -1) {
      path.push_back(pathNode);
      pathNode = prev[pathNode];
    }
    reverse(path.begin(), path.end());
    ostringstream outputStream;
    if (path[0] == startNode) {
      copy(path.begin(), path.end() - 1,
           ostream_iterator<int>(outputStream, "-"));
      outputStream << path.back();
      cout << outputStream.str() << endl;
    } else {
      cout << "Error: No path exists between vertex " << startNode << " and "
           << endNode << endl;
    }
  } else {
    cout << "Error: Invalid vertex specified" << endl;
  }
};

int main(int argc, char **argv) {

  static int vertices = 10;

  while (!cin.eof()) {
    string inputLine;
    getline(std::cin, inputLine);

    vector<string> inputArgs;
    istringstream inputStream(inputLine);
    string value = "";
    while (inputStream >> value) {
      inputArgs.push_back(value);
    }
    char command = inputArgs.at(0)[0];
    // initialize adjacency matrix as 10*10, would be overriden by V command
    static vector<vector<bool>> matrix(vertices, vector<bool>(vertices));
    string edges = "";

    switch (command) {
    case 'E':
      if (inputArgs.size() != 2) {
        cout << "Error: Invalid arguments for E command" << endl;
      } else {
        edges = inputArgs.at(1);
        constructEdges(edges, matrix);
      }
      break;
    case 'V':
      if (inputArgs.size() != 2) {
        cout << "Error: Invalid arguments for V command" << endl;
      } else {
        vertices = stoi(inputArgs.at(1));
        initialiseGraph(vertices, matrix);
      }
      break;
    case 's':
      if (inputArgs.size() != 3) {
        cout << "Error: Invalid arguments for s command" << endl;
      } else {
        computeShortestPath(stoi(inputArgs.at(1)), stoi(inputArgs.at(2)),
                            matrix);
      }
      break;
    // case 'p':
    //     for (int i = 0; i < (int)matrix.size(); i++)
    //     {
    //         for (int j = 0; j < (int)matrix[i].size(); j++)
    //         {
    //             cout << matrix[i][j] << " ";
    //         }
    //         cout << endl;
    //     };
    //     break;
    default:
      cout << "Error: Command not supported" << endl;
      break;
    }
  };
}
