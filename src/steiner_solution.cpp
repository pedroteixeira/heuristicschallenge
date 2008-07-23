/*
 * steiner_solution.cpp
 *
 *  Created on: Jul 14, 2008
 *      Author: pedro
 */

#include <assert.h>
#include <math.h>
#include <iostream>
#include <stack>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/prim_minimum_spanning_tree.hpp>
#include <boost/graph/kruskal_min_spanning_tree.hpp>
#include <boost/graph/random.hpp>

#include <boost/progress.hpp>

#include "headers/steiner_solution.hpp"

using namespace std;

void SteinerSolution::find_mst_tree() {
	boost::timer timer;
	tree.clear();

	//run prim's
	PredecessorMap p = boost::get(boost::vertex_predecessor, graph.boostgraph);
	boost::prim_minimum_spanning_tree(graph.boostgraph, p);

	//convert from parent map to list of edges
	foreach(Vertex v, boost::vertices(graph.boostgraph)) {
		if(v != p[v])
		tree.push_back(boost::edge(v, p[v], graph.boostgraph).first);
	}

	//eliminate all edges in graph that are not in tree (beware of invalidation of edges desccriptors)
	//TODO: bad to elimated because it's our working structure (to enabled search, etc..)
	/*
	int edges_removed = 0;
	boost::graph_traits<BoostGraph>::edge_iterator ei, ei_end, next;
	boost::tie(ei, ei_end) = boost::edges(graph.boostgraph);
	for(next = ei; ei != ei_end; ei = next) {
		++next;
		if(find(tree.begin(), tree.end(), *ei) == tree.end()) {
			graph.remove_edge(*ei); edges_removed++;
		}
	}
	cout << edges_removed << " edges eliminated from graph to match solution tree." << endl;
	*/

	//compact
	int edges_removed = 0;
	list<Edge>::iterator iter = tree.begin();
	while (iter != tree.end()) {
		Edge e = *iter;

		Vertex u = boost::source(e, graph.boostgraph);
		Vertex v = boost::target(e, graph.boostgraph);
		int iu = graph.index_for_vertex(u);
		int iv = graph.index_for_vertex(v);

		bool is_u_lonely = boost::degree(u, graph.boostgraph) == 1 && !instance->is_terminal(iu);
		bool is_v_lonely = boost::degree(v, graph.boostgraph) == 1 && !instance->is_terminal(iv);

		//removing 1-degree non-terminal nodes
		if (is_u_lonely || is_v_lonely) {
			tree.erase(iter);
			//graph.remove_edge(e); TODO: Keep structure intact for now?
			edges_removed++;
		} else {
			iter++;
		}

		/*
		if (is_u_lonely)
			graph.remove_vertex(u);

		if (is_v_lonely)
			graph.remove_vertex(v);
			*/
	}

	if (edges_removed> 0)
	cout << edges_removed << " edges lonely removed from solution tree. " << endl;

	cout << "mst computed and graph compacted in " << timer.elapsed() << " seconds." << endl;
}

class dfs_time_visitor: public boost::default_dfs_visitor {

public:
	dfs_time_visitor(Graph& g, VertexSet& cs, list<list<Vertex> >& paths) :
		graph(g), critical_set(cs), critical_hashed(cs.get<1> ()), on_key_path(false), keypaths(paths) {
	}

	void discover_vertex(Vertex u, const BoostGraph & g) {

		bool is_critical = critical_hashed.find(u) != critical_hashed.end();

		if (is_critical) {
			if (on_key_path) {
				//end current path
				path.push_back(u);
				list<Vertex> tmp = path;
				keypaths.push_back(tmp);
				path.clear();
			} else {
				//start new from here
				on_key_path = true;
			}
			//add key node to current path
			path.push_back(u);
			critical_stack.push(u);
		} else if (on_key_path) {
			//add seiner node to current path
			path.push_back(u);
		}
	}

	void examine_edge(Edge e, const BoostGraph & g) {
	}

	void finish_vertex(Vertex u, const BoostGraph & g) {
		//update track
		bool is_critical = critical_hashed.find(u) != critical_hashed.end();
		if (is_critical) {
			path.clear();
			//throw away finished
			if (!critical_stack.empty())
				critical_stack.pop();
			//and restore previous critical (to deal with branches)
			if (!critical_stack.empty()) {
				path.push_back(critical_stack.top());
				on_key_path = true;
			} else {
				on_key_path = false; //I guess only necessary if graph is disconnected
			}
		}
	}

	Graph& graph;
	VertexSet& critical_set;
	VertexHashSet& critical_hashed;
	list<Vertex> path;
	list<list<Vertex> >& keypaths;
	stack<Vertex> critical_stack;
	bool on_key_path;
};

void SteinerSolution::exchange_key_path() {

	VertexSet critical_set;

	//TODO: the following can be cached or done in pre-processing
	//classify nodes
	foreach(Vertex u, boost::vertices(graph.boostgraph)) {
		int iu = graph.index_for_vertex(u);
		if(boost::degree(u, graph.boostgraph)> 2 || instance->is_terminal(iu)) //is critical
		critical_set.push_back(u);
	}
	assert(critical_set.size()> 0);
	cout << critical_set.size() << " critical nodes found. \n";

	//build all key paths
	list< list<Vertex> > keypaths;
	dfs_time_visitor vis(graph, critical_set, keypaths);

	boost::depth_first_search(graph.boostgraph, boost::visitor(vis));

	cout << keypaths.size() << " key-node paths built: \n";
	foreach(list<Vertex> vertices, keypaths) {
		int path_weight = 0; Vertex previous;
		for(list<Vertex>::iterator iter = vertices.begin(); iter != vertices.end(); iter++) {
			if(iter != vertices.begin()) {
				Edge e; bool found;
				boost::tie(e, found) = boost::edge(previous, *iter, graph.boostgraph);
				assert(found);

				path_weight += graph.get_edge_weight(e);
			}
			previous = *iter;
			//cout << graph.index_for_vertex(*iter) << " - ";
		}
		//cout << "[" << path_weight << "] \n";

		//TODO: cache distances
		DistanceMap distances;
		PredecessorMap parents;
		graph.dijkstra_shortest_paths(vertices.front(), distances, parents);
		int best_distance = distances[vertices.back()];

		//		Vertex parent = vertices.back(), last_parent = parent, first = vertices.front();
		//		while(last_parent != first) {
		//			cout << graph.index_for_vertex(parent) << " - ";
		//			last_parent = parent;
		//			parent = parents[parent];
		//		}
		//		cout << "\n";

		if(best_distance < path_weight) {
			cout << "best distance FOUND is " << best_distance << " over " << path_weight << ".\n";
		}
	}

	//do exchange

	cout << endl;
}


/**
 * mantain out vertices
 */
bool SteinerSolution::is_new_candidate_for_out_vertex(int i) {

	bool is_already_in_solution = graph.contains_vertex(i);
	if(is_already_in_solution) //excludes terminals also
		return false;

	bool already_added_as_candidate = find(out_vertices.begin(), out_vertices.end(), i) != out_vertices.end(); //TODO: can make it O(1) with hashed index
	if(already_added_as_candidate)
		return false;

	//check if it can connect to at least 2 neighboors in this solution
	int neighboors_connected = 0;
	foreach(Vertex n_in_original, boost::adjacent_vertices(instance->graph.get_vertex(i), instance->graph.boostgraph)) {

		int index_n = instance->graph.index_for_vertex(n_in_original);
		bool n_exists_in_solution = graph.contains_vertex(index_n);

		if(n_exists_in_solution && boost::degree(graph.get_vertex(index_n), graph.boostgraph) > 0)
			neighboors_connected++;
	}

	return neighboors_connected > 1;
}


void SteinerSolution::update_candidates_out_vertices(int i) {

	foreach(Vertex n, boost::adjacent_vertices(instance->graph.get_vertex(i), instance->graph.boostgraph)) {
		int index_n = instance->graph.index_for_vertex(n);
		if (is_new_candidate_for_out_vertex(index_n)) {
			out_vertices.push_back(index_n);
		} else {
			out_vertices.remove(index_n);
		}
	}
}


void SteinerSolution::build_candidates_out_vertices() {
	out_vertices.clear();

	//select those nodes when added would have edges to connect to solution
	for(int i=0; i<instance->V; i++) {
		if (is_new_candidate_for_out_vertex(i)) {
			out_vertices.push_back(i);
		}
	}

	cout << "built list of " << out_vertices.size() << " candidates for 'out vertices'." << endl;
}


void SteinerSolution::on_steiner_node_inserted(int i) {
	out_vertices.remove(i);
	update_candidates_out_vertices(i);

}

void SteinerSolution::on_steiner_node_removed(int i) {
	out_vertices.push_back(i);
	update_candidates_out_vertices(i);
}

/**
 * Find cost of current solution.
 */
int SteinerSolution::find_cost() {
	int total = 0;
	foreach(Edge e, tree) {
		total += graph.get_edge_weight(e);
	}
	return total;
}



/**
 * Heuristics to generate good solution.
 */
void SteinerSolution::generate_chins_solution(SteinerSolution& solution) {

	//initialize list temp terminals (TODO: copy in random)
	list<int> terminals_left(solution.instance-> terminals.size());
	std::copy(solution.instance->terminals.begin(), solution.instance->terminals.end(), terminals_left.begin());

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
			//cout << parent << "  ";

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

	cout << "chins subgraph created with " << solution.graph.num_edges() << " edges and " << solution.graph.num_vertices() << " vertices." << endl;

	//trim and find MST on sub_graph
	solution.find_mst_tree();
}


/**
 * constructors
 * */

SteinerSolution::SteinerSolution(Steiner* steiner) {
	cout << "SteinerSolution::SteinerSolution(Steiner* steiner)" << endl;

	instance = steiner;
	init();
}

SteinerSolution::SteinerSolution(const SteinerSolution& solution) {
	copy(solution, *this);
}

SteinerSolution& SteinerSolution::operator = ( const SteinerSolution& source ) {
	if(this != &source) {
		copy(source, *this);
	}
	return *this;
}

void SteinerSolution::copy(const SteinerSolution& source, SteinerSolution& to) {
	//cout << "copying SteinerSolution" << endl;

	to.instance = source.instance;
	to.out_vertices = source.out_vertices;
	to.graph = source.graph; //deep copy

	//map edges from solution tree to the ones according to the new graph data structure
	to.tree.clear();
	for (list<Edge>::const_iterator iter = source.tree.begin(); iter != source.tree.end(); iter++) {

		Vertex u = boost::source(*iter, source.graph.boostgraph);
		Vertex v = boost::target(*iter, source.graph.boostgraph);

		int iu = source.graph.index_for_vertex(u);
		int iv = source.graph.index_for_vertex(v);
		Vertex nu = to.graph.get_vertex(iu);
		Vertex nv = to.graph.get_vertex(iv);

		Edge e;
		bool found;
		boost::tie(e, found) = boost::edge(nu, nv, to.graph.boostgraph );
		assert(found);
		to.tree.push_back(e);
	}

	assert(to.tree.size() == source.tree.size());
	assert(boost::num_vertices(source.graph.boostgraph) == boost::num_vertices(to.graph.boostgraph));
	assert(boost::num_edges(source.graph.boostgraph) == boost::num_edges(to.graph.boostgraph));
}

void SteinerSolution::init() {
}

