/*
 * key_nodesearch.cpp
 *
 *  Created on: Jul 22, 2008
 *      Author: pedro
 */

#include <boost/graph/connected_components.hpp>
#include <boost/property_map.hpp>
#include <boost/format.hpp>

#include "headers/steiner_nodesearch.hpp"
#include "headers/steiner_solution.hpp"
#include "headers/graph.hpp"

using namespace std;

void SteinerNodeLocalSearch::remove(SteinerSolution& solution) {
	if (solution.in_key_nodes.size() == 0)
		return;

	cout << solution.in_key_nodes.size() << " steiner nodes candidates for removal.\n";

	//try all out vertices (randomize)
	vector<int> tmp_vertices(solution.in_key_nodes.size());
	foreach(int v, solution.in_key_nodes) {
		tmp_vertices.push_back(v);
	}
	random_shuffle(tmp_vertices.begin(), tmp_vertices.end());

	int best_cost = INT_MAX, cost;
	int best_vertex_to_remove;
	foreach(int i, tmp_vertices) {
		if(solution.in_key_nodes.get<1>().find(i) == solution.in_key_nodes.get<1>().end())
		continue; //graph might have changed its in_key_nodes candidate list


		remove_key_node(i, solution);

		connect_graph(i, solution);

		solution.find_mst_tree();
		cost = solution.find_cost();

		if (cost < best_cost) {
			best_cost = cost;
			best_vertex_to_remove = i;
		}

		//undo insert
		insert_key_node(i, solution);
		solution.undo_last_mst();

	}

	//keep best
	if(best_cost != INT_MAX) {
		cout << "removing best steiner node of this round " << best_vertex_to_remove << " yield cost " << best_cost << "\n";
		remove_key_node(best_vertex_to_remove, solution);
		connect_graph(best_vertex_to_remove, solution);
		solution.find_mst_tree();
	}

}



void SteinerNodeLocalSearch::connect_graph(int vertex_removed, SteinerSolution& solution) {
	//find out how many disconnected components we made
	DistanceMap components;
	int num = boost::connected_components(solution.graph.boostgraph, components);
	cout << "steiner node " << vertex_removed << " was removed and yielded " << num << " components.\n";

	if (num == 1) {
		return;
	}

	//try cheap first
	solution.grow_graph();
	num = boost::connected_components(solution.graph.boostgraph, components);
	if(num == 1) {
		cout << "quick grow was enough to re-connect.\n";
		return;
	}

	cout << "after quick grow there were left " << num << " components.\n";

	map<int, list<Vertex> > vertices_by_component;
	for (int j = 0; j < num; j++)
		vertices_by_component.insert(make_pair(j, list<Vertex> ()));

	foreach(Vertex v, boost::vertices(solution.graph.boostgraph)) {
		//TODO: any heuristics to make this set smaller?
		vertices_by_component[components[v]].push_back(v);
	}

	//find out best ways to connect components
	map< int, map<int, WayToConnect> > best_way_to_connect;

	for(int j=0;j<num;j++) {
		best_way_to_connect.insert( make_pair(j, map<int, WayToConnect>()) );

		foreach(Vertex u, vertices_by_component[j]) {
			vector<int> distances, parents;
			int from = solution.graph.index_for_vertex(u);
			boost::tie(distances, parents) = solution.instance.get_shortest_distances( from );

			for(int l=j+1;l<num;l++) {
				int shortest_distance_to_connect = INT_MAX;
				best_way_to_connect[j].insert( make_pair(l, WayToConnect() ));

				foreach(Vertex v, vertices_by_component[l]) {
					int to = solution.graph.index_for_vertex(v);
					if(distances[to] < shortest_distance_to_connect) {

						shortest_distance_to_connect = distances[to];
						best_way_to_connect[j][l].shortest_distance = distances[to];
						best_way_to_connect[j][l].to_vertex = to;
						best_way_to_connect[j][l].from_vertex = from;
					}
				}
			}
		}
	}


	//connect components via best paths
	list<int> disconnected;
	for(int j=0;j<num;j++) {
		disconnected.push_back(j);
	}

	while(disconnected.size()> 0) {

		int component = disconnected.front(); disconnected.pop_front();

		int shortest_distance = INT_MAX, to_component; WayToConnect best_way;
		for(int j=0;j<num;j++) {
			if(j == component) continue;
			int from = min(j, component), to = max(j, component);
			WayToConnect best_way_for_these = best_way_to_connect[from][to];
			if(best_way_for_these.shortest_distance < shortest_distance) {
				best_way = best_way_for_these;
				to_component = j;
			}
		}

		disconnected.remove( to_component );

		vector<int> distances, parents;
		boost::tie(distances, parents) = solution.instance.get_shortest_distances( best_way.from_vertex );

		cout << "connecting components " << component << " and " << to_component << " from "
						<< best_way.from_vertex << " to " << best_way.to_vertex << " with cost " << best_way.shortest_distance << "\n";


		solution.add_path(best_way.from_vertex, best_way.to_vertex, parents);

		boost::tie(distances, parents) = solution.instance.get_shortest_distances( vertex_removed );
		cout << "distance from removed steiner " << vertex_removed << " to " << best_way.from_vertex;
		cout << " " << distances[best_way.from_vertex];
		cout << " and to " << best_way.to_vertex << " " << distances[best_way.to_vertex] << "\n";


	}

	cout << endl;
	num = boost::connected_components(solution.graph.boostgraph, components);
	assert(num == 1);

}

void SteinerNodeLocalSearch::insert(SteinerSolution& solution) {

	if (solution.out_key_nodes.size() == 0)
		return; //Nothing todo, really?! TODO: return false?

	//try all out vertices (randomize)
	vector<int> tmp_vertices(solution.out_key_nodes.size());
	foreach(int v, solution.out_key_nodes) {
		tmp_vertices.push_back(v);
	}
	random_shuffle(tmp_vertices.begin(), tmp_vertices.end());

	int best_cost = INT_MAX, cost;
	int best_vertex_to_insert;
	foreach(int i, tmp_vertices) {
		if(!solution.is_out_key_node(i))
			continue; //graph might have changed its out_key_nodes candidate list

		if(insert_key_node(i, solution)) {
			solution.find_mst_tree();
			cost = solution.find_cost();
			if (cost < best_cost) {
				best_cost = cost;
				best_vertex_to_insert = i;
			}

			//undo insert
			remove_key_node(i, solution);
			solution.undo_last_mst();
		}
	}

	//keep best
	if(best_cost != INT_MAX) {
		insert_key_node(best_vertex_to_insert, solution);
		solution.find_mst_tree();
	}

}

void SteinerNodeLocalSearch::remove_key_node(int i, SteinerSolution& solution) {
	solution.graph.remove_vertex(i);
	solution.on_key_node_removed(i);
}

bool SteinerNodeLocalSearch::insert_key_node(int i, SteinerSolution& solution) {

	Vertex v_original = solution.instance.graph.get_vertex(i);

	//add new edges (from original graph) to this graph
	bool at_least_one_edge_added = false;
	foreach(Vertex n_original, boost::adjacent_vertices(v_original, solution.instance.graph.boostgraph)) {

		int index_n = solution.instance.graph.index_for_vertex(n_original);
		bool n_exists_in_solution = solution.graph.contains_vertex(index_n);
		if(!n_exists_in_solution)
		continue;

		Vertex n = solution.graph.get_vertex(index_n);

		//only add if it's connected to solution tree (at least two edges)
		if (n_exists_in_solution && boost::out_degree(n, solution.graph.boostgraph)> 0) {
			//cout << "adding steiner edge " << i << ", "<< index_n << "\n";
			solution.add_edge_from_original(i, index_n);
			at_least_one_edge_added = true;
		}
	}

	solution.on_key_node_inserted(i);

	return at_least_one_edge_added;

}

