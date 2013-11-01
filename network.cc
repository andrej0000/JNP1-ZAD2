#ifdef DEBUG
	const bool debug = true;
#else
	const bool debug = false;
#endif

#include "network.h"
#include <map>
#include <set>
#include <tuple>
#include <iostream>

using std::get;
using std::cerr;
using std::pair;
using std::set;
using std::map;

typedef unsigned long net_id;
typedef size_t label_id;

typedef map<std::string, label_id> label_map;
typedef pair<set<label_id>, set<label_id> > node; //in, out
typedef map<label_id, node > graph;
typedef std::tuple<bool, size_t, size_t, label_map, graph> net;

typedef map<net_id, net>::iterator data_iterator;
typedef map<label_id, node >::iterator graph_iterator;

static const short IS_GROWING = 0;
static const short LINKS_NUMBER = 1;
static const short FIRST_FREE_LABEL_ID = 2;
static const short LABEL_MAP = 3;
static const short GRAPH = 4;

static net_id first_free_id = 0;

map<net_id, net >& get_data(){
	//std::ios_base::init::init();
	static map<net_id, net > data;
	return data;
}

/**
 * jeśli siec o podanym id istnieje,
 * to funkcja zwraca false i net_record przyjmuje wartość danych tej sieci
 * a w przeciwnym wypadku funkcja zwraca true, a wartosć net_record jest nieokreślona
 */
bool get_network(unsigned long id, data_iterator & net_iterator){
	data_iterator iter = get_data().find(id);

	if (iter == get_data().end()){
		if(debug)	cerr <<"Attempt to use non-existing network.\n";
		return true;
	}
	net_iterator = iter;
	return false;
}

/**
 * jeśli wierzchołek o etykiecie label jest zawarty w sieci id,
 * to funkcja zwraca false i g_iter przyjmuje wartość itertora na wpis wierzchołka label
 * a w przeciwnym wypadku funkcja zwraca true, a wartosć g_iter jest nieokreślona
 */
bool get_graph_iterator(data_iterator n, const char* label, graph::iterator & g_iter){
	label_map lm = get<LABEL_MAP>(n->second);
	label_map::iterator l_iter = lm.find(label);

	if(l_iter == lm.end() ){
		if(debug)	cerr <<"Attempt to use non-existing node.\n";
		return true;
	}
	g_iter = get<GRAPH>(n->second).find(l_iter->second);
	return false;
}

/**
 * jeśli z sieci mogą być usuwane elementy,
 * to funkcja zwraca true, false wpp.
 */
bool can_remove(net net_record){
	if(get<IS_GROWING>(net_record)){
		if(debug)	cerr <<"Attempt to remove something from grow-only network.\n";
		return true;
	}
	return false;
}

/**
 * jeśli etykieta to NULL,
 * to funkcja zwraca true, false wpp.
 */
bool is_null(const char* label){
	if(label == NULL){
		if(debug)	cerr <<"Attempt to use null label.\n";
		return true;
	}
	return false;
}


/**
 * Tworzy nową, pustą, sieć i zwraca jej identyfikator.
 * Sieć pusta, to sieć z pustym zbiorem węzłów.
 * Parametr growing mówi o tym, czy nowa sieć ma być rosnąca (growing != 0) czy nie (growing == 0).
 */
unsigned long network_new(int growing){
	if(debug)	cerr <<"New(" <<growing <<")\n";

	net empty;
	get<FIRST_FREE_LABEL_ID>(empty) = 0;
	get<IS_GROWING>(empty) = growing != 0 ? true : false;
	get_data().insert(pair<net_id, net>(first_free_id, empty));

	if(debug)	cerr <<"Network " <<first_free_id <<" created.\n";

	return first_free_id++;
}

/**
 * Jeżeli istnieje sieć o identyfikatorze id,
 * to usuwa sieć,
 * a w przeciwnym przypadku nic nie robi.
 */
void network_delete(unsigned long id){
	if(debug)	cerr <<"Delete(" <<id <<")\n";

	data_iterator iter = get_data().find(id);

	if(iter == get_data().end()){
		if(debug)	cerr <<"Attempt to use non-existing network.\n";
		return;
	}

	if(debug)	cerr <<"Network " <<id <<" deleted.\n";
	get_data().erase(iter);
	return;
}

/**
 * Jeżeli istnieje sieć o identyfikatorze id,
 * zwraca liczbę jej węzłów,
 * a w przeciwnym przypadku zwraca 0.
 */
size_t network_nodes_number(unsigned long id){
	if(debug)	cerr <<"Nodes_number(" <<id <<")\n";

	data_iterator net_it;
	if(get_network(id, net_it))	return 0;

	size_t number = get<GRAPH>(net_it->second).size();

	if(debug)	cerr <<"Network " <<id <<" has "<<number <<"nodes.\n";
	return number;
}

/**
 * Jeżeli istnieje sieć o identyfikatorze id,
 * zwraca liczbę jej węzłów,
 * a w przeciwnym przypadku zwraca 0.
 */
size_t network_links_number(unsigned long id){
	if(debug)	cerr <<"Links_number(" <<id <<")\n";

	data_iterator net_it;
	if(get_network(id, net_it))	return 0;

	if(debug)	cerr <<"Network " <<id <<" has "<<get<LINKS_NUMBER>(net_it->second) <<"links.\n";

	return get<LINKS_NUMBER>(net_it->second);
}

/**
 * Jeżeli istnieje sieć o identyfikatorze id, label != NULL
 * i sieć ta nie zawiera jeszcze węzła o etykiecie label,
 * to dodaje węzeł o etykiecie label do sieci,
 * a w przeciwnym przypadku nic nie robi.
 */
void network_add_node(unsigned long id, const char * label){
	if(debug)	cerr <<"Add_node(" <<id <<" " <<label <<")\n";

	if(is_null(label)) return;

	data_iterator net_it;
	if(get_network(id, net_it))	return;

	graph::iterator empty_node_iter;
	if(!get_graph_iterator(net_it, label, empty_node_iter)) return;

	label_id new_label_id = get<FIRST_FREE_LABEL_ID>(net_it->second);
	//cerr << "There are nodes: " << get<LABEL_MAP>(net_it->second).size();
	//cerr << "New node will have id: " << new_label_id << std::endl;
	get<GRAPH>(net_it->second).emplace(std::pair<label_id, node>(new_label_id, node()));
	std::string lbl(label);
	//cerr << "New node name " << lbl << std::endl;
	get<LABEL_MAP>(net_it->second).emplace(std::pair<std::string, label_id>(lbl, new_label_id));
	get<FIRST_FREE_LABEL_ID>(net_it->second) = new_label_id + 1;
	//cerr << "Next id " << get<FIRST_FREE_LABEL_ID>(net_it->second) << std::endl;

	if(debug)	cerr << "Added " << label << " to network " << id << ".\n";

	return;
}

/**
 * Jeżeli istnieje sieć o identyfikatorze id,
 * slabel != NULL oraz tlabel != NULL,
 * i sieć ta nie zawiera jeszcze krawędzi (slabel, tlabel),
 * to dodaje krawędź (slabel, tlabel) do sieci,
 * a w przeciwnym przypadu nic nie robi.
 * Jeżeli w sieci nie istnieje węzeł o etykiecie któregoś z końców krawędzi, to jest on również dodawany.
 */
void network_add_link(unsigned long id, const char* slabel, const char* tlabel){
	if(debug)	cerr <<"Add_link(" <<id <<" " <<slabel <<" " <<tlabel <<")\n";

	if(is_null(slabel) || is_null(tlabel)) return;

	data_iterator net_it;
	if(get_network(id, net_it))	return;

	graph::iterator snode_iter;
	graph::iterator tnode_iter;

	if(get_graph_iterator(net_it, slabel, snode_iter)){
		network_add_node(id, slabel);
		if(get_graph_iterator(net_it, slabel, snode_iter) && debug)
			cerr << "Fail\n";
		//if (debug) cerr << slabel << " has id " << snode_iter->first << std::endl;
	}
	if(get_graph_iterator(net_it, tlabel, tnode_iter)){
		network_add_node(id, tlabel);
		if(get_graph_iterator(net_it, tlabel, tnode_iter) && debug)
			cerr << "Fail\n";
		//if (debug) cerr << slabel << " has id " << snode_iter->first << std::endl;
		//if (debug) cerr << tlabel << " has id " << tnode_iter->first << std::endl;
	}

	if (debug)	cerr << "Obtained both nodes\n";
	set<label_id>::iterator s_iter = ((snode_iter->second).second).find(tnode_iter->first); //w wychodzących z snode szukam tnode label_id
	if(s_iter != (snode_iter->second).second.end()){
		if(debug)	cerr <<"Attempt to add existing link.\n";
		return;
	}
	//if (debug) cerr << slabel << " has id " << snode_iter->first << std::endl;
	//if (debug) cerr << tlabel << " has id " << tnode_iter->first << std::endl;
	(snode_iter->second).second.insert(tnode_iter->first);
	(tnode_iter->second).first.insert(snode_iter->first);
	//(get<1>(snode_iter->second)).insert(tlabel);
	//(get<0>(tnode_iter->second)).insert(slabel);
	get<LINKS_NUMBER>(net_it->second)++;
	//get_graph_iterator(net_it, slabel, snode_iter);
	//if (debug) cerr << slabel << " has " << get<1>(snode_iter->second).size() << std::endl;
	//network_out_degree(id, slabel);
	if(debug)	cerr <<"Link from" <<slabel <<" to " <<tlabel <<"in network " <<id <<" added.\n";
	return;
}

/**
 * Jeżeli istnieje sieć o identyfikatorze,
 * a w niej węzeł o etykiecie label
 * oraz sieć nie jest rosnąca,
 * to usuwa węzeł z sieci wraz ze wszystkimi krawędziami wchodzącymi i wychodzącymi,
 * a w przeciwnym przypadku nic nie robi.
 */
void network_remove_node(unsigned long id, const char* label){
	if(debug)	cerr <<"Remove_node(" <<id <<" " <<label <<")\n";

	if(is_null(label)) return;

	data_iterator net_it;
	if(get_network(id, net_it))	return;

	if(can_remove(net_it->second)) return;

	graph::iterator node_record_iter;

	if(get_graph_iterator(net_it, label, node_record_iter)) return;

	size_t links_connected_number = (node_record_iter->second).second.size() + (node_record_iter->second).first.size();

	//sprawdzenie czy jest pętla
	set<label_id>::iterator loop_iter = ((node_record_iter->second).second).find(node_record_iter->first);
	if(loop_iter != (node_record_iter->second).second.end())	--links_connected_number;

	//usuniencie krawedzi dualnych

	for(set<label_id>::iterator li = (node_record_iter->second).first.begin(); li != (node_record_iter->second).first.end(); ++li){
		graph::iterator other_node_iter = get<GRAPH>(net_it->second).find(*li);
		//network_remove_link(id, node_record_iter->first, other_node_iter->first);
		(other_node_iter->second).second.erase(node_record_iter->first);
	}
	for(set<label_id>::iterator li = (node_record_iter->second).second.begin(); li != (node_record_iter->second).second.end(); ++li){
		graph::iterator other_node_iter = get<GRAPH>(net_it->second).find(*li);
		//network_remove_link(id, other_node_iter->first, node_record_iter->first);
		(other_node_iter->second).first.erase(node_record_iter->first);
	}

	get<LINKS_NUMBER>(net_it->second)-= links_connected_number;
	get<GRAPH>(net_it->second).erase(node_record_iter);
	get<LABEL_MAP>(net_it->second).erase(label);

	if(debug)	cerr <<"Node " <<label <<" from network " <<id <<" removed.\n";
	return;
}

/**
 * Jeżeli istnieje sieć o identyfikatorze id,
 * a w niej krawędź (slabel, tlabel),
 * oraz sieć nie jest rosnąca,
 * to usuwa krawędź z sieci, a w przeciwnym przypadku nic nie robi.
 */
void network_remove_link(unsigned long id, const char* slabel, const char* tlabel){
	if(debug)	cerr <<"Remove_link(" <<id <<" " <<slabel <<" " <<tlabel <<")\n";

	if(is_null(slabel) || is_null(tlabel)) return;

	data_iterator net_it;
	if(get_network(id, net_it))	return;

	if(can_remove(net_it->second)) return;

	graph::iterator snode_iter, tnode_iter;
	if(get_graph_iterator(net_it, slabel, snode_iter))	return;
	if(get_graph_iterator(net_it, tlabel, tnode_iter))	return;

	node snode = (snode_iter->second), tnode = tnode_iter->second;

	set<label_id>::iterator s_iter = ((snode_iter->second).second).find(tnode_iter->first); //w wychodzących z snode szukam tnode label_id
	if(s_iter == (snode_iter->second).second.end()){
		if(debug)	cerr <<"Attempt to remove non-existing link.\n";
		return;
	}

	(snode_iter->second).second.erase(s_iter);
	(tnode_iter->second).first.erase(snode_iter->first);	//z wchodzących do tnode usuwam snode label_id
	get<LINKS_NUMBER>(net_it->second)--;

	if(debug)	cerr <<"Link from" <<slabel <<" to " <<tlabel <<"in network " <<id <<" removed.\n";
	return;
}

/**
 * Jeżeli istnieje sieć o identyfikatorze id
 * i sieć nie jest rosnąca,
 * usuwa wszystkie jej węzły i krawędzie,
 * a w przeciwnym przypadku nic nie robi.
 */
void network_clear(unsigned long id){
	if(debug)	cerr <<"Clear(" <<id <<")\n";

	data_iterator net_it;
	if(get_network(id, net_it))	return;

	if(!can_remove(net_it->second)) return;

	get<LINKS_NUMBER>(net_it->second) = 0;
	get<FIRST_FREE_LABEL_ID>(net_it->second) = 0;
	get<LABEL_MAP>(net_it->second).clear();
	get<GRAPH>(net_it->second).clear();

	if(debug)	cerr <<"Network " <<id <<" cleared.\n";
	return;
}

/**
 * Jeżeli istnieje sieć o identyfikatorze id,
 * a w niej węzeł o etykiecie label,
 * to zwraca ilość krawędzi wychodzących z tego węzła,
 * a w przeciwnym przypadku zwraca 0.
 */
size_t network_out_degree(unsigned long id, const char* label){
	if(debug)	cerr <<"Out_degree(" <<id <<" " <<label <<")\n";

	if(is_null(label)) return 0;


	data_iterator net_it;
	if(get_network(id, net_it))	return 0;

	graph::iterator node_record_iter;
	if(get_graph_iterator(net_it, label, node_record_iter)) return 0;

	size_t degree = get<1>(node_record_iter->second).size();

	if(debug)	cerr <<"Network " <<id <<" contains " <<degree <<"outgoing links from node " <<label <<".\n";
	return degree;
}

/**
 * Jeżeli istnieje sieć o identyfikatorze id,
 * a w niej węzeł o etykiecie label,
 * to zwraca ilość krawędzi wchodzących do tego węzła,
 * a w przeciwnym przypadku zwraca 0.
 */
size_t network_in_degree(unsigned long id, const char* label){
	if(debug)	cerr <<"In_degree(" <<id <<" " <<label <<")\n";

	if(is_null(label)) return 0;

	data_iterator net_it;
	if(get_network(id, net_it))	return 0;

	graph::iterator node_record_iter;
	if(get_graph_iterator(net_it, label, node_record_iter)) return 0;

	size_t degree = (node_record_iter->second).first.size();

	if(debug)	cerr <<"Network " <<id <<" contains " <<degree <<"ingoing links from node " <<label <<".\n";
	return degree;
}
