/*
 * steiner_solution.cpp
 *
 *  Created on: Jul 14, 2008
 *      Author: pedro
 */

#include <assert.h>
#include <math.h>
#include <fstream>
#include <iostream>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/prim_minimum_spanning_tree.hpp>
#include <boost/graph/kruskal_min_spanning_tree.hpp>
#include <boost/graph/random.hpp>
#include <boost/progress.hpp>

#include "headers/steiner_solution.hpp"
using namespace std;

void SteinerSolution::find_mst_tree() {
	//find MST on sub_graph
	boost::timer timer;
	tree.clear();

	PredecessorMap p = boost::get(boost::vertex_predecessor, graph.boostgraph);

	boost::prim_minimum_spanning_tree(graph.boostgraph,	p);


	//compact
	/*
	int edges_removed = 0;
	list<Edge>::iterator iter = tree.begin();
	while (iter != tree.end()) {
		Edge e = *iter;

		Vertex u = boost::source(e, graph.boostgraph);
		Vertex v = boost::target(e, graph.boostgraph);

		bool is_u_lonely = boost::degree(u, graph.boostgraph) == 1 && !instance->is_terminal(u);
		bool is_v_lonely = boost::degree(v, graph.boostgraph) == 1 && !instance->is_terminal(v);

		//removing 1-degree non-terminal nodes
		if (is_u_lonely || is_v_lonely) {
			tree.erase(iter);
			graph.remove_edge(e);
			edges_removed++;
		} else {
			iter++;
		}


		if (is_u_lonely)
			graph.remove_vertex(u);

		if (is_v_lonely)
			graph.remove_vertex(v);
	}

	if (edges_removed > 0)
		cout << edges_removed << " edges lonely removed from solution. " << endl;

	*/

	cout << "mst computed in " << timer.elapsed() << " seconds." << endl;
}

void SteinerSolution::exchange_key_path() {
	//classify nodes
	//degree > 2 or termina = critical
	Edge e = boost::random_edge(graph.boostgraph, instance->rng);
	Vertex u, v;
	boost::tie(u, v) = boost::incident(e, graph.boostgraph);

	//find key path
}

int SteinerSolution::find_cost() {
	int total = 0;
	foreach(Edge e, tree) {
		total += graph.get_edge_weight(e);
	}
	return total;
}

struct show_events_visitor : boost::dijkstra_visitor<>
{
	Graph* graph;
	show_events_visitor(Graph& g) {
		graph = &g;
	}

  template<typename Vertex, typename Graph>
  void discover_vertex(Vertex v, const Graph&)
  {
    std::cerr << "on_discover_vertex(" << v << ", " << graph->index_for_vertex(v) << ")\n";
  }

  template<typename Vertex, typename Graph>
  void examine_vertex(Vertex v, const Graph&)
  {
  	std::cerr << "on_examine_vertex(" << v << ", " << graph->index_for_vertex(v) << ")\n";
  }

  template<typename Vertex, typename Graph>
    void finish_vertex(Vertex v, const Graph&)
    {
    	std::cerr << "on_finish_vertex(" << v << ", " << graph->index_for_vertex(v) << ")\n";
    }


};



void SteinerSolution::generate_chins_solution(SteinerSolution& solution) {

	//initialize list temp terminals (TODO: copy in random)
	list<int> terminals_left(solution.instance->terminals.size());
	copy(solution.instance->terminals.begin(), solution.instance->terminals.end(), terminals_left.begin());

	assert(terminals_left.size()> 0);

	list<int> vertices_in_solution;

	//pick initial terminal
	int t0 = terminals_left.front();
	terminals_left.pop_front();
	vertices_in_solution.push_back(t0);

	while (terminals_left.size() > 0) {
		//choose terminal closest to any of the vertices added
		int closest_distance = INT_MAX;
		int closest_terminal, closest_vertex_in_tree;
		foreach(int t, terminals_left) {

			vector<int> distances = solution.instance->distances_from_terminal[t]; //TODO: Confirm = does not copy
			vector<int> parents = solution.instance->parents_from_terminal[t];

			foreach(int v, vertices_in_solution) {
				int distance_to_v = distances[v];

				if(distance_to_v < closest_distance) {
					closest_distance = distance_to_v;
					closest_terminal = t;
					closest_vertex_in_tree = v;
				}
			}
		}

		terminals_left.remove(closest_terminal);

		assert(closest_distance != INT_MAX);
		/*
		 cout << "closest node to terminal " << closest_terminal << " is node " << closest_vertex_in_tree
		 << " [" << solution.instance->distances_from_terminal[closest_terminal][ closest_vertex_in_tree ]
		 << "]"
		 << " and path is : " << endl;
		 */

		//traverse shortest path from terminal
		int parent = closest_vertex_in_tree, last_parent = closest_vertex_in_tree;
		while(last_parent != closest_terminal) {
			//cout << parent << " - ";

			//add all vertices in this path
			bool not_in_solution = find(vertices_in_solution.begin(), vertices_in_solution.end(), parent)==vertices_in_solution.end();
			if(not_in_solution)
				vertices_in_solution.push_back(parent);

			last_parent = parent;
			parent = solution.instance->parents_from_terminal[closest_terminal][parent];
		};
		//cout << endl;
	}

	cout << "creating subgraph for solution..."<< endl;

	//create a sub graph structure with only the vertices added so far
	foreach(int v, vertices_in_solution) {

		Vertex vertex = solution.instance->graph.get_vertex(v);

		//bring any feasible adges (i.e. connecting with vertices in chins solution)
		boost::graph_traits<BoostGraph>::adjacency_iterator vi, viend;
		for (boost::tie(vi,viend) = boost::adjacent_vertices(vertex, solution.instance->graph.boostgraph); vi != viend; ++vi) {

			int index_neighboor = solution.instance->graph.index_for_vertex(*vi);

			//add only vertices that were considered (and avoid parallel edges)
			bool is_feasible = find(vertices_in_solution.begin(), vertices_in_solution.end(), index_neighboor)!=vertices_in_solution.end(); //TODO: can make it O(1)

			if(is_feasible && !solution.graph.contains_edge(v, index_neighboor)) {
				//add edge with same weight to this sub graph
				int weight = solution.instance->graph.get_edge_weight(vertex, *vi);
				solution.graph.add_edge(v, index_neighboor, weight);
			}
		}
	}

	cout << "chins subgraph created with " << solution.graph.num_edges() << " edges and " << solution.graph.num_vertices() << " vertices."  <<  endl;
	solution.graph.writedot("chins.dot");


	//find MST on sub_graph
	solution.find_mst_tree();
}

/* constructors */
SteinerSolution::SteinerSolution() {
	//TODO: understand alternative to value initialization
	cout << "SteinerSolution::SteinerSolution()" << endl;
}

SteinerSolution::SteinerSolution(Steiner* steiner) {
	cout << "SteinerSolution::SteinerSolution(Steiner* steiner)" << endl;

	instance = steiner;
	init();
}

SteinerSolution::SteinerSolution(const SteinerSolution& solution) {
	cout << "SteinerSolution::SteinerSolution(const SteinerSolution& solution)" << endl;

	instance = solution.instance;
	graph = Graph(solution.graph);

	//map edges from vector to the ones according to the new graph data structure
	foreach(Edge original_edge, solution.tree) {
		Edge e; bool found;
		Vertex u = boost::source(original_edge, solution.graph.boostgraph);
		Vertex v = boost::target(original_edge, solution.graph.boostgraph);

		Vertex nu = graph.get_vertex( solution.graph.index_for_vertex(u) );
		Vertex nv = graph.get_vertex( solution.graph.index_for_vertex(v) );

		boost::tie(e, found) = boost::edge(nu, nv, graph.boostgraph ); assert(found);
		tree.push_back(e);
	}

	assert(tree.size() == solution.tree.size());
	assert(boost::num_vertices(solution.graph.boostgraph) == boost::num_vertices(graph.boostgraph));
	assert(boost::num_edges(solution.graph.boostgraph) == boost::num_edges(graph.boostgraph));
	init();
}

void SteinerSolution::init() {
}

