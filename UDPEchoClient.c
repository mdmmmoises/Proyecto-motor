/*
Nombres: Huberney Pamo Saenz, Moises Mora Martínez, Alejandro Arenas Jiménez.
Fecha: 7 de abríl del 2020.
Presentado a: Ing. Marco Ortíz.

El siguiente programa, realizará la conexión adecuada hacia el servidor del motor a través de la dirección IP: 186.155.208.171,
además el carnet correspondiente del usuario y también mediante el puerto de entrada 65000. Los procesará y guardará las variables mediante una string 
para posteriormente realizar su uso en el proceso padre de la interfaz gráfica.

*/

//Definimos las librerías y constantes necesarias que conectaran con algunas estructuras externas
#include <stdio.h> // Libreria basica de c.
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#define MAX_FILE 500
#define MAX_F 100
#define MAX_C 100
#define MAX 2
#define ECHOMAX 255


//DEFINIMOS FUNCIONES

char *Conexion_internet(int argc, char *argv[], char *buffer); // Funcion que nos permite conectarnos al servidor con protocolo UDP y modificar una cadena de string en el menu.
void DieWithError (char *errorMessage); //Mediante esta función imprimimos los errores.
void Separar_datos (char informacion []);//Gracias a esta función separamos los datos principales y los imprimimos en un archivo 

//PROGRAMA PRINCIPAL 
int main (int argc, char *argv[])
{
	//DECLARAMOS VARIABLES
	char informacion[ECHOMAX]; // Se crea la cadena de caracteres que en el menu guardara la informacion del estado del motor.

	Conexion_internet(argc, argv, informacion);// Establecemos conexión y obtenemos una cadena de caracteres con la informacion completa.
	printf("Received: %s\n", informacion);  
	Separar_datos(informacion);
	
	return 0;
}


//FUNCIONES 
char *Conexion_internet(int argc, char *argv[], char *buffer)//El apuntador servira para cambiar el valor de informacion en el principal.
{
	int sock;
	struct sockaddr_in echoServAddr;
	struct sockaddr_in fromAddr;
	unsigned short echoServPort;
	unsigned int fromSize;
	char *servIP;
	char *echoString;
	char echoBuffer[ECHOMAX+1];
	int echoStringLen;
	int respStringLen;

	if ((argc < 3) || (argc > 4))
	{
		fprintf (stderr, "Usage: %s <Server IP> <Echo Word> [<Echo Port>]\n", argv[0]);	//Realizamos la validación del número de argumentos recibidos, para el correcto funcionamiento del programa
		exit(1);
	}

	servIP = argv[1];
	echoString = argv[2];

	if ((echoStringLen = strlen(echoString)) > ECHOMAX)
		DieWithError("Echo word too long"); //Validamos la longitud de la cadena recibida del ECHO

	if (argc == 4)
		echoServPort = atoi (argv[3]);
	else
		echoServPort = 7;

	if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		DieWithError("socket() failed"); //Si la validación del socket es menor a 0, encontramos este error

	memset(&echoServAddr, 0, sizeof(echoServAddr));
	echoServAddr.sin_family = AF_INET;
	echoServAddr.sin_addr.s_addr = inet_addr(servIP);
	echoServAddr.sin_port	= htons(echoServPort);

	if (sendto(sock, echoString, echoStringLen, 0, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr)) != echoStringLen) 
        DieWithError("sendto() sent a different number of bytes than expected"); 
		
	fromSize = sizeof(fromAddr);
	(respStringLen = recvfrom(sock, echoBuffer, ECHOMAX, 0, (struct sockaddr *) &fromAddr, &fromSize));

	if (echoServAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr)
	{
		fprintf(stderr,"Error: received a packet from unknow source.\n"); //Si el servidor validado es distinto al original, se envía este mensaje 
		exit(1);
	}

	echoBuffer[respStringLen] = '\0';
	strcpy(buffer,echoBuffer); // Copiamos en buffer el echoBuffer
	close(sock);
	return buffer;// Devolvemos el puntero de cada posicion para que el proceso se haga automatico.
	//exit(0); // Se comenta para que no se salga aqui si no en el principal.
	
	
}

void DieWithError (char *errorMessage) //Se realiza la descripción de la función "DieWithError" con el fin de imprimir los mensajes determinados
{
	printf("%s",errorMessage);	
}

void Separar_datos (char informacion [])
{
		int longitud_info=0, contador=0;
		int identifica_doble_punto=0,posicion_motor=0, copia_lugar_doble_punto=0;;
		unsigned char lugar_doble_punto[ECHOMAX];
		char motor_info[ECHOMAX],copia_info[ECHOMAX];
		FILE *ruta_archivo_informacion;
		
		longitud_info=strlen(informacion);
		strcpy(copia_info,informacion);
		copia_info[longitud_info-1]=','; //Colocamos una coma al final, para que el algoritmo identifique bien 
		
		for(contador=0;contador<longitud_info-1;contador++)//Generamos ciclo de identificación del caracter ':'
		{
			if(copia_info[contador]==':')//Si encuentra un ':'
			{
				lugar_doble_punto[identifica_doble_punto]=contador;//Guarde la posición en "lugar_doble_punto" donde encontró al ':'
				identifica_doble_punto++;//Aumenta 1 espacio para guardar la siguiente coincidencia
			}
		}
		
		for(contador=1;contador<7;contador++)
		{
			copia_lugar_doble_punto=lugar_doble_punto[contador]+1;//Se guarda como constante lo obtenido dentro del arreglo con el fin de lograr una inicialización para el siguiente for
			for(identifica_doble_punto=copia_lugar_doble_punto;identifica_doble_punto<longitud_info-1;identifica_doble_punto++)
			{
				if(copia_info[identifica_doble_punto]!=',' && copia_info[identifica_doble_punto]!='}')//Si dentro de la información excluimos los caracteres ',' y '}'
					{
						motor_info[posicion_motor]=copia_info[identifica_doble_punto];//Copiamos la información obtenida hasta el momento
						posicion_motor++;//Aumenta 1 espacio para guardar la siguiente coincidencia para el espacio
					}
				if(copia_info[identifica_doble_punto]==',' || copia_info[identifica_doble_punto]=='\0')//Si dentro de la información excluimos los caracteres ',' o '\0'
					{
						motor_info[posicion_motor]=' ';//Colocamos un espacio con el fin de separar la información
						posicion_motor++;//Aumenta 1 espacio para guardar la siguiente coincidencia de información del motor
						identifica_doble_punto=longitud_info;//Asignamos la nueva posición donde se identificó el doble_punto dada la longitud actual restante
					}
			}
				
		}
		motor_info[posicion_motor]=' ';//Asignamos un espacio al final
		motor_info[posicion_motor+1]='\0';//Ponemos un fin de cadena para evitar desbordes
		
		ruta_archivo_informacion=fopen("/home/alse/Documents/Tercio3/version7/datos_motor.txt","w+");
		
		fputs(motor_info,ruta_archivo_informacion);//Escribe motor_info en ruta_archivo_informacion
		printf("\n\t se guardaron los datos del motor \n\n");
		fclose(ruta_archivo_informacion);	
}







