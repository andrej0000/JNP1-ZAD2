#ifndef NETWORK_H
#define NETWORK_H

#include <cstddef>

unsigned long network_new(int growing);
/*
void network_delete(unsigned long id);
size_t network_nodes_number(unsigned long id);
size_t network_links_number(unsigned long id);
void network_add_node(unsigned long id, const char* label);
void network_add_link(unsigned long id, const char* slabel, const char* tlabel);
void network_remove_node(unsigned long id, const char* label);
void network_remove_link(unsigned long id, const char* slabel, const char* tlabel);
void network_clear(unsigned long id);
size_t network_out_degree(unsigned long id, const char* label);
size_t network_in_degree(unsigned long id, const char* label);*/

#endif // NETWORK_H
