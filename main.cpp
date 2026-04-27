In file included from main.cpp:7:
Graph.hpp:1:47: warning: missing terminating ' character
    1 | main.cpp:179:22: warning: missing terminating ' character
      |                                               ^
Graph.hpp:1:47: error: missing terminating ' character
    1 | main.cpp:179:22: warning: missing terminating ' character
      |                                               ^~~~~~~~~~~
Graph.hpp:2:30: warning: missing terminating ' character
    2 |   179 | >>>>>>> a1e0730 (Prim's Algorithem Added)
      |                              ^
Graph.hpp:2:30: error: missing terminating ' character
    2 |   179 | >>>>>>> a1e0730 (Prim's Algorithem Added)
      |                              ^~~~~~~~~~~~~~~~~~~~
Graph.hpp:4:45: warning: missing terminating ' character
    4 | main.cpp:179:22: error: missing terminating ' character
      |                                             ^
Graph.hpp:4:45: error: missing terminating ' character
    4 | main.cpp:179:22: error: missing terminating ' character
      |                                             ^~~~~~~~~~~
Graph.hpp:5:30: warning: missing terminating ' character
    5 |   179 | >>>>>>> a1e0730 (Prim's Algorithem Added)
      |                              ^
Graph.hpp:5:30: error: missing terminating ' character
    5 |   179 | >>>>>>> a1e0730 (Prim's Algorithem Added)
      |                              ^~~~~~~~~~~~~~~~~~~~
Graph.hpp:8:24: error: extended character ‘ is not valid in an identifier
    8 | Graph.hpp:44:1: error: ‘git’ does not name a type
      |                        ^
Graph.hpp:8:24: error: extended character ’ is not valid in an identifier
Graph.hpp:12:31: error: extended character ‘ is not valid in an identifier
   12 | Graph.tpp: In member function ‘int Graph<T>::getVertexIndex(const T&) const’:
      |                               ^
Graph.hpp:12:71: error: extended character ’ is not valid in an identifier
   12 | Graph.tpp: In member function ‘int Graph<T>::getVertexIndex(const T&) const’:
      |                                                                       ^
Graph.hpp:1:1: error: ‘main’ does not name a type
    1 | main.cpp:179:22: warning: missing terminating ' character
      | ^~~~
main.cpp:31:59: error: ‘Graph’ has not been declared
   31 | void printPathsInfo(const std::vector<std::string>& path, Graph<std::string>& g) { //print format for shortest paths to state
      |                                                           ^~~~~
main.cpp:31:64: error: expected ‘,’ or ‘...’ before ‘<’ token
   31 | void printPathsInfo(const std::vector<std::string>& path, Graph<std::string>& g) { //print format for shortest paths to state
      |                                                                ^
main.cpp: In function ‘void printPathsInfo(const std::vector<std::__cxx11::basic_string<char> >&, int)’:
main.cpp:39:28: error: ‘g’ was not declared in this scope
   39 |             auto weights = g.getEdgeWeights(path[i], path[i+1]);
      |                            ^
main.cpp: In function ‘int main()’:
main.cpp:48:5: error: ‘Graph’ was not declared in this scope; did you mean ‘isgraph’?
   48 |     Graph<std::string> airportGraph;
      |     ^~~~~
      |     isgraph
main.cpp:48:22: error: expected primary-expression before ‘>’ token
   48 |     Graph<std::string> airportGraph;
      |                      ^
main.cpp:48:24: error: ‘airportGraph’ was not declared in this scope
   48 |     Graph<std::string> airportGraph;
      |                        ^~~~~~~~~~~~
main.cpp:61:41: error: ‘parseCSVLine’ was not declared in this scope
   61 |         std::vector<std::string> cols = parseCSVLine(line);