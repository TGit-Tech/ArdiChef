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
  DB1((F("PortsCollection.Setup( '")));DB1((json));DB1L(("' )"));
  
  // Parse the JSON text
  StaticJsonBuffer<JSON_BUFFER_SIZE> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  if(!root.success()) { DB1L(("Error parsing JSON")); return; }

  // Parse JSON
  for ( int i = 0; i < MAX_PORTS; i++ ) {
    
    const char* drv = root["arduino"][SLAVE_I2C_ADDR]["port"][i]["driver"];
    if ( drv == NULL ) break;

    if ( strcmp(drv,"ULN2003") == 0 ) {       // ULN2003 - DB logging in Constructor
      DB1((F("Port[")));DB1((i));DB1((F("] -> ")));
      int P1 = root["arduino"][SLAVE_I2C_ADDR]["port"][i]["motorpins"][0];
      int P2 = root["arduino"][SLAVE_I2C_ADDR]["port"][i]["motorpins"][1];
      int P3 = root["arduino"][SLAVE_I2C_ADDR]["port"][i]["motorpins"][2];
      int P4 = root["arduino"][SLAVE_I2C_ADDR]["port"][i]["motorpins"][3];
      mPortDriver[i] = new ULN2003Driver(P1,P2,P3,P4);
      
    } else if ( strcmp(drv,"A4988") == 0 ) {  // A4988
      DB1((F("Port[")));DB1((i));DB1((F("] -> ")));
      int stp = root["arduino"][SLAVE_I2C_ADDR]["port"][i]["steppin"];
      int dir = root["arduino"][SLAVE_I2C_ADDR]["port"][i]["dirpin"];
      mPortDriver[i] = new A4988Driver(stp,dir);
    }
    
  }
}

/******************************************************************************************
 * @brief SetupWithSDFile()
 *    Loads a JSON file (Typically Ingredients.json) from the ethernet shields SD-Card
 *    to setup the Arduino ArdiChef 'Ports' and attach required drivers to those ports.
 * @param[in] SDjsonFile - The directory and filename of the SD-Card to use for setup
 * @return void
 *****************************************************************************************/
void PortsCollection::SetupWithSDFile(char* SDjsonFile) {
  DB1((F("Ports.SetupWithSDFile( '")));DB1((SDjsonFile));DB1((F("' ) -> ")));
  
  digitalWrite(W5100_CSPIN,HIGH);  // Disable w5100 Chip Select SPI while starting SD
  if(!SD.begin(SD_CSPIN)) { DB1L((F("SD.begin() FAILED!"))); return; }   // SD.begin
  File myFile = SD.open(SDjsonFile);                              // SD.open
  
  if(!myFile) {
    DB1L((F("SD.open() FAILED!")));     // If file open FAILS then:
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

  myFile.close();DB1L((F("File.close()"))); 
  Setup(json);     // Setup Arduino Ports with the JSON file
}

/******************************************************************************************
 * @brief doloop() - Needs to be called to keep active
 *    Calls the doloop() of each 'Port' that has been setup.
 * @return void
 *****************************************************************************************/
void PortsCollection::doloop() {
  for ( int i = 0; i < MAX_PORTS; i++ ) {
    if ( mPortDriver[i] != NULL ) { mPortDriver[i]->doloop(); }
  }
}

/******************************************************************************************
 * @brief Command(DCommand)
 * @param[in] DCommand - The driver command
 * @todo Modify to support ports beyond 0-9
 *****************************************************************************************/
void PortsCollection::Command(const char* DCommand) {
  DB1((F("PortsCollection::Comand( '")));DB1((DCommand));DB1((F("' )")));

  int p = -1;
  sscanf(DCommand, "%d", &p);
  DB1((F(" -> Sending to Port: ")));DB1L((p));
  
  if (mPortDriver[p] == NULL ) { DB1L((F("Port == NULL!"))); return; }
  mPortDriver[p]->Command(DCommand);   // Send the command to the port driver
  
}

