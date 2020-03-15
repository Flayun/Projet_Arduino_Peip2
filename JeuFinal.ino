  //Bluetooth 
    #include<SoftwareSerial.h>
    #include<Servo.h>
    #define RX 12
    #define TX 13
    SoftwareSerial BlueT(RX,TX);
    char data;
    int coordXD=0;
    int coordYD=0;
    int coordXA=0;
    int coordYA=0;


  //Moteur 
    const byte stepPin = 2; 
    const byte dirPin = 3; 
    const byte enable = 4;
    const byte dirPin2 = 5;
    const byte stepPin3 = 6;
    const byte dirPin3 = 7;
    const byte enable3 = 8;  
    const byte stepPin4 = 10;
    const byte dirPin4 = 11;

  //ServoMoteur
    Servo servo;
    const byte pinServo = 9;

  //Tableau echiquier
    int jeu[8][8] ={
    {1,1,1,2,2,1,1,1},
    {1,1,1,1,1,1,1,1},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {1,1,1,1,1,1,1,1},
    {1,1,1,2,2,1,1,1},
    };

  //--------------------------------------
  void setup() {
    Serial.begin(9600);

    //Bluetooth
    BlueT.begin(9600);

    //Servomoteur (pince)
    servo.attach(pinServo);

    //Réglage des pins en Sortie
    pinMode(stepPin,OUTPUT); 
    pinMode(dirPin,OUTPUT);
    pinMode(enable,OUTPUT);
    pinMode(dirPin2,OUTPUT);
    pinMode(dirPin3,OUTPUT);
    pinMode(stepPin3,OUTPUT);
    pinMode(enable3,OUTPUT);
    pinMode(stepPin4,OUTPUT); 
    pinMode(dirPin4,OUTPUT);
    pinMode(pinServo,OUTPUT);

    
    digitalWrite(enable,HIGH);  //Met tous les moteurs hors tension 
    digitalWrite(enable3,HIGH);  
    delay(500); 

    remonterPinceDepart();   

  }
  //-----------------------------------------

  void loop() {
    //on remet les coordonnées à 0 à chaque tour de boucle pour éviter de faire tourner les moteurs en continu
    int coordXD=-1;
    int coordYD=-1;
    int coordXA=-1;
    int coordYA=-1;
    char movement;

    //On récupère les données du Bluetooth
    if (BlueT.available()) {  
      data = (char)BlueT.read();
      
      if (data=='*'){    
        coordXD = quelNbrCase((char)BlueT.read());
        coordYD = quelNbrCase((char)BlueT.read());        
        movement = (char)BlueT.read();
        coordXA = quelNbrCase((char)BlueT.read());
        coordYA = quelNbrCase((char)BlueT.read());   
      }
      
    }

    //On utilise les coordonnées, de ce fait on ne bouge les moteurs que si une commande a été lancé sur le Bluetooth
    if (coordXD!=-1) {
      
      //On vérifie la présence d'une pièce sur la case d'arriver et si c'est le cas on l'enleve 
      if (verifierPiecePrise(coordXA, coordYA)) {
        piecePrise(coordXA, coordYA);        
      }
      
      avancer_X_caseX(coordXD);
      avancer_X_caseY(coordYD);
      delay(2000);
      ouvrirPince();
      delay(500);
      descendreLaPince(coordXD, coordYD);
      delay(2000);
      fermerPince();
      delay(500);
      remonterLaPince(coordXD, coordYD);
      delay(500);
      avancer_X_caseX(coordXA-coordXD);
      avancer_X_caseY(coordYA-coordYD);
      delay(2000);
      descendreLaPince(coordXD, coordYD);
      delay(2000);
      ouvrirPince();
      delay(500);
      remonterLaPince(coordXD,coordYD);
      delay(500);
      fermerPince();
      delay(1000);
      revenirAlOrigine(coordXA,coordYA);
      changerPiecePlateau(coordXD, coordYD, coordXA, coordYA);

    }
    
    delay(1000); //On attend un peu pour ralentir le temps de la boucle
  }

  //**********************************************fonction de verification*******************************
  boolean verifierPiecePrise(int x, int y){    
    if(jeu[y][x] == 1 || jeu[y][x] == 2){
      return true; 
    }
    
    else{
      return false;
    }
  }

  void piecePrise(int x, int y){
    avancer_X_caseX(x);
    avancer_X_caseY(y);
    descendreLaPince(x, y);
    fermerPince();
    remonterLaPince(x, y);
    revenirAlOrigine(x+2, y);
    ouvrirPince();
    avancer_X_caseX(2);
  }

  boolean verifierTaille(int x, int y){    
    if(jeu[y][x] == 1){
      return true;//piece de petite taille
    }
    
    else if(jeu[y][x] == 2){
      return false;//piece de grande taille
    }
  }

  void changerPiecePlateau(int xD, int yD, int xA, int yA) {
    jeu[yA][xA]=jeu[yD][xD];
    jeu[yD][xD]=0;
  }


  //**********************************fonction moteurs*********************************************
  void avancer_X_caseX(int x){
    int nbrPas = x*50+0;
    digitalWrite(enable,LOW);
    digitalWrite(dirPin,HIGH);
    digitalWrite(dirPin2,LOW);

    for(int i=0; i<nbrPas;i++){
      digitalWrite(stepPin,HIGH);
      delay(5);
      digitalWrite(stepPin,LOW);
      delay(5);
    }

    digitalWrite(enable,HIGH);
  
  }

  void avancer_X_caseY(int y){
    int nbrPas = y*100;
    digitalWrite(enable3,LOW);
    digitalWrite(dirPin3,HIGH);

    for(int i=0; i<nbrPas;i++){
      digitalWrite(stepPin3,HIGH);
      delay(5);
      digitalWrite(stepPin3,LOW);
      delay(5);
    }
    
    digitalWrite(enable3,HIGH);
  }


  void revenirAlOrigine(int x, int y){
    int nbrPasX = x*50;
    int nbrPasY = y*100;
    digitalWrite(enable,LOW);
    digitalWrite(enable3,LOW);
    digitalWrite(dirPin,LOW);
    digitalWrite(dirPin2,HIGH);
    digitalWrite(dirPin3,LOW);

    for(int i=0; i<nbrPasX;i++){
      digitalWrite(stepPin,HIGH);
      delay(5);
      digitalWrite(stepPin,LOW);
      delay(5);
    }
    
    for(int i=0; i<nbrPasY;i++){
      digitalWrite(stepPin3,HIGH);
      delay(5);
      digitalWrite(stepPin3,LOW);
      delay(5);
    }
    
    digitalWrite(enable,HIGH);
    digitalWrite(enable3,HIGH);
  }

  int quelNbrCase(char a){
    
    if(a =='a' || a == '1'){
      return 1;
    }
    
    if(a =='b' || a == '2'){
      return 2;
    }
    
    if(a == 'c' || a == '3'){
      return 3;
    }
    
    if(a == 'd' || a == '4'){
      return 4;
    }
    
    if(a == 'e' || a == '5'){
      return 5;
    }
    
    if(a =='f' || a == '6'){
      return 6;
    }
    
    if(a == 'g' || a == '7'){
      return 7;
    }
    
    if(a == 'h' || a == '8'){
      return 8;
    }
  }

  //**********************************fonction pince*********************************************
  void remonterPinceDepart() {
    digitalWrite(dirPin4,LOW);

    for(int i=0; i<300;i++){ //eventuellement changer les valeurs pour descendre plus ou moins bas
      digitalWrite(stepPin4,HIGH);
      delay(2);
      digitalWrite(stepPin4,LOW);
      delay(2);
    }
  }

  void descendreLaPince(int x, int y){ //coordonnées de la pince
    digitalWrite(dirPin4,HIGH);
    
    if(verifierTaille(x, y)){
      for(int i=0; i<340;i++){ //eventuellement changer les valeurs pour descendre plus ou moins bas
        digitalWrite(stepPin4,HIGH);
        delay(5);
        digitalWrite(stepPin4,LOW);
        delay(5);
      }
    }
    
    else{
      for(int i=0; i<300;i++){//eventuellement changer les valeurs pour descendre plus ou moins bas
        digitalWrite(stepPin4,HIGH);
        delay(5);
        digitalWrite(stepPin4,LOW);
        delay(5);
      }
    }
  }

  void remonterLaPince(int x, int y){
    digitalWrite(dirPin4,LOW);
    
    if(verifierTaille(x, y)){
      for(int i=0; i<340;i++){ //eventuellement changer les valeurs pour descendre plus ou moins bas
        digitalWrite(stepPin4,HIGH);
        delay(5);
        digitalWrite(stepPin4,LOW);
        delay(5);
      }
    }
    
    else{
      for(int i=0; i<300;i++){ //eventuellement changer les valeurs pour descendre plus ou moins bas
        digitalWrite(stepPin4,HIGH);
        delay(5);
        digitalWrite(stepPin4,LOW);
        delay(5);
      }
    }
    
    digitalWrite(dirPin4,HIGH);
  }

  void fermerPince(){
    servo.write(80);
  }

  void ouvrirPince(){
    servo.write(30);
  }
