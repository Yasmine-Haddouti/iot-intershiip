#include <string.h>
#include <stdio.h>
#include <stdlib.h>

String orientation="W";
int posX=50;
int posY=50;

void setup() {
Serial.begin(9600);
do_turnLeft();
Serial.println("Set up done");
}

void loop() {
  do_recule();
  Serial.println("\n positionX :");
  Serial.print(posX);
  Serial.println("\n positionY :");
  Serial.print(posY);
  Serial.println("\n Orientation :");
  Serial.print(orientation);
  Serial.println("\n");
 delay(500);
}



void do_turnLeft(){
 Serial.println("à gauche \n");
 if (orientation =="N")
    orientation ="W";
   else if (orientation =="W")
    orientation ="S";
   else if (orientation =="S")
    orientation ="E";
   else if (orientation =="E")
    orientation ="N"; 
} 
  
  
void do_turnRight(){
       Serial.println("à droite\n");
  if (orientation =="N")
    orientation ="E";
   else if (orientation =="W")
    orientation ="N";
   else if (orientation =="S")
    orientation ="W";
   else if (orientation =="E")
    orientation ="S"; 
} 
  
void do_Avance(){
        Serial.println("avance \n");
  if (orientation =="N"){
    if (posY==100)
      Serial.println("Impossible");
    else posY++;}
  else if (orientation =="S"){
    if (posY==0)
      Serial.println("Impossible");
    else posY--;}
  else if (orientation =="E"){
    if (posX==100)
      Serial.println("Impossible");
    else posX++;}
  else if (orientation =="W"){
    if (posX==0)
      Serial.println("Impossible");
    else posX--;}
    
    } 
void do_recule(){
  
  if (orientation =="N"){
    if (posY==0)
      Serial.println("Impossible");
    else posY--;}
  else if (orientation =="S"){
    if (posY==100)
      Serial.println("Impossible");
    else posY++;}
  else if (orientation =="E"){
    if (posX==0)
      Serial.println("Impossible");
    else posX--;}
  else if (orientation =="W"){
    if (posX==100)
      Serial.println("Impossible");
    else posX++;}
     
    } 
