#include "defines.h"
#include "proxy.h"

int status = WL_IDLE_STATUS;     // the Wifi radio's status
int reqCount = 0;                // number of requests received

int is_id = 0;
int boolaccess=0;
int boolmodifurl=0;
int boolmodifreg=0;

WiFiWebServer server(80);

//Check if header is present and correct
bool is_authenticated()
{
  Serial.println(F("Enter is_authenticated"));

  if (server.hasHeader(F("Cookie")))
  {
    Serial.print(F("Found cookie: "));
    String cookie = server.header(F("Cookie"));
    Serial.println(cookie);
    if (cookie.indexOf(F("NINASESSIONID=1")) != -1)
    {
      Serial.println(F("Authentication Successful"));
      return true;
    }
  }
  Serial.println(F("Authentication Failed"));
  return false;
}
//---------------------------------------------------------------------------------------------------------------------------------
   //Creation of users
  User* yasmine=build_user("yasmine","test",1,1,1); //password = test
  User* dudu=build_user("dudu","test1",1,0,0);
  //Creation of the list of users
  List* reg=(List*)malloc(sizeof(List));
  //handle url
  char * currenturl="None";

List * updatereg(List* reg){
  init_list(reg);
  insert_beg(reg,yasmine); //yasmine is added to the list of users
  insert_beg(reg,dudu);

  return reg;
}

//---------------------------------------------------------------------------------------------------------------------------------

//login page, also called for disconnect
void handleLogin()
{
  String msg;
  if (server.hasHeader(F("Cookie")))
  {
    Serial.print(F("Found cookie: "));
    String cookie = server.header(F("Cookie"));
    Serial.println(cookie);
  }

  if (server.hasArg("DISCONNECT"))
  {
    Serial.println(F("Disconnection"));
    server.sendHeader(F("Location"), F("/login"));
    server.sendHeader(F("Cache-Control"), F("no-cache"));
    server.sendHeader(F("Set-Cookie"), F("NINASESSIONID=0"));
    server.send(301);
    is_id = 0;
    return;
  }

  if (server.hasArg(F("USERNAME")) && server.hasArg(F("PASSWORD")))
  {
    int finish=0; 
    int success=0;
    Element*current=reg->start;
    while (current!=NULL && !finish){
      if (server.arg(F("USERNAME"))==current->user->username){
        finish=1;
        Serial.println("\nUsername found \n");
        if (server.arg(F("PASSWORD"))==current->user->password){
          success=1;
          Serial.println("Valid password : \n");
          if (current->user->acc_led==1)
              boolaccess=1;
          if (current->user->acc_led==0)
              boolaccess=0;
          if (current->user->acc_url==1)
              boolmodifurl=1;
          if (current->user->acc_url==0)
              boolmodifurl=0;
          if (current->user->acc_reg==1)
              boolmodifreg=1;
          if (current->user->acc_reg==0)
              boolmodifreg=0;
        }
      }
    current=current->next;
    }
    
    if (success==1)
    {
      server.sendHeader(F("Location"), F("/"));
      server.sendHeader(F("Cache-Control"), F("no-cache"));
      server.sendHeader(F("Set-Cookie"), F("NINASESSIONID=1"));
      server.send(301);
      Serial.println(F("Log in Successful"));
      is_id = 1;
      return;
    }

    msg = F("Wrong username/password! try again.");
    Serial.println(F("Log in Failed"));
    //is_id=0;
  }

  String content = F("<html><body><form action='/login' method='POST'> Log in :<br>");
  content += F("User:<input type='text' name='USERNAME' placeholder='user name'><br>");
  content += F("Password:<input type='password' name='PASSWORD' placeholder='password'><br>");
  content += F("<input type='submit' name='SUBMIT' value='Submit'></form>");
  content += msg;
  content += F("<br>");
  content += F("You also can go <a href='/inline'>here</a></body></html>");

  server.send(200, F("text/html"), content);
}

//root page can be accessed only if authentication is ok
void handleRoot()
{
  String header;
  Serial.println(F("Enter handleRoot"));

  if (!is_authenticated())
  {
    server.sendHeader(F("Location"), F("/login"));
    server.sendHeader(F("Cache-Control"), F("no-cache"));
    server.send(301);

    return;
  }

  String content = F("<html><body><H2>Hello, you successfully connected to WiFiNINA on ");
  content += BOARD_NAME;
  content += F("!</H2>");
  content += F("<hr><br>");

  
  content += F("<h3>Register remote control</h3>");
  if (boolmodifreg==1){
  content += F("Click <a href=\"/adduser\">here</a> to add a new user <br>");
  content += F("Click <a href=\"/register\">here</a> to enter the register of users <br><br>");}
  else   content += F("You donnot have the authorisations to access the register <br>");

  content += F("<h3>Led remote control</h3>");
  if (boolaccess==1){
  content += F("Click <a href=\"/H\">here</a> to turn the LED on pin 9 on<br>");
  content += F("Click <a href=\"/L\">here</a> to turn the LED on pin 9 off<br><br>");
  }
  else   content += F("You donnot have the authorisations to access the LED <br>");

  content += F("<h3>URL modification</h3>");
  if (boolmodifurl==1){
  content += F("Click <a href=\"/addurl\">here</a> to change the url<br>");
  }
  else   content += F("You donnot have the authorisations to change the URL <br><br>");
  
content += F("Click <a href=\"");
content += currenturl;
content += F("\">here</a> to access the WoT url<br><br>");
/*
  if (server.hasHeader(F("User-Agent")))
  {
    content += F("<br>the user agent used is : ");
    content += server.header(F("User-Agent"));
    content += F("<br><br>");
  }*/
  
  content += F("<br><br>You can access this page until you <a href=\"/login?DISCONNECT=YES\">disconnect</a></body></html>");
  server.send(200, F("text/html"), content);
}
//---------------------------------------------------------------------------------------------------------------------------------------------------

void handleRegistre()
{
  String content=F(" ");
  
  if (server.hasHeader(F("Cookie")))
  {
    Serial.print(F("(handle register) Found cookie: "));
    String cookie = server.header(F("Cookie"));
    Serial.println(cookie);
  }

  if (is_id  && boolmodifreg){
    Serial.println("Access_Register");
    content = F("<html><body><H1>Register of authorized users</H1>");
    content += F("<hr>");

    if (reg->start == NULL) {
      Serial.println("The register is empty.\n");
      content += F("No user found : "); //détecte le bug car at least one user to enter the session
    }
      else { 
        Serial.println("List of users entered \n");
        Element* current = reg->start;
        content += F("<h3>Number of authorized users :  ");
        content += reg->nb_users;
        content += F("</h3>Users : <br>");
        while (current != NULL) {
          content += F("Username :  ");
          content += current->user->username;
          content += F("    /   Password :  ");
          content += current->user->password;
          
            if (current->user->acc_led==1)
              content += F("  This user can modifie the IoT Led state,");
            else 
              content +=F(" This user cannot access the led IoT,");
              
            if (current->user->acc_url==1)
              content += F("  + can modifie the URL,");
            else 
              content +=F(" + cannot modifie the URL,");
              
            if (current->user->acc_reg==1)
              content += F("  + can access and modifie the register");
            else 
              content +=F(" + cannot access nor modifie the register");
              
          content += F("<br><br>");
          current = current->next;
        }
     content+=F("</body></html>");
     }
    }
    else {
    content+=F("<html><body> ACCESS TO REGISTER DENIED </body></html>");
    }
    content += F("<br><hr><br>Click <a href=\"/\">here</a> to go back <br>");
    server.send(200, F("text/html"), content);
    server.send(301);
  return;
  }

//---------------------------------------------------------------------------------------------------------------------------------------------------

 void handleAdd()
{
  String content;
  String msg;
  int premier=1;
if (is_id && boolmodifreg){
  
  if (server.hasArg(F("ADDUSERNAME")) && server.hasArg(F("ADDPASSWORD")) && server.hasArg(F("ADDURL")) && server.hasArg(F("ADDACCESS")) && server.hasArg(F("ADDREG")))
  {
    int found=0; 
    Element*current=reg->start;
    while (current!=NULL && !found){
      if (server.arg(F("ADDUSERNAME"))==current->user->username){
        found=1;
        Serial.println("\nUsername already taken \n");
        msg+=F("Username already taken, please enter another username");
        }
     current=current->next;
     }
     if (found==0){
      char* toadduser=(char*)malloc(50*sizeof(char*));
      int i;
      for (i = 0; i < 50; i++) {
      toadduser[i] = server.arg(F("ADDUSERNAME"))[i];
      }
      
      char* toaddpass=(char*)malloc(50*sizeof(char*));
      for (i = 0; i < 50; i++) {
      toaddpass[i] = server.arg(F("ADDPASSWORD"))[i];
      }
      /*
      int boolaccessbis=0;
      boolaccessbis = server.arg(F("ADDACCESS"))[0];
      int boolurlbis=0;
      boolurlbis = server.arg(F("ADDURL"))[0];
      int boolregbis=0;
      boolregbis = server.arg(F("ADDREG"))[0];*/

      int boolaccessbis=0;
      if (server.arg(F("ADDACCESS"))=="1") boolaccessbis=1;
      
      int boolurlbis=0;
      if (server.arg(F("ADDURL"))=="1") boolurlbis=1;
      
      int boolregbis=0;
      if (server.arg(F("ADDREG"))=="1") boolregbis=1;
      
Serial.println("\nAdd new user booleans\t");  
Serial.print(server.arg(F("ADDACCESS")));
Serial.print(server.arg(F("ADDURL")));
Serial.print(server.arg(F("ADDREG")));
Serial.println("\n vs\t");
Serial.print(boolaccessbis);
Serial.print(boolurlbis);
Serial.print(boolregbis);
       User* newuser =build_user(toadduser,toaddpass,boolaccessbis,boolurlbis,boolregbis);
       insert_beg(reg,newuser);
       msg+=F("New user added");
       premier =0;
        
    }
  }
  else {
  if (premier=0)
  content +=F("<br>missing arguments<br>");}

  content += F("<h3>Register remote control : Add User </h3>");
  content += F("<html><body><form action='/adduser' method='POST'> Enter the following :<br>");
  content += F("Username:<input type='text' name='ADDUSERNAME' placeholder='new_username'><br>");
  content += F("Password:<input type='password' name='ADDPASSWORD' placeholder='max50char'><br>");
  content += F("Can modifie the Led IoT state :<input type='number' name='ADDACCESS' placeholder='yes=1/0=no'><br>");
  content += F("Can modifie the URL :<input type='number' name='ADDURL' placeholder='yes=1/0=no'><br>");
  content += F("Can access and modifie the register :<input type='number' name='ADDREG' placeholder='yes=1/0=no'><br>");
  content += F("<input type='submit' name='SUBMIT' value='Submit'></form>");
  content += msg;
  content += F("<br>");
  }
  else content += F("<br><hr><br> ACCESS DENIED <br>");
  content += F("<br><hr><br>Click <a href=\"/\">here</a> to go back <br>");
  server.send(200, F("text/html"), content);
  
  return;
}
//--------------__--__-_--__-_---_--_-_-_-____--____-__-_-_-_-_--__-___---_-_---_---_--
void handleURL()
{
  String content;
  String msg;
  if (is_id && boolmodifurl){
  msg += F("<br>The current URL is : ");
  msg+= currenturl;
  msg += F("<br>");
  if (server.hasArg(F("ADDURL")))
  {
      char* toaddurl=(char*)malloc(200*sizeof(char*));
      int i;
      for (i = 0; i < 200; i++) {
      toaddurl[i] = server.arg(F("ADDURL"))[i];}
      
      Serial.print(toaddurl);
      currenturl=toaddurl;
      msg+=F("<br>The URL has been changed to : ");
      //msg+=server.arg(F("ADDURL"));
      msg+=toaddurl;
      //}
  }

  content += F("<html><body><form action='/addurl' method='POST'> Enter the new link :<br>");
  content += F("URL:<input type='text' name='ADDURL' placeholder='new_reel'><br>");
  content += F("<input type='submit' name='SUBMIT' value='Submit'></form>");
  content += msg;
  content += F("<br>");
  }
  else content += F("<br><hr><br> ACCESS DENIED <br>");
  content += F("<br><hr><br>Click <a href=\"/\">here</a> to go back <br>");
  server.send(200, F("text/html"), content);
  //server.sendHeader(F("Location"), F("/reg"));
  return;
}

void handleON()
{
  String content = F(" ");
  String header;
  
  if (is_id==1 && boolaccess){
    //server.sendHeader(F("Location"), F(""));
    server.sendHeader(F("Cache-Control"), F("no-cache"));
    content = F("<html><body><form action='/H' method='GET'> Led turned On <br>");
    digitalWrite(9, HIGH);
    content += F("<br><hr><br>Click <a href=\"/\">here</a> to go back <br>");
    Serial.println(F("LED tuned ON"));
    server.send(200, F("text/html"), content);
    server.send(301); //a sup
  }
  server.sendHeader(F("Location"), F("/login"));
  content = F("<html><body><form Access denied <br>");
}

void handleOFF()
{
  String content = F(" ");
  String header;
    
  if (is_id==1 && boolaccess){
    //server.sendHeader(F("Location"), F("/"));
    server.sendHeader(F("Cache-Control"), F("no-cache"));
    content += F("<html><body><form action='/L' method='GET'> Led turned Off <br>");
    digitalWrite(9, LOW);
    content += F("<br><hr><br>Click <a href=\"/\">here</a> to go back <br>");
    Serial.println(F("LED tuned OFF"));
    server.send(200, F("text/html"), content);
  }
  server.sendHeader(F("Location"), F("/login"));
  content = F("<html><body><form Access denied <br>");
}


  void setup(void)
  {
    Serial.begin(115200);
    while (!Serial);
    Serial.print(F("\nStarting SimpleAuthentication on "));
    Serial.print(BOARD_NAME);
    Serial.print(F(" with "));
    Serial.println(SHIELD_TYPE);
    Serial.println(WIFI_WEBSERVER_VERSION);

    String fv = WiFi.firmwareVersion();

    // attempt to connect to WiFi network
    while ( status != WL_CONNECTED)
    {
      Serial.print(F("Connecting to WPA SSID: "));
      Serial.println(ssid);
      // Connect to WPA/WPA2 network
      status = WiFi.begin(ssid, pass);
    }
    updatereg(reg);
    //List* reg=listofusers();
    server.on(F("/"), handleRoot);
    server.on(F("/login"), handleLogin);
    server.on(F("/register"), handleRegistre);
    server.on(F("/adduser"), handleAdd);
    server.on(F("/H"), handleON);
    server.on(F("/L"), handleOFF);
    server.on(F("/addurl"), handleURL);
    server.on(F("/inline"), []()
    {
      server.send(200, F("text/html"), F("This works without need of authentication<br><br>Click <a href=\"/\">here</a> to go back <br>"));
    });

    //here the list of headers to be recorded
    const char * headerkeys[] = {"User-Agent", "Cookie"} ;
    size_t headerkeyssize = sizeof(headerkeys) / sizeof(char*);

    //ask server to track these headers
    server.collectHeaders(headerkeys, headerkeyssize);
    server.begin();
    Serial.print(F("HTTP server started @ "));
    Serial.println(WiFi.localIP());
    int i;
    for (i = 0; i < 200; i++) {
    currenturl[i] = WiFi.localIP()[i];
  }
  }

  void loop(void)
  {
    server.handleClient();
  }
