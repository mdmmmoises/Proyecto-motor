/*
Nombres: Huberney Pamo Saenz, Moises Mora Martínez, Alejandro Arenas Jiménez.
Fecha: 22 de mayo del 2020.
Presentado a: Ing. Marco Ortíz.

El siguiente programa, realizará la conexión adecuada hacia el servidor del motor a través de la dirección IP: 186.155.208.171,
además el carnet correspondiente del usuario y también mediante el puerto de entrada 65000. Los procesará y guardará las variables mediante una string
para posteriormente realizar su uso en el proceso padre de la interfaz gráfica.
*/


//Librerias de la interfaz grafica
#include "ventana.h"
#include "ui_ventana.h"
//Definimos las librerías y constantes necesarias que conectaran con algunas estructuras externas
#include <stdio.h> // Libreria basica de c.
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>
#include <sys/wait.h>   //Se define la librería para poder utilizar la función wait
#define ECHOMAX 255


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Ventana::Ventana(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Ventana)
{
    ui->setupUi(this);
    setWindowTitle("Estado del motor");

    ui->InsertarCarn->setInputMask("9999999");//Si el cuadro de texto de Insertar Carné no cumple con está mascara no tomará el valor
}

Ventana::~Ventana()
{
    delete ui;
}

//////////////////////////////////// PROGRAMA PRINCIPAL //////////////////////////////////


void Ventana::on_Actualizar_clicked()// Aqui establecemos el protocolo de informacion.
{
    //_________________________________DECLARAMOS VARIABLES_______________________________________
    int velocidad = 1800;
    int frecuencia= 60; // EL FORMATO DE LOS PARAMETROS DEBE SER FLOAT!!!!!!!!!!!!!!!!!!!!Como este ejemplo.
    int voltaje = 120;
    int temperatura = 40;
    float tiempo_op = 0;
    QString carnet1;
    int carnet_numerico, proceso;
    char *argv_definidos[5], nombre_del_c[20]={"./UDPEchoClient.c"}, serverIP[25]={"186.155.208.171"}, puerto[20]={"65000"};
    char carnet_cadena[7] = {0};
    int tv=0, tv2 = 0, tvolt=0, tt=0;

    struct tm tiempo_lugar;
    char tiempo[50];

    carnet1 = ui->InsertarCarn->text();
    carnet_numerico = carnet1.toInt(); //Generamos esta conversión para verificar la validez del carné de acuerdo a la institución

    for (int contador = 0; contador < 8; contador++)//Realizamos la conversión del carné a una cadena con el fin de utilizarlo en nuestro proceso hijo
    {
        std::string cStd = carnet1.toStdString();
        carnet_cadena[contador] = cStd[contador];

        if(contador==2)//Se utiliza esta pequeña implementación con el fin de generar la validación de las variables
            tv=atoi(&carnet_cadena[contador]);
        if(contador==3)
        {
            tv2=atoi(&carnet_cadena[contador]);
            tv=tv2+(tv*10);//se multiplica por diez al ser decenas.
        }
        if(contador==4) // quinto argumento
            tvolt=atoi(&carnet_cadena[contador]);
        if(contador==6) // ultimo argumento
            tt=atoi(&carnet_cadena[contador]);

    }

    if (carnet_numerico != 0 && carnet_numerico > 2000000 && carnet_numerico < 2300000)
    {
        proceso = fork();
        switch (proceso)
        {
            case 0: //Se abre el proceso hijo

                    pid_t pid_hijo;
                    argv_definidos[0] = nombre_del_c; //Se definen los argumentos base para que el programa funcione
                    argv_definidos[1] = serverIP;
                    argv_definidos[2] = carnet_cadena;
                    argv_definidos[3] = puerto;
                    argv_definidos[4] = NULL;
                    execv("/home/alse/Documents/Tercio3/version7/UDPEchoClient.bin", argv_definidos);
            break;

            default: //Proceso padre
                    pid_hijo = wait(NULL); //Espera a que el proceso hijo haya terminado para proceder con el padre

                    FILE *apertura_txt;
                    char *informacion_txt;
                    int size = 0;
                    apertura_txt = fopen ("/home/alse/Documents/Tercio3/version7/datos_motor.txt","r");
                    if(apertura_txt == NULL)
                    {
                        ui->DatosTiempo->setText("Archivo no valido");
                    }
                    fseek (apertura_txt,0,SEEK_END);
                    size = ftell (apertura_txt);
                    informacion_txt=(char*)malloc(size*sizeof(char));
                    rewind(apertura_txt);
                    fread(informacion_txt, 1, size, apertura_txt);


                    char datos_del_txt[size];
                    int variables_cont=0, datos_cont=0, contador=0;
                    float variables[6];

                    for(contador=0;contador<size;contador++)
                    {
                        if(informacion_txt[contador]!=' ' && informacion_txt[contador]!='\0')
                        {
                            datos_del_txt[datos_cont]=informacion_txt[contador];
                            datos_cont++;
                        }
                        if(informacion_txt[contador]==' ' || informacion_txt[contador]=='\0')
                        {
                            variables[variables_cont]=atof(datos_del_txt);
                            datos_cont=0;
                            variables_cont++;
                            datos_del_txt[0]='\0';
                        }
                    }

                    if (variables[1] < (velocidad-tv) || variables[1] > (velocidad+tv)) //Se genera el rango de funcionamiento para la velocidad
                    {
                        ui->Velocidad->setText(QString::number(variables[1],'f', 2)); // pintar de rojo con una precisión de 2.
                        ui->Velocidad->setStyleSheet("background-color:rgb(239, 41, 41)");
                        Ventana::on_checkVelo_stateChanged(1);
                    }
                    else
                    {
                        ui->Velocidad->setText(QString::number(variables[1],'f', 2)); // pintar de verde.
                        ui->Velocidad->setStyleSheet("background-color:rgb(115, 210, 22)");
                        Ventana::on_checkVelo_stateChanged(0);
                    }

                    if (variables[2] < (frecuencia-frecuencia*0.05) || variables[2] > (frecuencia+frecuencia*0.05) ) //Se genera el rango de funcionamiento para la frecuencia
                    {
                        ui->Frecuencia->setText(QString::number(variables[2],'f', 2)); // pintar de rojo.
                        ui->Frecuencia->setStyleSheet("background-color:rgb(239, 41, 41)");
                        Ventana::on_checkFrecuen_stateChanged(1);
                    }
                    else
                    {
                        ui->Frecuencia->setText(QString::number(variables[2],'f', 2)); // pintar de verde.
                        ui->Frecuencia->setStyleSheet("background-color:rgb(115, 210, 22)");
                        Ventana::on_checkFrecuen_stateChanged(0);
                    }

                    if (variables[3] < (voltaje-tvolt) || variables[3] > (voltaje+tvolt)) //Se genera el rango de funcionamiento para el voltaje
                    {
                        ui->Voltaje->setText(QString::number(variables[3],'f', 2)); // pintar de rojo.
                        ui->Voltaje->setStyleSheet("background-color:rgb(239, 41, 41)");
                        Ventana::on_checkVoltaje_stateChanged(1);
                    }
                    else
                    {
                        ui->Voltaje->setText(QString::number(variables[3],'f', 2)); // pintar de verde.
                        ui->Voltaje->setStyleSheet("background-color:rgb(115, 210, 22)");
                        Ventana::on_checkVoltaje_stateChanged(0);
                    }

                    if (variables[4] < (temperatura-tt) || variables[4] > (temperatura+tt)) //Se genera el rango de funcionamiento para la temperatura
                    {
                        ui->Temperatura->setText(QString::number(variables[4],'f', 2)); // pintar de rojo.
                        ui->Temperatura->setStyleSheet("background-color:rgb(239, 41, 41)");
                        Ventana::on_checkTemp_stateChanged(1);
                    }
                    else
                    {
                        ui->Temperatura->setText(QString::number(variables[4],'f', 2)); // pintar de verde.
                        ui->Temperatura->setStyleSheet("background-color:rgb(115, 210, 22)");
                        Ventana::on_checkTemp_stateChanged(0);
                    }

                    ui->TolVeloci->setText("(Ref.1800)RPM +/- "+(QString::number(tv))+"%");  //Se genera una entrada de texto al recuadro según las variables de tolerancia obtenidas por el carné
                    ui->TolFrecue->setText("(Ref.60)Hz +/- "+QString::number(tt)+"%");
                    ui->TolVolt->setText("(Ref.120)Volt +/- "+QString::number(tvolt)+"%");
                    ui->TolTemp->setText("(Ref.40)C° +/- "+QString::number(tt)+"%");


                    tiempo_op = variables[5]/1000;
                    time_t rawtime=tiempo_op;
                    tiempo_lugar=*localtime(&rawtime);
                    strftime(tiempo, sizeof(tiempo),"%a %Y-%m-%d %H:%M:%S %Z",&tiempo_lugar); //Da formato gracias al timestamp
                    ui->TiempoOpera->setText(tiempo);

        //5. Hace falta subirlo a GITHUB al finalizar todo
					

        }
    }
    else
    {
        ui->DatosTiempo->setText("Digite un carné válido");
        ui->DatosTiempo->setStyleSheet("background-color:rgb(239, 41, 41)"); // pintar de rojo.
        ui->Frecuencia->clear();
        ui->Frecuencia->setStyleSheet("background-color:rgb(255, 255, 255)");
        ui->Velocidad->clear();
        ui->Velocidad->setStyleSheet("background-color:rgb(255, 255, 255)");
        ui->Voltaje->clear();
        ui->Voltaje->setStyleSheet("background-color:rgb(255, 255, 255)");
        ui->Temperatura->clear();
        ui->Temperatura->setStyleSheet("background-color:rgb(255, 255, 255)");
        ui->TiempoOpera->clear();
        ui->TiempoOpera->setStyleSheet("background-color:rgb(255, 255, 255)");
        ui->TolVeloci->clear();
        ui->TolVeloci->setStyleSheet("background-color:rgb(255, 255, 255)");
        ui->TolVolt->clear();
        ui->TolVolt->setStyleSheet("background-color:rgb(255, 255, 255)");
        ui->TolTemp->clear();
        ui->TolTemp->setStyleSheet("background-color:rgb(255, 255, 255)");
        ui->TolFrecue->clear();
        ui->TolFrecue->setStyleSheet("background-color:rgb(255, 255, 255)");
        ui->DatosVEL->clear();
        ui->DatosVEL->setStyleSheet("background-color:rgb(255, 255, 255)");
        ui->DatosFRECUEN->clear();
        ui->DatosFRECUEN->setStyleSheet("background-color:rgb(255, 255, 255)");
        ui->DatosVOLTAJE->clear();
        ui->DatosVOLTAJE->setStyleSheet("background-color:rgb(255, 255, 255)");
    }

}


void Ventana::on_checkVelo_stateChanged(int estado)
{
  if (estado)
  {
      ui->DatosVEL->setText("ES NECESARIO TOMAR ACCIONES EN LA VELOCIDAD");
  }
  else
      ui->DatosVEL->clear();
}


void Ventana::on_checkFrecuen_stateChanged(int estado)
{
    if (estado)
    {
        ui->DatosFRECUEN->setText("ES NECESARIO TOMAR ACCIONES EN LA FRECUENCIA");
    }
    else
        ui->DatosFRECUEN->clear();
}

void Ventana::on_checkVoltaje_stateChanged(int estado)
{
    if (estado)
    {
        ui->DatosVOLTAJE->setText("ES NECESARIO TOMAR ACCIONES EN EL VOLTAJE");
    }
    else
        ui->DatosVOLTAJE->clear();
}

void Ventana::on_checkTemp_stateChanged(int estado)
{
    if (estado)
    {
        ui->DatosTiempo->setText("ES NECESARIO TOMAR ACCIONES EN TEMPERATURA");
    }
    else
        ui->DatosTiempo->clear();
}




