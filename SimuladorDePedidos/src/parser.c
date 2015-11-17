/*
 * parser.c
 *
 *  Created on: 16/11/2015
 *      Author: utnso
 */

#include "parser.h"


void parsearPedidos(t_list* listaPedidos, FILE* archivoPedidos)
{
	while(!feof(archivoPedidos))
	{
		char* linea = leerLinea(archivoPedidos);
		char** aux;
		aux = string_split(linea,";");
		if(atoi(aux[0]) != 3)
		{


			t_protoc_inicio_lectura_Proceso* pedido = malloc(sizeof(t_protoc_inicio_lectura_Proceso));

			pedido->tipoInstrucc = atoi(aux[0]);
			pedido->paginas= atoi(aux[1]);
			pedido->pid= atoi(aux[2]);

			list_add(listaPedidos,pedido);

		}else
		{

			t_protoc_escrituraProceso* pedido = malloc(sizeof(t_protoc_escrituraProceso));

			pedido->tipoInstrucc= ESCRIBIR;
			pedido->pid=atoi(aux[2]);
			pedido->pagina= atoi(aux[1]);
			pedido->contenido=aux[3];
			pedido->tamanio=strlen(pedido->contenido)+1;




			list_add(listaPedidos,pedido);

		}


		free(linea);
		free(aux);

	}

}



char* leerLinea(FILE* f)
{
	char* buffer = malloc(100);
	memset(buffer,'\0',100);

	fgets(buffer,100,f);
	char** aux = string_split(buffer,"\n");

	int tamanio = strlen(aux[0]);

	memcpy(buffer, aux[0],tamanio);
	buffer[tamanio]='\0';




	if(buffer[0]=='#')
	{
		free(buffer);
		return leerLinea(f);

	}

	free(aux);
	return buffer;



}
