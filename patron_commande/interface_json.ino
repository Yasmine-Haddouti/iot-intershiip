#define LARGE_JSON_BUFFERS 1

#include <Arduino.h>
#include <Thing.h>
#include <WebThingAdapter.h>

int carPin = 13;
String orientation = "W";
int posX = 50;
int posY = 50;
const char *ssid = "";
const char *password = "";


ThingActionObject *action_generator_avance(DynamicJsonDocument *);
WebThingAdapter *adapter;

const char *prop[] = {"OnOffSwitch", "orientation", nullptr};
ThingDevice voiture("voiture", "Mini", prop);

ThingProperty carOn("on", "Description", BOOLEAN, "OnOffProperty");
ThingProperty positionX("posX", " position horizontale ", NUMBER, "PosProperty");
ThingProperty orient("orientation", " direction cardinale", STRING, "PosProperty");
ThingProperty positionY("posY", " position verticale ", NUMBER, "PosProperty");

StaticJsonDocument<256> avanceInput;
StaticJsonDocument<256> reculeInput;
StaticJsonDocument<256> gaucheInput;
StaticJsonDocument<256> droiteInput;

JsonObject avanceInputObj = avanceInput.to<JsonObject>();
JsonObject reculeInputObj = reculeInput.to<JsonObject>();
JsonObject gaucheInputObj = gaucheInput.to<JsonObject>();
JsonObject droiteInputObj = droiteInput.to<JsonObject>();

ThingAction avance("av", "t_avance", "avance d'une case", "NO_STATE", &avanceInputObj, &action_generator_avance);
ThingAction recule("recule", " avance d'une case", NO_STATE, &recuelInputObj, action_generator_recule);
ThingAction tourneGauche("gauche", " tourne a gauche", NO_STATE, &gaucheInputObj, action_generator_gauche);
ThingAction tourneDroite("droite", "tourne a droite", NO_STATE, &droiteInputObj, action_generator_droite);

void setup(void) {
  pinMode(carPin, OUTPUT);
  digitalWrite(carPin, HIGH);

  WiFi.begin(ssid, password);
  bool blink = true;

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.println(WiFi.localIP());
  adapter = new WebThingAdapter("voiture-v1", WiFi.localIP());
  carOn.title = "On/off";
  voiture.addProperty(&carOn);

  voiture.description = "A web connected car";
  voiture.addProperty(&positionX);
  voiture.addProperty(&positionY);
  voiture.addProperty(&orient);
  voiture.addAction(&avance);
  voiture.addAction(&recule);
  voiture.addAction(&tourneGauche);
  voiture.addAction(&tourneDroite);

  adapter->addDevice(&voiture);
  adapter->begin();
  Serial.print(WiFi.localIP());
  Serial.print("/things/");
  Serial.println(voiture.id);

}

void loop(void) {
  adapter->update();
  bool on = carOn.getValue().boolean;
  //digitalWrite(carPin, on ? LOW : HIGH); // active low led
}

void do_avance(){
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

ThingActionObject *action_generator_avance(DynamicJsonDocument *input) {
    return new ThingActionObject("avance", input, do_avance, nullptr);
}
ThingActionObject *action_generator_recule(DynamicJsonDocument *input) {
    return new ThingActionObject("recule", input, do_recule, nullptr);
}
ThingActionObject *action_generator_droite(DynamicJsonDocument *input) {
    return new ThingActionObject("droite", input, do_turnRight, nullptr);
}
ThingActionObject *action_generator_gauche(DynamicJsonDocument *input) {
    return new ThingActionObject("gauche", input, do_turnLeft, nullptr);
}

