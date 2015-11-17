/*
 * conexion.c
 *
 *  Created on: 16/11/2015
 *      Author: utnso
 */

#include "conexion.h"
#include "SimuladorDePedidos.h"

int establecerConexion(const char* archivoConfig, int* modulo,char* rutaPedidos ){

	t_config* config =config_create((char*)archivoConfig);

	*modulo = (config_get_int_value(config,"MODULO"));

	char* prueba = config_get_string_value(config,"RUTA");
	int tamanio = strlen(prueba)+1;
	memcpy(rutaPedidos,prueba,tamanio);


	struct sockaddr_in dire_serv;
	dire_serv.sin_family = AF_INET;
	dire_serv.sin_addr.s_addr = inet_addr(config_get_string_value(config,"IP"));
	dire_serv.sin_port = htons(atoi(config_get_string_value(config,"PUERTO")));
	int socketS = socket(AF_INET, SOCK_STREAM, 0);
	if (connect(socketS, (void*) &dire_serv, sizeof(dire_serv)) < 0) {
		perror("Error al intentar conectar al módulo Swap");
		exit(0);
	}
	else printf("Conexión establecida \n");

	config_destroy(config);

	return socketS;

}
