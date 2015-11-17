/*
 * SimuladorDePedidos.c
 *
 *  Created on: 14/11/2015
 *      Author: utnso
 */

#include "SimuladorDePedidos.h"



void mostrar();
typedef enum {MEMORIA=1,SWAP}modulos;

FILE *archivoPedidos;
t_list* listaPedidos;
int modulo;
t_log* logSimulador;

int main()
{
	char* rutaPedidos=malloc(100) ;
	int socketServidor = establecerConexion("config.cfg",&modulo,rutaPedidos);
	listaPedidos = list_create();

	char* pathPedidos = malloc(200);
	strcpy(pathPedidos,"RepertorioPedidos/");
	strcat(pathPedidos,rutaPedidos);

	archivoPedidos= fopen(pathPedidos,"r");
    logSimulador= log_create("LogSimulador","SimuladorDePedidos",false,LOG_LEVEL_INFO);



	parsearPedidos(listaPedidos,archivoPedidos);
	procesar(listaPedidos,modulo,socketServidor);

	//mostrar();



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

	t_protoc_inicio_lectura_Proceso* pedido = elemento;

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




}
void procesarLectura(void* elemento,int socketServidor)
{

	t_protoc_inicio_lectura_Proceso* pedido = elemento;

	int bytesEnviados=0;

	bytesEnviados+=send(socketServidor,&pedido->tipoInstrucc,sizeof(char),0);
	bytesEnviados+=send(socketServidor,&pedido->paginas,sizeof(int),0);
	bytesEnviados+=send(socketServidor,&pedido->pid,sizeof(int),0);

	printf("se envio Pedido Lectura. Bytes Enviados = %d\n",bytesEnviados);

	int tamanioContenido;
	recv(socketServidor,&tamanioContenido,sizeof(int),0);
	char*  contenido = malloc(tamanioContenido);
	recv(socketServidor,contenido,tamanioContenido,0);

	log_info(logSimulador,"PID = %d | PAGINA LEIDA = %d | CONTENIDO = %s",pedido->pid,pedido->paginas,contenido);

	free(contenido);
}
void procesarEscritura(void* elemento,int socketServidor)
{

	t_protoc_escrituraProceso* pedido = elemento;


	int bytesEnviados=0;

		bytesEnviados+=send(socketServidor,&pedido->tipoInstrucc,sizeof(char),0);
		bytesEnviados+=send(socketServidor,&pedido->pid,sizeof(int),0);
		bytesEnviados+=send(socketServidor,&pedido->pagina,sizeof(int),0);
		bytesEnviados+=send(socketServidor,&pedido->tamanio,sizeof(int),0);
		bytesEnviados+=send(socketServidor,pedido->contenido,pedido->tamanio,0);

		printf("se envio Pedido Escritura. Bytes Enviados = %d\n",bytesEnviados);

		char respuesta;
		recv(socketServidor,&respuesta,sizeof(char),0);

		if(respuesta)
		log_info(logSimulador,"PID = %d | PAGINA ESCRITA = %d | CONTENIDO = %s",pedido->pid,pedido->pagina,pedido->contenido);
		else
			log_info(logSimulador,"PID = %d | FALLO AL ECRIBIR = %d | CONTENIDO = %s",pedido->pid,pedido->pagina,pedido->contenido);






}
void procesarFinaliza(void* elemento,int socketServidor)
{
	t_protoc_inicio_lectura_Proceso* pedido = elemento;


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



}
void mostrar()
{
	int i ;
	for(i=0; i<list_size(listaPedidos);i++)
	{
		char inst;
		int pag;
		int pid;

		void* buff = list_get(listaPedidos,i);

		memcpy(&inst,buff,sizeof(char));
		memcpy(&pag , buff+1, sizeof(int));
		memcpy(&pid , buff+4+1, sizeof(int));

		printf("Leido INS: %d PAG: %d PID = %d \n ",inst,pag,pid);


	}


}
