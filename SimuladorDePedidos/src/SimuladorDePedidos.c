/*
 * SimuladorDePedidos.c
 *
 *  Created on: 14/11/2015
 *      Author: utnso
 */

#include "SimuladorDePedidos.h"




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
