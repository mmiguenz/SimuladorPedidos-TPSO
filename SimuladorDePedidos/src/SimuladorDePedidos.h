/*
 * SimuladorDePedidos.h
 *
 *  Created on: 16/11/2015
 *      Author: utnso
 */

#ifndef SRC_SIMULADORDEPEDIDOS_H_
#define SRC_SIMULADORDEPEDIDOS_H_

#include <stdio.h>
#include <commons/log.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <commons/collections/list.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "protocolos.h"
#include <netinet/in.h>
#include <commons/config.h>
#include<commons/string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "conexion.h"
#include "parser.h"

void procesar(t_list* listaPedidos, int modulo,int socketServidor);
void procesarInicio(void* elemento,int socketServidor);
void procesarLectura(void* elemento,int socketServidor);
void procesarEscritura(void* elemento, int socketServidor);
void procesarFinaliza(void* elemento,int socketServidor);

#endif /* SRC_SIMULADORDEPEDIDOS_H_ */
