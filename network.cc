/**
 * 	TODO LIST
 *	sprawdzić nulle
 *	wyrzucić rzeczy powtarzające się do funkcji
 *	dopisać 3 funkcje
 * 	sprawdzić czy działa
 * 	ujednolicić nazewnictwo (node/vertex)
 * 	ewentualne komentarze
 * 	?! skrócenie kodu przez zmianę coding style ?
 *	sprawdzić czy to się da czytać
 * 	dopisać/usunąć using, typedef
 */

#ifdef DEBUG
	const bool debug = true;
#else
	const bool debug = false;
#endif

#include "network.h"
#include <algorithm>
#include <map>
#include <set>
#include <tuple>
#include <iostream>

using std::get;
using std::cerr;
using std::pair;
using std::set;

typedef unsigned long net_id;
typedef size_t label_id;

typedef std::map<const char*, label_id> label_map;
typedef pair<label_id, label_id> edge;
typedef pair<set<edge>, set<edge> > vertex; //in, out
typedef std::map<label_id, vertex > graph;
typedef std::tuple<bool, size_t, size_t, label_map, graph> net;
typedef std::map<net_id, net>::iterator data_iterator;

const short IS_GROWING = 0;
const short LINKS_NUMBER = 1;
const short FIRST_FREE_LABEL_ID = 2;
const short LABEL_MAP = 3;
const short GRAPH = 4;

static std::map<net_id, net > data;
static net_id first_free_id = 0;

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
void network_delete(unsigned long id)
{
	if(debug)	cerr <<"Delete(" <<id <<")\n";

	data_iterator iter;

	if( (iter = data.find(id) ) == data.end());
	{
		if(debug)	cerr <<"Attempt to use non-existing network.\n";
		return;
	}

	if(debug)	cerr <<"Network " <<id <<" deleted.\n";
	data.erase(iter);
}

/**
 * Jeżeli istnieje sieć o identyfikatorze id,
 * zwraca liczbę jej węzłów,
 * a w przeciwnym przypadku zwraca 0.
 */
size_t network_nodes_number(unsigned long id)
{
	if(debug)	cerr <<"Nodes_number(" <<id <<")\n";

	data_iterator iter;

	if( (iter = data.find(id) ) == data.end());
	{
		if(debug)	cerr <<"Attempt to use non-existing network.\n";
		return 0;
	}

	size_t number = get<GRAPH>(iter->second).size();
	if(debug)	cerr <<"Network " <<id <<" has "<<number <<"nodes.\n";

	return number;
}



/**
 * Jeżeli istnieje sieć o identyfikatorze id,
 * zwraca liczbę jej węzłów,
 * a w przeciwnym przypadku zwraca 0.
 */
size_t network_links_number(unsigned long id)
{
	if(debug)	cerr <<"Links_number(" <<id <<")\n";

	data_iterator iter;

	if( (iter = data.find(id) ) == data.end());
	{
		if(debug)	cerr <<"Attempt to use non-existing network.\n";
		return 0;
	}

	if(debug)	cerr <<"Network " <<id <<" has "<<get<LINKS_NUMBER>(iter->second) <<"links.\n";

	return get<LINKS_NUMBER>(iter->second);
}

/**
 * Jeżeli istnieje sieć o identyfikatorze id, label != NULL
 * i sieć ta nie zawiera jeszcze węzła o etykiecie label,
 * to dodaje węzeł o etykiecie label do sieci,
 * a w przeciwnym przypadku nic nie robi.
 */
void network_add_node(unsigned long id, const char* label)
{
	if(debug)	cerr <<"Add_node(" <<id <<" " <<label <<")\n";

	data_iterator iter;

	if( (iter = data.find(id) ) == data.end());
	{
		if(debug)	cerr <<"Attempt to use non-existing network.\n";
		return;
	}

	label_map lm = get<LABEL_MAP>(iter->second);
	if(lm.find(label) != lm.end() )
	{
		if(debug)	cerr <<"Attempt to add existing node to network.\n";
		return;
	}
	vertex empty;
	get<GRAPH>(iter->second)[FIRST_FREE_LABEL_ID] = empty;
	label_id next_label = get<FIRST_FREE_LABEL_ID>(iter->second)++;
	get<LABEL_MAP>(iter->second)[label] = next_label;

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
void network_add_link(unsigned long id, const char* slabel, const char* tlabel)
{
	if(debug)	cerr <<"Add_link(" <<id <<" " <<slabel <<" " <<tlabel <<")\n";

	data_iterator iter;

	if( (iter = data.find(id) ) == data.end());
	{
		if(debug)	cerr <<"Attempt to use non-existing network.\n";
		return;
	}

//TODO: sprawdz czy wierzcholki istnieja (ewentualnie utworz), sprawdz czy istnieje krawedz jeśli nie to utworz
//zwiększenie liczby krawedzi get<LINKS_NUMBER>(iter->second)++;

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
void network_remove_node(unsigned long id, const char* label)
{
	if(debug)	cerr <<"Remove_node(" <<id <<" " <<label <<")\n";

	data_iterator iter;

	if( (iter = data.find(id) ) == data.end());
	{
		if(debug)	cerr <<"Attempt to use non-existing network.\n";
		return;
	}

	if(get<IS_GROWING>(iter->second))
	{
		if(debug)	cerr <<"Attempt to remove something from growing network.\n";
		return;
	}

	label_map lm = get<LABEL_MAP>(iter->second);
	label_map::iterator l_iter = lm.find(label);

	if(l_iter == lm.end() )
	{
		if(debug)	cerr <<"Attempt to remove non-existing node.\n";
		return;
	}

	get<GRAPH>(iter->second).erase( get<GRAPH>(iter->second).find(l_iter->second) );
	//TODO: usuniencie krawedzi dualnych!
	//get<LINKS_NUMBER>(iter->second)-= suma rozmiarów - 1(jeśli pętla)
	get<LABEL_MAP>(iter->second).erase(l_iter);


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

	data_iterator iter;

	if( (iter = data.find(id) ) == data.end());
	{
		if(debug)	cerr <<"Attempt to use non-existing network.\n";
		return;
	}

	if(get<IS_GROWING>(iter->second))
	{
		if(debug)	cerr <<"Attempt to remove something from growing network.\n";
		return;
	}

//TODO sprawdz czy oba istnieją, znajdz je i usun (2 wpisy)
//get<LINKS_NUMBER>(iter->second)--;

	if(debug)	cerr <<"Link from" <<slabel <<" to " <<tlabel <<"in network " <<id <<" removed.\n";
	return;
}

/**
 * Jeżeli istnieje sieć o identyfikatorze id
 * i sieć nie jest rosnąca,
 * usuwa wszystkie jej węzły i krawędzie,
 * a w przeciwnym przypadku nic nie robi.
 */
void network_clear(unsigned long id)
{
	if(debug)	cerr <<"Clear(" <<id <<")\n";

	data_iterator iter;

	if( (iter = data.find(id) ) == data.end());
	{
		if(debug)	cerr <<"Attempt to use non-existing network.\n";
		return;
	}

	if(get<IS_GROWING>(iter->second))
	{
		if(debug)	cerr <<"Attempt to remove something from growing network.\n";
		return;
	}

	get<LINKS_NUMBER>(iter->second) = 0;
	get<FIRST_FREE_LABEL_ID>(iter->second) = 0;
	get<LABEL_MAP>(iter->second).clear();
	get<GRAPH>(iter->second).clear();

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

	data_iterator iter;

	if( (iter = data.find(id) ) == data.end());
	{
		if(debug)	cerr <<"Attempt to use non-existing network.\n";
		return 0;
	}

	label_map lm = get<LABEL_MAP>(iter->second);
	label_map::iterator l_iter = lm.find(label);

	if(l_iter == lm.end() )
	{
		if(debug)	cerr <<"Attempt to use non-existing node.\n";
		return 0;
	}

	graph::iterator g_iter = get<GRAPH>(iter->second).find(l_iter->second);
	size_t degree = (g_iter->second).second.size();

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

	data_iterator iter;

	if( (iter = data.find(id) ) == data.end());
	{
		if(debug)	cerr <<"Attempt to use non-existing network.\n";
		return 0;
	}

	label_map lm = get<LABEL_MAP>(iter->second);
	label_map::iterator l_iter = lm.find(label);

	if(l_iter == lm.end() )
	{
		if(debug)	cerr <<"Attempt to use non-existing node.\n";
		return 0;
	}

	graph::iterator g_iter = get<GRAPH>(iter->second).find(l_iter->second);
	size_t degree = (g_iter->second).first.size();

	if(debug)	cerr <<"Network " <<id <<" contains " <<degree <<"ingoing links from node " <<label <<".\n";
	return degree;
}
