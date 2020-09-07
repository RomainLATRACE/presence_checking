#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <DYE_Fingerprint.h>

//pin 62: fil vert pour l'entrée du leteur 
//pin 63; blanc pour la sortie de l'arduino
SoftwareSerial mySerial(62, 63);
DYE_Fingerprint finger = DYE_Fingerprint(&mySerial);

LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

#define RST_PIN 5
#define SS_PIN 53
MFRC522 module_rfid(SS_PIN, RST_PIN);

//je defini les entrées pour savoir dans quel sens on tourne l'encodeur :
#define clk 2 
#define data 3

//variables pour le menu
int i = 0;
int initrotation;
int rotation;

//variables pour les id
int counter = 0; 
int State;
int LastState;

int bouton1 = 4;
int bouton2 = 6;

uint8_t id;
int val=0;
int buzzerPin=64;



void setup() {
  lcd.begin(16, 2);
  SPI.begin();
  module_rfid.PCD_Init();
  lcd.print("Demarage...");
  Serial.begin(9600);
  pinMode(buzzerPin,OUTPUT);//initialize the buzzer pin as an output
  while (!Serial);
  delay(100);
  Serial.println("\n\nEn attente du lecteur d'empreinte");

  // set the data rate for the sensor serial port
  finger.begin(57600);
  
  if (finger.verifyPassword()) {
    Serial.println("Lecteur d'empreinte trouvé !");
    tone(buzzerPin, 560, 100);  // tone(Pin, Freq, Durée)
    delay(100); // temps de pause
    tone(buzzerPin, 750, 100);
    delay(100);
    tone(buzzerPin, 1125, 100);
    delay(100);
    tone(buzzerPin, 1500, 100);
    delay(100);
    tone(buzzerPin, 3000, 100);
    delay(100);
    tone(buzzerPin, 4000, 100);
    delay(100);
    digitalWrite(buzzerPin,LOW);
  } else {
    Serial.println("Pas de lecteur d'empreinte déteté :(");
    while (1) { delay(1); }
  }
  lcd.clear();
  lcd.print("Dispositif");
  lcd.setCursor(0, 1);
  lcd.print("d'identification");
  delay(3000);
  lcd.clear();
  
  String Nom = "";
  String Etat = "";
  Etat = "ACCES REFUSE";
  
}

void loop() {
  
  lcd.print("Scannez votre");
  lcd.setCursor(0, 1);
  lcd.print("carte ou badge...");

  String Nom = "";
  String Etat = "";
  Etat = "ACCES REFUSE";
  
  while (Etat != "ACCES AUTORISE"){
  
    
  if ( ! module_rfid.PICC_IsNewCardPresent()){
    return;
  }

  if ( ! module_rfid.PICC_ReadCardSerial()){
    return;
  }

  String UID = "";
  for (byte i = 0; i < module_rfid.uid.size; i++) {
    UID.concat(String(module_rfid.uid.uidByte[i] < 0x10 ? " 0" : " "));
    UID.concat(String(module_rfid.uid.uidByte[i], HEX));
  }

  UID.toUpperCase();


   //////JUSTE POUR CONNAITRE L'ID///////
     lcd.clear();
      lcd.print(UID.substring(1));
      delay(1000);
  ///////////////////////////////////////
      
  if (UID.substring(1) == "29 BD 02 DB"){
    Etat = "ACCES AUTORISE";
    Nom = "M. LATRACE";
  }


    if (UID.substring(1) == "21 44 02 DB"){
    Etat = "ACCES AUTORISE";
    Nom = "M.GRARI";

  }

  if (UID.substring(1) != "21 44 02 DB" && UID.substring(1) != "29 BD 02 DB"){
    Etat = "ACCES REFUSE";
      }
  lcd.clear();
  //lcd.setCursor(0, 0);
  lcd.print (Etat.substring(0));
  lcd.setCursor(0, 1);
  lcd.print(Nom.substring(0));
  delay (2000);
  lcd.clear();
      
  }

////////////////////MENU////////////////////////////////////////////////////////////////////////////
  lcd.clear();
  initrotation = digitalRead(clk);
  while(digitalRead(bouton1)!=HIGH){
  Serial.print(i);
  //on affecte une valeur à "i" suivant le sens de rotation de l'encoder
  rotation = digitalRead(clk);
  
  if (rotation != initrotation){

    if (digitalRead(data) != rotation){
      i = i-1;
      }else{
        i = i+1;
        }
  }
  
  initrotation = rotation;
  

  //Contruction du menu// il est possible de régler la sensibilité de navigation en jouant avec les conditions dans les "if"
  if ((i >= 0)&&(i < 2)){
  lcd.setCursor(0, 0);
  lcd.print("> Faire l'appel    ");
  lcd.setCursor(0, 1);
  lcd.print("Ajout empreinte     ");
  }
  
    if ((i >= 2)&&(i < 4)){
  lcd.setCursor(0, 0);
  lcd.print(">Ajout empreinte");
  lcd.setCursor(0, 1);
  lcd.print("Suppr empreinte");
  }
  
      if ((i >= 4)&&(i < 6)){
  lcd.setCursor(0, 0);
  lcd.print(">Suppr empreinte");
  lcd.setCursor(0, 1);
  lcd.print(" EXIT                ");
  }

      if ((i >= 6)&&(i < 8)){
  lcd.setCursor(0, 0);
  lcd.print("Suppr empreinte");
  lcd.setCursor(0, 1);
  lcd.print(">EXIT                ");
  }


  //on définit une limite spérieure//
    if (i > 8){
      i=8;
    }

  //on définit une limite inférieure//
   if (i < 0){
      i=0;
   }
  }


  lcd.clear();
  
  if ((i >= 0)&&(i < 2)){
    Comparer_Emp();
    lcd.clear();
    delay(1000);
  }
  
  if ((i >= 2)&&(i < 4)){
    Ajouter_Emp();
    lcd.clear();
    delay(1000);
  }
  
  if ((i >= 4)&&(i < 6)){
    Supprimer_Emp();
    lcd.clear();
    delay(1000);
  }

    if ((i >= 6)&&(i < 8)){
      lcd.clear();
      lcd.print("Retour");
      lcd.setCursor(7,1);
      lcd.print("au scan");
      delay(3000);
      lcd.clear();
  }

}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////FONCTIONS/////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//fonction à appeller lorsqu'on veut entrer et afficher une id, on la select avec le bouton push
int Variation_id(){
  lcd.print("Saisissez l'id:");
  LastState = digitalRead(clk);
  while(digitalRead(bouton2)!=HIGH){
    State=digitalRead(clk);
    if (State != LastState){
      if (digitalRead(data) != State) { 
         counter ++;
      } else {
         counter --;
      }
    }
    lcd.setCursor(0,1);
    lcd.print(counter);
    LastState = State;
    // on fixe une limite au counter
    if(counter > 127){
      counter=127;
    }
    if(counter < 1){
      counter=1;
    }
  }
  return counter; 
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
int BuzzerFonctionValidation(){ // il est possible de jouer sur la fréquence (donc la tonalité) du buzzer en ajustant les delay
  tone(buzzerPin, 1500, 100);
  delay(250);
  tone(buzzerPin, 3800, 100);
  delay(100);
  digitalWrite(buzzerPin,LOW);
  return 0;
  }



//Fonction pour ajouter une empreinte////////////////////////////////////////////////////////////////////////////////////////
int Ajouter_Emp(){
  lcd.clear();
  Serial.println("Prêt à enregistrer une empreinte");
  Serial.println("Veuillez saisir le numéro d'identification (de 1 à 127) sous lequel vous souhaitez enregistrer le doigt... ");
  
  id = Variation_id();
  
  Serial.print("Enregistrement de l'ID #");
  Serial.println(id);

  while (!  getFingerprintEnroll() );
  return 0;
}


uint8_t getFingerprintEnroll() {

  int p = -1;
  lcd.clear();
  lcd.print("Posez votre doigt");

  
  Serial.print("En attente d'un doigt valide pour s'inscrire #"); Serial.println(id);
  int m=0;
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(".");
      lcd.setCursor(m,1);
      lcd.print(".");
      lcd.setCursor(m,1);
      lcd.print(".");
      lcd.setCursor(m,1);
      lcd.print(".");
      m ++;
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  lcd.clear();
  lcd.print("Retirez le doigt");
  Serial.println("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("Enregistrement de l'ID "); Serial.println(id);
  p = -1;
  lcd.clear();
  lcd.print("Replacez le doigt");
  Serial.println("Place same finger again");
  m=0;
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      Serial.println(".");
      lcd.setCursor(m,1);
      lcd.print(".");
      lcd.setCursor(m,1);
      lcd.print(".");
      lcd.setCursor(m,1);
      lcd.print(".");
      m ++;
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  // OK converted!
  Serial.print("Creating model for #");  Serial.println(id);
  
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
  
  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    lcd.clear();
    lcd.print("id ");
    lcd.setCursor(3,0);
    lcd.print(id);
    lcd.setCursor(0,1);
    lcd.print("enregistree :)");
    BuzzerFonctionValidation();    
    Serial.println("Stored!");
    delay(3000);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
//fonction de suppression 
int Supprimer_Emp(){
  lcd.clear();
  Serial.println("Please type in the ID # (from 1 to 127) you want to delete...");

  id = Variation_id();
  deleteFingerprint(id);
  lcd.clear();
  lcd.print("id ");
  lcd.setCursor(3,0);
  lcd.print(id);
  lcd.setCursor(0,1);
  lcd.print("supprimee :)");
  BuzzerFonctionValidation();
  Serial.println("Terminée");
  Serial.println(id);
  delay(3000);
  return 0;
}

uint8_t deleteFingerprint(uint8_t id) {
  uint8_t p = -1;
  
  p = finger.deleteModel(id);

  if (p == FINGERPRINT_OK) {
    Serial.println("Deleted !");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not delete in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.print("Unknown error: 0x"); Serial.println(p, HEX);
    return p;
  }   
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//fonction pour savoir si l'empreinte exsiste déjà
int Comparer_Emp(){
  lcd.clear();
  lcd.print("Posez votre doigt");
  while (digitalRead(bouton2)!=HIGH) {
    getFingerprintIDez();
    delay(1000);
  }
  delay(500);
  return 0;
}

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  // OK converted!
  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
    
  } 
  else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } 
  else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return p;
  }
  else {
    Serial.println("Unknown error");
    return p;
  }   
  
  // found a match!
  //Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  //Serial.print(" with confidence of "); Serial.println(finger.confidence); 

  Serial.print(finger.fingerID);
  
  return finger.fingerID;
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;
  
  // found a match!
  BuzzerFonctionValidation();
  Serial.print("Found ID #");
  lcd.setCursor(0,1);
  lcd.print("id: ");
  lcd.setCursor(4,1);
  lcd.print(finger.fingerID); 
  Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); 
  Serial.println(finger.confidence);
  //faudrait pouvoir envoyer l'id de l'empreinte au serveur
  //et recevoir le nom du propiétaire pour l'afficher sur l'écran
  delay(2000);
  lcd.setCursor(0,1);
  lcd.print("             ");
  return finger.fingerID;
}






