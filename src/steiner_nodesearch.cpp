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
	//cout << "steiner node " << vertex_removed << " was removed and yielded " << num << " components.\n";

	if (num == 1) {
		return;
	}

	//try cheap first  //TODO: understand different and randomize usage
	solution.grow_graph();
	num = boost::connected_components(solution.graph.boostgraph, components);
	if (num == 1) {
		//cout << "quick grow was enough to re-connect.\n";
		return;
	}

	//cout << "after quick grow there were left " << num << " components.\n";

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

		solution.add_path(best_way.from_vertex, best_way.to_vertex, parents);

		/*
		 cout << "connecting components " << component << " and " << to_component << " from "
		 << best_way.from_vertex << " to " << best_way.to_vertex << " with cost " << best_way.shortest_distance << "\n";


		 boost::tie(distances, parents) = solution.instance.get_shortest_distances( vertex_removed );
		 cout << "distance from removed steiner " << vertex_removed << " to " << best_way.from_vertex;
		 cout << " " << distances[best_way.from_vertex];
		 cout << " and to " << best_way.to_vertex << " " << distances[best_way.to_vertex] << "\n";
		 */

	}

	num = boost::connected_components(solution.graph.boostgraph, components);
	assert(num == 1);

}

void SteinerNodeLocalSearch::insert(SteinerSolution& solution) {

	vector<int> candidates_steiner_nodes;
	foreach(Vertex u, boost::vertices(solution.instance.graph.boostgraph)) {
		int iu = solution.instance.graph.index_for_vertex(u);
		if(!solution.graph.contains_vertex(iu)) {
			//restrict more //theses nodes seem more interesting (better heuristic??)
			if(boost::degree(u, solution.instance.graph.boostgraph) > 2)
				candidates_steiner_nodes.push_back(iu);
		}
	}

	cout << candidates_steiner_nodes.size() << " candidates steiner nodes to insert. \n";

	//randomize? use just a subset?
	random_shuffle(candidates_steiner_nodes.begin(), candidates_steiner_nodes.end());

	int best_cost = INT_MAX, cost, original_cost = solution.find_cost();
	int best_vertex_to_insert;
	foreach(int iu, candidates_steiner_nodes) {
		if(solution.graph.contains_vertex(iu))
		continue; //graph might have changed its structure?

		SteinerSolution tmp_solution = solution;

		//insert new key node
		insert_key_node(iu, tmp_solution);

		tmp_solution.grow_graph();
		tmp_solution.find_mst_tree();

		//check if it was worth it
		cost = tmp_solution.find_cost();

		if (cost < best_cost) {
			best_cost = cost;
			best_vertex_to_insert = iu;

			if(cost < original_cost)
				break; //does not need to keep going, right?
		}
	}

	//keep best
	insert_key_node(best_vertex_to_insert, solution);
	solution.grow_graph();
	solution.find_mst_tree();

}

void SteinerNodeLocalSearch::remove_key_node(int iu, SteinerSolution& solution) {
	solution.graph.remove_vertex(iu);
}

void SteinerNodeLocalSearch::insert_key_node(int iu, SteinerSolution& solution) {

	vector<int> distances, parents;
	boost::tie(distances, parents) = solution.instance.get_shortest_distances(iu);

	//add all shortest paths to all terminals of current tree ( ?? )

	foreach(int t, solution.instance.terminals) {
		solution.add_path(iu, t, parents);
	}


	//add all paths
	/*
	list<int> vertices_to_connect;
	foreach(Vertex v, boost::vertices(solution.graph.boostgraph)) {
		vertices_to_connect.push_back( solution.graph.index_for_vertex(v) );
	}

	foreach(int iv, vertices_to_connect) {
		solution.add_path(iu, iv, parents);
	}
	*/


	solution.grow_graph();

}

