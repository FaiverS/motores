/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "Thread.h"
#include "UnbufferedSerial.h"
#include "cmsis_os2.h"
#include "mbed.h"
#include <cstdio>
#include <cstring>

//define 
#define TIEMPO_LECTURA  100ms
#define BLINKING_RATE   500ms
//prototipos
void leer_datos(void);
void servo_giro(void);
void motor_paso_a_paso (void);
void leer (void);
void motor_reductor(void);

// hilos y funciones del sistema operativo 
Thread hilo_servo_giro;
Thread hilo_leer_datos(osPriorityNormal,2048);
Thread hilo_motor_paso_a_paso;
Thread hilo_leer;
Thread hilo_motor_reductor;

//pines y puertos 
UnbufferedSerial pc(USBTX, USBRX);

// variables 
char men_out[30];
char men_in[4];
char men_servo[90];
char men_pasos[30];
int i=0, j=0, k=0;
int npasos=0,pasosder=0,pasosizq=0;
int servom=0, paso_a_paso=0;
int n,V,o,x,dd;
char men[30];
const char pasos[4] = {0xc,0x6,0x3,0x9};
bool motorpasoapaso_stop=false;
float vel_interna=0.0;
//pines
PwmOut servo(D15);
BusOut bobinas(D3,D4,D5,D6);
DigitalOut led(LED1);
PwmOut   Vel_motorder (D13);
PwmOut   Vel_motorizq (D12);

//case
Mutex p_serie;
Semaphore girederecha;
Semaphore gireizquierda;
Semaphore gireservom;
Semaphore motoreductor_der;
Semaphore motoreductor_izq;

int main()
{
    
   // hilo_servo_giro.start(servo_giro);
   hilo_leer.start(leer);
   // hilo_leer_datos.start(leer_datos);
    sprintf(men_out, "Arranque del programa\n\r");
    pc.write(men_out, strlen(men_out));
    sprintf(men_servo,"escribir 1 servo\n\r");
    pc.write(men_servo, strlen(men_servo));
    
    while (true) 
    {

    }
}

void leer_datos(void)
{
    char c[1];
    char men_int[4];
    char p[1];
    int h;
   
    while(true)
    {
       
        if (pc.readable())
        {
            pc.read(c,1);
            men_in[i] = c[0];
            i++;
            if (i == 4 && men_in[3] == 13) 
            {
                
                strcpy(men_int,men_in);
                pc.write(men_int,4);
                pc.write("\n\r",2);
                npasos = atoi(men_in);
               hilo_motor_paso_a_paso.start(motor_paso_a_paso);
               hilo_motor_reductor.start(motor_reductor);

            if(x==1)
            {
            
                girederecha.acquire();
                sprintf(men_servo,"escribir pasos\n\r");
                pc.write(men_servo, strlen(men_servo));
                p_serie.lock();
                pasosder=npasos;
                p_serie.unlock();
                if (!motorpasoapaso_stop)   girederecha.release();
            }
            if (x==2)
             {
                gireizquierda.acquire();
                p_serie.lock();
                pasosizq=npasos;
                p_serie.unlock();
                if (!motorpasoapaso_stop)   gireizquierda.release();
             
             }

             if (x==3)
             {
                gireservom.acquire();
                p_serie.lock();
                 if (npasos>0 && npasos < 200)
                    {
                    sprintf(men_out,"Angulo = %u \n\r", npasos);
                    pc.write(men_out,strlen(men_out));
                    }
                if (npasos<0 or  npasos> 200)
                    {
                    sprintf(men_out, "Ángulo fuera de rango \n\r");
                    pc.write(men_out, strlen(men_out));
                    }
                p_serie.unlock();
                if (!motorpasoapaso_stop)   gireservom.release();
             
             }
             if(x==4)
            {
            
                 motoreductor_der.acquire();
                
                p_serie.lock();
               if (npasos>0 && npasos < 100)
                    {
                    dd=1;
                    sprintf(men_out,"porcentaje = %u \n\r", npasos);
                    pc.write(men_out,strlen(men_out));
                    }
                if (npasos<0 or  npasos> 100)
                    {
                    sprintf(men_out, "porcentaje fueraderango \n\r");
                    pc.write(men_out, strlen(men_out));
                    }
                p_serie.unlock();
                if (!motorpasoapaso_stop)    motoreductor_der.release();
            }
            if (x==5)
             {
                 dd=1;
                 motoreductor_izq.acquire();
                p_serie.lock();
               if (npasos>0 && npasos < 100)
                    {
                    sprintf(men_out,"porcentaje = %u \n\r", npasos);
                    pc.write(men_out,strlen(men_out));
                    }
                if (npasos<0 or  npasos> 100)
                    {
                    sprintf(men_out, "porcentaje fueraderango \n\r");
                    pc.write(men_out, strlen(men_out));
                    }
                p_serie.unlock();
                if (!motorpasoapaso_stop)    motoreductor_izq.release();
             
             }

             


            for (int j=0; j<4 ; j++) {men_in[j] = '\0';}
        }
            else if (i>4)
            {
                sprintf(men_out, "Error de digitación \n\r");
                pc.write(men_out, strlen(men_out));
                i=0;
                for (int j=0; j<4 ; j++) {men_in[j] = '\0';}
            }
        }

        ThisThread::sleep_for(TIEMPO_LECTURA);

        
       // if (!motorpasoapaso_stop)   girederecha.release();
    }    
    ThisThread::sleep_for(TIEMPO_LECTURA);

        
}


void leer(void)
{
    int caso;
    char c1[1];
    hilo_leer_datos.start(leer_datos);
    while (o<=1){

        if (pc.readable())
        {
         pc.read(c1,1);
         caso = int (c1)-48;

        switch (c1[0]) {
        case 'd':  girederecha.release();motorpasoapaso_stop = true;x=1; break;
        case 'i':  gireizquierda.release();motorpasoapaso_stop = true;x=2; break;
        case 's':  gireservom.release();motorpasoapaso_stop = true;x=3; break;
        case 'w':  motoreductor_der.release();motorpasoapaso_stop = true;x=4; break;
        case 'e':  motoreductor_izq.release();motorpasoapaso_stop = true;x=5; break;
        
       
        }
     o++;
}
}
}




void servo_giro (void)
{
   
  // hilo_leer_datos.start(leer_datos);
    
    while (true)
    {
        n=npasos;
        if (n>=0 && n<=44 )
        {
            V=(6.2*n)+461;
            servo.pulsewidth_us(V);
            sprintf(men, "El angulo es: %u \n\r", n);
            pc.write(men,sizeof(men) );
            ThisThread::sleep_for(2s);
        }
        if (n>=45 && n<=89 )
        {
            V=(9.3*n)+322;
            sprintf(men, "El angulo es: %u \n\r", n);
            pc.write(men,sizeof(men) );
            servo.pulsewidth_us(V);
            ThisThread::sleep_for(2s);
        }
        if (n>=90 && n<=134 )
        {
            V=(10.8*n)+220;
            servo.pulsewidth_us(V);
            sprintf(men, "El angulo es: %u \n\r", n);
            pc.write(men,sizeof(men) );
            ThisThread::sleep_for(2s);
        }
        if (n>=135 && n<=179 )
        {
            V=(11.5*n)+108;
            servo.pulsewidth_us(V);
            sprintf(men, "El angulo es: %u \n\r", n);
            pc.write(men,sizeof(men) );
            ThisThread::sleep_for(2s);
        }
        if (n>=180 && n<=200 )
        {
            V=(7.5*n)+875;
            servo.pulsewidth_us(V);
            sprintf(men, "El angulo es: %u \n\r", n);
            pc.write(men,sizeof(men) );
            ThisThread::sleep_for(2s);
        }
    }
    ThisThread::sleep_for(BLINKING_RATE); 
}







void motor_paso_a_paso (void)
{
    int m=0;


     while (true) {
     
  while(m<=pasosder)
    {
        k++;
        if (k==4) k=0;
        bobinas = pasos[k];
            sprintf(men,"Giro derecha %u \n\r",m);
            pc.write(men, sizeof(men));
       led=!led;
        ThisThread::sleep_for(1ms); 
       m++;
    }
    while(m<=pasosizq)
    { 
            k--;
            if (k==-1) k=3;
            bobinas = pasos[k];
            sprintf(men,"Giro izquierda %u \n\r", m);
            pc.write(men, sizeof(men));
            ThisThread::sleep_for(1ms);
            m++;
    }
     }

ThisThread:: sleep_for(100ms);
}


void motor_reductor (void)
{

    while (true)
        if(dd==1){
        vel_interna += 0.1;      
        if (vel_interna > 0.6) vel_interna=0.0;
        Vel_motorder = vel_interna;
        led = vel_interna;
        ThisThread::sleep_for(1ms);
        }

        if(dd==2){
        vel_interna += 0.1;      
        if (vel_interna > 0.6) vel_interna=0.0;
        Vel_motorizq = vel_interna;
        led = vel_interna;
        ThisThread::sleep_for(1ms);
        }
}

