/*
 * parser.h
 *
 *  Created on: 16/11/2015
 *      Author: utnso
 */

#ifndef SRC_PARSER_H_
#define SRC_PARSER_H_
#include "SimuladorDePedidos.h"

void parsearPedidos(t_list* listaPedidos, FILE* archivoPedidos);
char* leerLinea(FILE* f);

#endif /* SRC_PARSER_H_ */
