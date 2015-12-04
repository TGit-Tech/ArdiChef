/******************************************************************************
 * Port.h - ArdiChef Port Class
 * 
 ******************************************************************************/
#include "PortsCollection.h"

//----------------------------------------------------------------------------
PortsCollection::PortsCollection() {};

void PortsCollection::PrintDirectory(File dir, int numTabs) {
  while (true) {

    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      PrintDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}

/***************************************************************************************
 * SetupmPortDriverWithSDFile(SDjsonFile) - 
 * @param SDjsonFile The /path/filename.json file to load as seen by the SD library
 * @return void
 **************************************************************************************/
void PortsCollection::Setup(char* json) {
  DBG((F("Ports.Setup( ")));DBG((json));DBGL((" )"));
  
  // Parse the JSON text
  StaticJsonBuffer<JSON_BUFFER_SIZE> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  if(!root.success()) { DBGL(("Error parsing JSON")); return; }

  // Parse JSON
  for ( int i = 0; i < MAX_PORTS; i++ ) {
    
    const char* drv = root["arduino"][BOARD_I2C_ADDR]["port"][i]["driver"];
    if ( drv == NULL ) break;

    if ( strcmp(drv,"ULN2003") == 0 ) {       // ULN2003
      DBG((F("\tPort[")));DBG((i));DBG((F("] SETUP using Driver: ")));DBGL((drv));
      int P1 = root["arduino"][BOARD_I2C_ADDR]["port"][i]["motorpins"][0];
      int P2 = root["arduino"][BOARD_I2C_ADDR]["port"][i]["motorpins"][1];
      int P3 = root["arduino"][BOARD_I2C_ADDR]["port"][i]["motorpins"][2];
      int P4 = root["arduino"][BOARD_I2C_ADDR]["port"][i]["motorpins"][3];
      //DBG(("\t\tPin1="));DBGL((P1));
      //DBG(("\t\tPin2="));DBGL((P2));
     // DBG(("\t\tPin3="));DBGL((P3));
      //DBG(("\t\tPin4="));DBGL((P4));
      mPortDriver[i] = new ULN2003Driver(P1,P2,P3,P4);
      
    } else if ( strcmp(drv,"A4988") == 0 ) {  // A4988
      DBG((F("\tPort[")));DBG((i));DBG((F("] SETUP using Driver: ")));DBGL((drv));
      int stp = root["arduino"][BOARD_I2C_ADDR]["port"][i]["steppin"];
      int dir = root["arduino"][BOARD_I2C_ADDR]["port"][i]["dirpin"];
     // DBG(("\t\tsteppin = "));DBGL((stp));
     // DBG(("\t\tdirpin = "));DBGL((dir));
      mPortDriver[i] = new A4988Driver(stp,dir);
    }
    
  }
}

/******************************************************************************************
 * SetupmPortDriverWithSDFile(SDjsonFile) - 
 *    - Load a JSON file from the SD-Card to setup mPortDriver on the arduino
 * 
 *****************************************************************************************/
void PortsCollection::SetupWithSDFile(char* SDjsonFile) {
  DBG((F("Ports.SetupWithSDFile( ")));DBG((SDjsonFile));DBGL((F(" )")));
  
  digitalWrite(10,HIGH);  // Disable w5100 Chip Select SPI while starting SD
  if(!SD.begin(4)) { DBGL((F("SD.begin() FAILED!"))); return; }   // SD.begin
  File myFile = SD.open(SDjsonFile);                              // SD.open
  
  if(!myFile) {
    DBGL((F("SD.open() FAILED!")));     // If file open FAILS then:
    myFile.close();                     //  File.Close()
    File aFile = SD.open("/");          //  Open root directory
    PrintDirectory(aFile,2);            //  Print files on SD-Card
    aFile.close();                      //  Close root directory
    return; }                           //  exit

  // Read the File into json[]; remove all un-necessary characters
  char json[myFile.size() + 2]; int i = 0;          // Initialize memory
  while(myFile.available()) {
    int c = myFile.read();                          // Read a character
    if ( c > 32 && c < 126 ) { json[i] = c; i++; }  // Append qualified characters
  } json[i] = '\0';                                 // Terminate the string

  myFile.close();DBGL((F("File.close()"))); 
  Setup(json);     // Setup Arduino Ports with the JSON file
  
  //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
}

void PortsCollection::doloop() {
  for ( int i = 0; i < MAX_PORTS; i++ ) {
    if ( mPortDriver[i] != NULL ) { mPortDriver[i]->doloop(); }
  }
}

void PortsCollection::Command(const char* DriverCommand) {
  DBG((F("PortsCollection::Comand( '")));DBG((DriverCommand));DBGL((F("' )")));
  
  int p = atoi(&DriverCommand[0]);     // Grab port# (ONLY SUPmPortDriver 0-9!!!)
  DBG((F("Sending Command to PortDriver#: ")));DBGL((p));
  
  if (mPortDriver[p] == NULL ) { DBGL((F("Port == NULL!"))); return; }
  mPortDriver[p]->Command(DriverCommand);   // Send the command to the port driver
}

