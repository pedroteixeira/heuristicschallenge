/*
 * steiner_nodesearch.cpp
 *
 *  Created on: Jul 22, 2008
 *      Author: pedro
 */

#include "headers/steiner_nodesearch.hpp"
#include "headers/steiner_solution.hpp"
#include "headers/graph.hpp"

using namespace std;

void SteinerNodeLocalSearch::search(SteinerSolution& solution) {

	if(solution.out_vertices.size() == 0) return; //Nothing todo, really?! TODO: return false?

	//try all out vertices (randomize)
	vector<int> tmp_vertices;
	foreach(int v, solution.out_vertices) {
		tmp_vertices.push_back(v);
	}
	random_shuffle(tmp_vertices.begin(), tmp_vertices.end());

	int best_cost = INT_MAX, cost;
	int best_vertex_to_insert;
	foreach(int i, tmp_vertices) {
		if(solution.out_vertices.get<1>().find(i) == solution.out_vertices.get<1>().end())
		continue; //graph might have changed its out_vertices candidate list

		if(insert_steiner_node(i, solution)) {
			solution.find_mst_tree();
			cost = solution.find_cost();
			if (cost < best_cost) {
				best_cost = cost;
				best_vertex_to_insert = i;
			}
			remove_steiner_node(i, solution);
		}
	}

	//keep best
	insert_steiner_node(best_vertex_to_insert, solution);
	solution.find_mst_tree();

}

void SteinerNodeLocalSearch::remove_steiner_node(int i, SteinerSolution& solution) {
	solution.graph.remove_vertex(i);
	solution.on_steiner_node_removed(i);
}

bool SteinerNodeLocalSearch::insert_steiner_node(int i, SteinerSolution& solution) {

	Vertex v_original = solution.instance->graph.get_vertex(i);

	//add new edges (from original graph) to this graph
	bool at_least_one_edge_added = false;
	foreach(Vertex n_original, boost::adjacent_vertices(v_original, solution.instance->graph.boostgraph)) {

		int index_n = solution.instance->graph.index_for_vertex(n_original);
		bool n_exists_in_solution = solution.graph.contains_vertex(index_n);
		if(!n_exists_in_solution)
		continue;

		Vertex n = solution.graph.get_vertex(index_n);

		//only add if it's connected to solution tree (at least two edges)
		if (n_exists_in_solution && boost::out_degree(n, solution.graph.boostgraph)> 1) {
			//get weight from original graph
			int weight = solution.instance->graph.get_edge_weight(v_original, n_original);
			//add edges from new vertex to solution graph
			solution.graph.add_edge(i , index_n, weight);
			at_least_one_edge_added = true;
		}
	}

	solution.on_steiner_node_inserted(i);

	return at_least_one_edge_added;

}

