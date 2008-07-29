/*
 * steiner_pathsearch.cpp
 *
 *  Created on: Jul 26, 2008
 *      Author: pedro
 */

#include "headers/steiner_pathsearch.hpp"
#include "headers/steiner_solution.hpp"
#include "headers/graph.hpp"
#include <stack>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/undirected_dfs.hpp>
#include <boost/graph/graph_utility.hpp>

using namespace std;

class dfs_time_visitor: public boost::default_dfs_visitor {

public:
	dfs_time_visitor(Graph& g, IntSet& cs, list<list<int> >& paths) :
		graph(g), critical_set(cs), critical_hashed(cs.get<1> ()), on_key_path(false), keypaths(paths) {
	}

	void discover_vertex(Vertex u, const BoostGraph & g) {

		int iu = graph.index_for_vertex(u);
		bool is_critical = critical_hashed.find(iu) != critical_hashed.end();

		if (is_critical) {
			if (on_key_path) {
				//end current path
				path.push_back(iu);
				list<int> tmp = path;
				keypaths.push_back(tmp);
				path.clear();

				//cout << "starting new path from key-node " << graph.index_for_vertex(u) << "\n";
			} else {
				//start new from here
				on_key_path = true;
				//cout << "on key path \n";
			}
			//add key node to current path
			//cout << "adding key-node " << graph.index_for_vertex(u) <<"\n";


			path.push_back( iu );
			critical_stack.push( iu);
		}
		else if (on_key_path) {

			//cout << "adding steiner " << graph.index_for_vertex(u) <<"\n";
			//add seiner node to current path
			path.push_back(iu);
		} else {

			//cout << "ignore steiner node off path " << graph.index_for_vertex(u) <<"\n";

		}
	}

	void finish_vertex(Vertex u, const BoostGraph & g) {

		//update track
		int iu = graph.index_for_vertex(u);
		bool is_critical = critical_hashed.find(iu) != critical_hashed.end();

		if (is_critical) {
			//throw away finished
			if (!critical_stack.empty())
				critical_stack.pop();
			//cout << "finished key-node " << graph.index_for_vertex(u) <<"\n";
		}

		if(is_critical || graph.get_degree(u) == 1) {
			path.clear();
			//and restore previous critical (to deal with branches)
			if (!critical_stack.empty()) {
				path.push_back(critical_stack.top());
				on_key_path = true;

				//cout << "keep as current key-node " << graph.index_for_vertex(critical_stack.top()) <<"\n";
			} else {
				on_key_path = false; //I guess only necessary if graph is disconnected

				//cout << "off key path\n";
			}
		}
	}

	Graph& graph;
	IntSet& critical_set;
	IntHashSet& critical_hashed;
	list<int> path;
	bool on_key_path;
	list<list<int> >& keypaths;
	stack<int> critical_stack;

};

void SteinerPathLocalSearch::search(SteinerSolution& solution) {

	IntSet critical_set;

	//TODO: the following can be cached or done in pre-processing
	//classify nodes
	foreach(Vertex u, boost::vertices(solution.graph.boostgraph)) {
		int iu = solution.graph.index_for_vertex(u);
		if(boost::degree(u, solution.graph.boostgraph)> 2 || solution.is_terminal(iu)) //is critical
		critical_set.push_back(iu);
	}
	assert(critical_set.size()> 0);

	//build all key paths for this solution
	list< list<int> > keypaths;
	dfs_time_visitor vis(solution.graph, critical_set, keypaths);

	boost::undirected_dfs(solution.graph.boostgraph, boost::root_vertex(*boost::vertices(solution.graph.boostgraph).first)
						.visitor(vis)
	           .edge_color_map(boost::get(boost::edge_color, solution.graph.boostgraph)));

	//cout << keypaths.size() << " key-node paths were found.\n";

	int improved = 0;
	foreach(list<int> keypath, keypaths) {

		//calculate weight for this path
		int path_weight = 0;
		int previous;
		for(list<int>::iterator iter = keypath.begin(); iter != keypath.end(); iter++) {
			if(iter != keypath.begin()) {
				if(!solution.graph.contains_edge(previous, *iter)) {
					cerr << "is graph disconected? " << previous << ", " << *iter << "\n";
				} else {
					path_weight += solution.graph.get_edge_weight(*iter, previous);
				}
			}
			previous = *iter;
		}



		//find shortest distance between two critical nodes and compare
		vector<int> distances, parents;
		int from = keypath.front();
		int to = keypath.back();
		boost::tie(distances, parents) = solution.instance.get_shortest_distances(from);
		int best_distance = distances[to];

		if(best_distance < path_weight) {
			//cout << "!! best distance FOUND is " << best_distance << " over " << path_weight << ".\n";

			exchange_path(solution, keypath, parents);
			solution.find_mst_tree();

			improved++;
			//break;
		}
	}

	if(improved > 0)
		cout << improved << " paths \n";

	//cout << endl;
}

void SteinerPathLocalSearch::exchange_path(SteinerSolution& solution, list<int>& keypath, vector<int>& shortestpath) {


	//remove old path
	//cout << "removing old path: ";
	foreach(int v, keypath) {
		//cout << solution.graph.index_for_vertex(v) << ", ";
		if(v != keypath.front() && v != keypath.back()) {
			if(solution.graph.get_degree(v) > 2) {
				cerr << "something went wrong - non key-node with degree > 2 " << v << "\n";
				solution.graph.writedot("oddkeynodepath.dot");
				assert(false);
			}
			solution.graph.remove_vertex(v);
		}
	}

	//cout << "\n";
	//add new (better) path
	//cout << "adding new path: ";
	solution.add_path(keypath.front(), keypath.back(), shortestpath);


}
