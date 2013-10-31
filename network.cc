/**
 * 	TODO LIST (Soszu):
 *
 *	sprawdzić działanie gdy nulle
 *	dopisać 3 funkcje
 * 	sprawdzić czy działa
 * 	ewentualne komentarze
 */

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

typedef map<const char*, label_id> label_map;
typedef pair<label_id, label_id> link;
typedef pair<set<link>, set<link> > node; //in, out
typedef map<label_id, node > graph;
typedef std::tuple<bool, size_t, size_t, label_map, graph> net;

typedef map<net_id, net>::iterator data_iterator;

static const short IS_GROWING = 0;
static const short LINKS_NUMBER = 1;
static const short FIRST_FREE_LABEL_ID = 2;
static const short LABEL_MAP = 3;
static const short GRAPH = 4;

static map<net_id, net > data;
static net_id first_free_id = 0;

bool get_network(unsigned long id, net &net_record){
	data_iterator iter = data.find(id);

	if (iter == data.end()){
		if(debug)	cerr <<"Attempt to use non-existing network.\n";
		return true;
	}
	net_record = iter->second;
	return false;
}

bool get_node(net n, const char* label, node & node_record){

	label_map lm = get<LABEL_MAP>(n);
	label_map::iterator l_iter = lm.find(label);

	if(l_iter == lm.end() ){
		if(debug)	cerr <<"Attempt to add existing node to network.\n";
		return true;
	}
	node_record = (get<GRAPH>(n).find(l_iter->second))->second;
	return false;
}

bool can_remove(net net_record){
	if(get<IS_GROWING>(net_record)){
		if(debug)	cerr <<"Attempt to remove something from grow-only network.\n";
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
	data[first_free_id] = empty;

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

	data_iterator iter = data.find(id);

	if(iter == data.end()){
		if(debug)	cerr <<"Attempt to use non-existing network.\n";
		return;
	}

	if(debug)	cerr <<"Network " <<id <<" deleted.\n";
	data.erase(iter);
	return;
}

/**
 * Jeżeli istnieje sieć o identyfikatorze id,
 * zwraca liczbę jej węzłów,
 * a w przeciwnym przypadku zwraca 0.
 */
size_t network_nodes_number(unsigned long id){
	if(debug)	cerr <<"Nodes_number(" <<id <<")\n";

	net net_record;
	if(get_network(id, net_record))	return 0;

	size_t number = get<GRAPH>(net_record).size();

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

	net net_record;
	if(get_network(id, net_record))	return 0;

	if(debug)	cerr <<"Network " <<id <<" has "<<get<LINKS_NUMBER>(net_record) <<"links.\n";

	return get<LINKS_NUMBER>(net_record);
}

/**
 * Jeżeli istnieje sieć o identyfikatorze id, label != NULL
 * i sieć ta nie zawiera jeszcze węzła o etykiecie label,
 * to dodaje węzeł o etykiecie label do sieci,
 * a w przeciwnym przypadku nic nie robi.
 */
void network_add_node(unsigned long id, const char* label){
	if(debug)	cerr <<"Add_node(" <<id <<" " <<label <<")\n";

	net net_record;
	if(get_network(id, net_record))	return;

	node empty_node;
	if(get_node(net_record, label, empty_node)) return;

	get<GRAPH>(net_record)[FIRST_FREE_LABEL_ID] = empty_node;
	label_id next_label = get<FIRST_FREE_LABEL_ID>(net_record)++;
	get<LABEL_MAP>(net_record)[label] = next_label;

	if(debug)	cerr <<"Added " <<label <<" to network " <<id <<".\n";
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

	net net_record;
	if(get_network(id, net_record))	return;

//TODO: sprawdz czy wierzcholki istnieja (ewentualnie utworz), sprawdz czy istnieje krawedz jeśli nie to utworz
//zwiększenie liczby krawedzi get<LINKS_NUMBER>(net_record)++;

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

	net net_record;
	if(get_network(id, net_record))	return;

	if(!can_remove(net_record)) return;

	label_map lm = get<LABEL_MAP>(net_record);
	label_map::iterator l_iter = lm.find(label);

	if(l_iter == lm.end() ){
		if(debug)	cerr <<"Attempt to remove non-existing node.\n";
		return;
	}

	get<GRAPH>(net_record).erase( get<GRAPH>(net_record).find(l_iter->second) );
	//TODO: zebranie czegoś w funkcje
	//TODO: usuniencie krawedzi dualnych!
	//get<LINKS_NUMBER>(net_record)-= suma rozmiarów - 1(jeśli pętla)
	get<LABEL_MAP>(net_record).erase(l_iter);


	if(debug)	cerr <<"Node " <<label <<" from network " <<id <<" removed.\n";
	return;
}

/**
 * Jeżeli istnieje sieć o identyfikatorze id,
 * a w niej krawędź (slabel, tlabel),
 * oraz sieć nie jest rosnąca,
 * to usuwa krawędź z sieci, a w przeciwnym przypadku nic nie robi.
 */
void network_remove_link(unsigned long id, const char* slabel, const char* tlabel)
{
	if(debug)	cerr <<"Remove_link(" <<id <<" " <<slabel <<" " <<tlabel <<")\n";

	net net_record;
	if(get_network(id, net_record))	return;

	if(!can_remove(net_record)) return;

//TODO sprawdz czy oba istnieją, znajdz je i usun (2 wpisy)
//get<LINKS_NUMBER>(net_record)--;

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

	net net_record;
	if(get_network(id, net_record))	return;

	if(!can_remove(net_record)) return;

	get<LINKS_NUMBER>(net_record) = 0;
	get<FIRST_FREE_LABEL_ID>(net_record) = 0;
	get<LABEL_MAP>(net_record).clear();
	get<GRAPH>(net_record).clear();

	if(debug)	cerr <<"Network " <<id <<" cleared.\n";
	return;
}

/**
 * Jeżeli istnieje sieć o identyfikatorze id,
 * a w niej węzeł o etykiecie label,
 * to zwraca ilość krawędzi wychodzących z tego węzła,
 * a w przeciwnym przypadku zwraca 0.
 */
size_t network_out_degree(unsigned long id, const char* label)
{
	if(debug)	cerr <<"Out_degree(" <<id <<" " <<label <<")\n";

	net net_record;
	if(get_network(id, net_record))	return 0;

	node node_record;
	if(get_node(net_record, label, node_record)) return 0;

	size_t degree = node_record.second.size();

	if(debug)	cerr <<"Network " <<id <<" contains " <<degree <<"outgoing links from node " <<label <<".\n";
	return degree;
}

/**
 * Jeżeli istnieje sieć o identyfikatorze id,
 * a w niej węzeł o etykiecie label,
 * to zwraca ilość krawędzi wchodzących do tego węzła,
 * a w przeciwnym przypadku zwraca 0.
 */
size_t network_in_degree(unsigned long id, const char* label)
{
	if(debug)	cerr <<"In_degree(" <<id <<" " <<label <<")\n";

	net net_record;
	if(get_network(id, net_record))	return 0;

	node node_record;
	if(get_node(net_record, label, node_record)) return 0;

	size_t degree = node_record.first.size();

	if(debug)	cerr <<"Network " <<id <<" contains " <<degree <<"ingoing links from node " <<label <<".\n";
	return degree;
}
