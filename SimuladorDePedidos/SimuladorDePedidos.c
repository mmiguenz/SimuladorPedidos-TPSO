/*
 * SimuladorDePedidos.c
 *
 *  Created on: 14/11/2015
 *      Author: utnso
 */
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


void parsearPedidos(t_list* listaPedidos, FILE* archivoPedidos);
int establecerConexion(const char* archivoConfig,int*);
char* leerLinea(FILE* f);
void procesar(t_list* listaPedidos, int modulo,int socketServidor);
void procesarInicio(void* elemento,int socketServidor);
void procesarLectura(void* elemento,int socketServidor);
void procesarEscritura(void* elemento, int socketServidor);
void procesarFinaliza(void* elemento,int socketServidor);

typedef enum {MEMORIA=1,SWAP}modulos;

FILE *archivoPedidos;
t_list* listaPedidos;
int modulo;
t_log* logSimulador;

int main()
{
	int socketServidor = establecerConexion("config.cfg",&modulo);
	listaPedidos = list_create();
	archivoPedidos= fopen("pedidos","r");
    logSimulador= log_create("LogSimulador","SimuladorDePedidos",false,LOG_LEVEL_INFO);



	parsearPedidos(listaPedidos,archivoPedidos);
	procesar(listaPedidos,modulo,socketServidor);



	list_destroy_and_destroy_elements(listaPedidos,free);
	log_destroy(logSimulador);
	fclose(archivoPedidos);


	exit(EXIT_SUCCESS);

}

int establecerConexion(const char* archivoConfig, int* modulo){

	t_config* config =config_create((char*)archivoConfig);

	*modulo = (config_get_int_value(config,"MODULO"));


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


			pedido->pid=atoi(aux[0]);
			pedido->pagina= atoi(aux[1]);
			pedido->contenido=aux[2];
			pedido->tamanio=strlen(pedido->contenido);



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

void procesar(t_list* listaPedidos, int modulo,int socketServidor)
{
	int i ;
	for(i=0; i<list_size(listaPedidos); i++)
	{

		void* elemento = list_get(listaPedidos,i);
		 char tipoInstruccion;


		 memcpy(&tipoInstruccion,elemento,sizeof(char));


		 switch(tipoInstruccion)
		 {
		 case 1: procesarInicio(elemento,socketServidor);break;
		 case 2: procesarLectura(elemento,socketServidor);break;
		 case 3: procesarEscritura(elemento,socketServidor);break;
		 case 4: procesarFinaliza(elemento,socketServidor);break;
		 default: perror("Error al procesar pedido");



		 }





	}


}

void procesarInicio(void* elemento,int socketServidor)
{

	t_protoc_inicio_lectura_Proceso* pedido = malloc(sizeof(t_protoc_inicio_lectura_Proceso));

	memcpy(pedido,elemento,sizeof(t_protoc_inicio_lectura_Proceso));
	int bytesEnviados=0;

	bytesEnviados+=send(socketServidor,&pedido->tipoInstrucc,sizeof(char),0);
	bytesEnviados+=send(socketServidor,&pedido->paginas,sizeof(int),0);
	bytesEnviados+=send(socketServidor,&pedido->pid,sizeof(int),0);



	printf("se envio Pedido Inicio. Bytes Enviados = %d\n",bytesEnviados);

	char respuesta;

	recv(socketServidor,&respuesta,sizeof(char),0);

	if(respuesta==1)
	{
		log_info(logSimulador,"INICIADO     | PID = %d PAGINAS = %d",pedido->pid,pedido->paginas);

	}else
	{
		log_info(logSimulador,"FALLO INICIO | PID = %d PAGINAS = %d",pedido->pid,pedido->paginas);

	}



	free(pedido);



}
void procesarLectura(void* elemento,int socketServidor)
{


}
void procesarEscritura(void* elemento,int socketServidor)
{



}
void procesarFinaliza(void* elemento,int socketServidor)
{
	t_protoc_inicio_lectura_Proceso* pedido = malloc(sizeof(t_protoc_inicio_lectura_Proceso));

	memcpy(pedido,elemento,sizeof(t_protoc_Finaliza));

	int bytesEnviados=0;


	if(modulo==SWAP)
	{


			bytesEnviados+=send(socketServidor,&pedido->tipoInstrucc,sizeof(char),0);
			bytesEnviados+=send(socketServidor,&pedido->pid,sizeof(int),0);


	}else

	{
		bytesEnviados+=send(socketServidor,&pedido->tipoInstrucc,sizeof(char),0);
		bytesEnviados+=send(socketServidor,&pedido->paginas,sizeof(int),0);
		bytesEnviados+=send(socketServidor,&pedido->pid,sizeof(int),0);

	}

	printf("se envio Pedido Finalizar. Bytes Enviados = %d\n",bytesEnviados);

	char respuesta;

	recv(socketServidor,&respuesta,sizeof(char),0);

	if(respuesta==1)
	{
		log_info(logSimulador,"FINALIZADO      | PID = %d",pedido->pid);

	}else
	{
		log_info(logSimulador,"FALLO FINALIZAR | PID = %d",pedido->pid);

	}

	free(pedido);

}
