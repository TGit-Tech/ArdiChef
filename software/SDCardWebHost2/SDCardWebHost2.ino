/******************************************************************
 * Program:     WebServerST - PLUS - v1.0
 * 
 * Description: 
 *    SD Web server for Arduino.  Specifically modified for the 
 *    ArdiChef project.  see http://www.ArdiChef.com
 *              
 * Setup:       
 *    Arduino Mega2560
 *    Desloo Ethernet Shield w/5100 Wiznet
 * 
 * Authors:
 *    SurferTim   11/2013         Original Posting
 *    ThomasG     11/2015         Simplified pre-processor debug
 *    ThomasG     11/2015         Large file name and DHCP support
 * ****************************************************************/
#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>
#include <utility/w5100.h>
#include <utility/socket.h>

#define DEBUG   // Comment to disable serial debug (saves ~1.6K)
#ifdef DEBUG
  #define DBGL(x) Serial.println x
  #define DBG(x) Serial.print x
#else
  #define DBGL(x)
  #define DBG(x)
#endif

//--------------------- NETWORK SETTINGS --------------------------
bool UseDHCP = true;                // Attempt to use DHCP setup
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEC };  // this must be unique
IPAddress ip( 192,168,0,25 );
IPAddress gateway( 192,168,0,1 );
IPAddress subnet( 255,255,255,0 );
EthernetServer server(80);
//-----------------------------------------------------------------

File root;
const char legalChars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890/.-_~";
unsigned int requestNumber = 0;
unsigned long connectTime[MAX_SOCK_NUM];
int loopCount = 0;

int freeRam() {
  extern int __heap_start,*__brkval;
  int v;
  return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int) __brkval);  
}
//=================================================================================================
void setup()
{
  Serial.begin(115200);

  // disable w5100 SPI while starting SD
  digitalWrite(10,HIGH);

  DBG((F("Starting SD..")));
  if(!SD.begin(4)) DBGL((F("failed"))); else DBGL((F("ok")));

  //---------- Start Ethernet Connection --------------------------------
  if(UseDHCP){ 
    if (Ethernet.begin(mac) == 0) {
      DBGL(("Failed to configure Ethernet using DHCP"));
      UseDHCP = false; } //Default to Static Settings
  }
  if(!UseDHCP) Ethernet.begin(mac, ip, gateway, gateway, subnet);
  
  DBG(("Server IP address: "));  // Print local IP address
  
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    DBG((Ethernet.localIP()[thisByte], DEC)); DBG(("."));
  } DBGL(());
  
  delay(2000);
  server.begin();
  
  unsigned long thisTime = millis();
  for(int i=0;i<MAX_SOCK_NUM;i++) { connectTime[i] = thisTime; }
  DBGL((F("Ready")));
}

//=================================================================================================
void loop() {
  checkServer();
  myStuff();
}

void myStuff() {
  if(Serial.available()) { if(Serial.read() == 'r') ShowSockStatus(); }
  checkSockStatus();
}

//=================================================================================================
void checkServer() {

  EthernetClient client = server.available();
  if(client) {
    boolean currentLineIsBlank = true;
    boolean currentLineIsGet = true;
    int tCount = 0;
    char tBuf[65];
    int r,t;
    char *pch;
    char ext[5] = "";
    char methodBuffer[8];
    char requestBuffer[48];
    char pageBuffer[48];
    char paramBuffer[48];
    char protocolBuffer[9];
    char fileName[32];
    char fileType[4];
    int clientCount = 0;

    requestNumber++;
    DBG((F("\r\nClient request #"))); DBG((requestNumber)); DBG((F(": ")));
    int loopCount = 0;              // this controls the timeout

    while (client.connected()) {
      while(client.available()) {
        // if packet, reset loopCount  //        loopCount = 0;

        char c = client.read();
        if(currentLineIsGet && tCount < 63)
        {
          tBuf[tCount] = c;
          tCount++;
          tBuf[tCount] = 0;
        }

        if (c == '\n' && currentLineIsBlank) {
          DBG((tBuf));
//        DBG((F("POST data: ")));

          while(client.available()) client.read();

          int scanCount = sscanf(tBuf,"%7s %95s %8s",methodBuffer,requestBuffer,protocolBuffer);
          if(scanCount != 3) {
            DBGL((F("bad request")));
            sendBadRequest(client);
            return;
          }

          char* pch = strtok(requestBuffer,"?");
          if(pch != NULL) {
            strncpy(fileName,pch,63);
            strncpy(tBuf,pch,63);
            pch = strtok(NULL,"?");
            if(pch != NULL) strcpy(paramBuffer,pch);
            else paramBuffer[0] = 0;
          }

          strtoupper(requestBuffer);
          strtoupper(tBuf);

          for(int x = 0; x < strlen(requestBuffer); x++) {
            if(strchr(legalChars,requestBuffer[x]) == NULL) {
              DBGL((F("bad character")));  
              sendBadRequest(client);
              return;
            }            
          }

          // Get 'fileType'
          DBG((F("file = ")));DBGL((requestBuffer));
          pch = strtok(tBuf,".");
          if(pch != NULL) {
            pch = strtok(NULL,".");
            if(pch != NULL) strncpy(fileType,pch,4);
            else fileType[0] = 0;
            DBG((F("file type = ")));DBGL((fileType));
          }
          
          DBG((F("method = "))); DBGL((methodBuffer));
          if(strcmp(methodBuffer,"GET") != 0 && strcmp(methodBuffer,"HEAD") != 0) {
            sendBadRequest(client);
            return;
          }
          
          DBG((F("params = "))); DBGL((paramBuffer));
          DBG((F("protocol = "))); DBGL((protocolBuffer));
          
          // if dynamic page name (this code looks in-complete
          if(strcmp(requestBuffer,"/MYTEST.PHP") == 0) {
            DBGL((F("dynamic page")));            
          }
          else {
            //----------- PARSE ----------------------
            
            // Home page
            if(strcmp(fileName,"/") == 0) {
              strcpy(fileName,"/INDEX.HTM");
              strcpy(fileType,"HTM");
              DBG((F("Home page ")));            
            }

            // File Name (Modify for 8.3)
            DBG((F("SD file = "))); DBGL((fileName));
            pch = strrchr(fileName,'.');                          //ptr to extension
            if(pch>0){memcpy(ext,pch,4);pch[0]='\0';}             //ext=4-digit copy, truncate name
            pch = strrchr(fileName,'/');                          //ptr to filename
            if(strlen(pch)>9) pch[7]='~';pch[8]='1';pch[9]='\0';  //Truncate fileName to 8-digits
            strcat(fileName,ext);                                 //Concate extension
            DBG((F("SD file(8.3) = "))); DBGL((fileName));
            
            // Check FileName
            if(strlen(fileName) > 30) {
              DBGL((F("filename too long")));
              sendBadRequest(client);
              return;
            }
            else if(strlen(fileType) > 3 || strlen(fileType) < 1) {
              
              DBGL((F("file type invalid size")));
              sendBadRequest(client);
              return;
            }
            else {
              
              DBGL((F("filename format ok")));
              if(SD.exists(fileName)) {
                // SRAM check
                DBG((F("SRAM = ")));
                DBGL((freeRam()));
                DBG((F("file found..")));                
                File myFile = SD.open(fileName);
                if(!myFile) {
                  DBGL((F("open error")));
                  sendFileNotFound(client);
                  return;
                }
                else DBG((F("opened..")));
                
                strcpy_P(tBuf,PSTR("HTTP/1.0 200 OK\r\nContent-Type: "));
                // send Content-Type
                if(strcmp(fileType,"HTM") == 0) strcat_P(tBuf,PSTR("text/html"));
                else if(strcmp(fileType,"PHP") == 0) strcat_P(tBuf,PSTR("text/html"));
                else if(strcmp(fileType,"TXT") == 0) strcat_P(tBuf,PSTR("text/plain"));
                else if(strcmp(fileType,"CSS") == 0) strcat_P(tBuf,PSTR("text/css"));
                else if(strcmp(fileType,"GIF") == 0) strcat_P(tBuf,PSTR("image/gif"));
                else if(strcmp(fileType,"JPG") == 0) strcat_P(tBuf,PSTR("image/jpeg"));
                else if(strcmp(fileType,"JS") == 0) strcat_P(tBuf,PSTR("application/javascript"));
                else if(strcmp(fileType,"ICO") == 0) strcat_P(tBuf,PSTR("image/x-icon"));
                else if(strcmp(fileType,"PNG") == 0) strcat_P(tBuf,PSTR("image/png"));
                else if(strcmp(fileType,"PDF") == 0) strcat_P(tBuf,PSTR("application/pdf"));
                else if(strcmp(fileType,"ZIP") == 0) strcat_P(tBuf,PSTR("application/zip"));
                else strcat_P(tBuf,PSTR("text/plain"));

                strcat_P(tBuf,PSTR("\r\nConnection: close\r\n\r\n"));
                client.write(tBuf);

                if(strcmp(methodBuffer,"GET") == 0)  {
                  DBG((F("send..")));
                  while(myFile.available()) {
                    clientCount = myFile.read(tBuf,64);
                    client.write((byte*)tBuf,clientCount);
                  }
                }
                myFile.close();              
                DBGL((F("closed")));
                client.stop();                
                DBGL((F("disconnected")));
                return;
                
              }
              else {

                DBGL((F("File not found")));
                sendFileNotFound(client);
                root = SD.open("/");
                printDirectory(root, 3);
                return;
              }

            }
          }

          pch = strtok(paramBuffer,"&");

          while(pch != NULL)
          {
            if(strncmp(pch,"t=",2) == 0)
            {
              t = atoi(pch+2);

              DBG(("t="));
              DBGL((t,DEC));            

            }

            if(strncmp(pch,"r=",2) == 0)
            {
              r = atoi(pch+2);

              DBG(("r="));              
              DBGL((r,DEC));

            }


            pch = strtok(NULL,"& ");
          }

          DBGL((F("Sending response")));


          strcpy_P(tBuf,PSTR("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"));
          client.write(tBuf);

          if(strcmp(methodBuffer,"GET") == 0) {

            strcpy_P(tBuf,PSTR("<html><head><script type=\"text/javascript\">"));
            client.write(tBuf);
            strcpy_P(tBuf,PSTR("function show_alert() {alert(\"This is an alert\");}"));
            client.write(tBuf);
            strcpy_P(tBuf,PSTR("</script></head>"));
            client.write(tBuf);

            strcpy_P(tBuf,PSTR("<body><H1>TEST</H1><form method=GET onSubmit=\"show_alert()\">"));
            client.write(tBuf);
            strcpy_P(tBuf,PSTR("T: <input type=text name=t><br>"));
            client.write(tBuf);
            strcpy_P(tBuf,PSTR("R: <input type=text name=r><br><input type=submit></form></body></html>"));
            client.write(tBuf);
          }

          client.stop();
        }
        else if (c == '\n') {
          currentLineIsBlank = true;
          currentLineIsGet = false;
        }
        else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }

      loopCount++;

      // if 1000ms has passed since last packet
      if(loopCount > 1000) {
        // close connection
        client.stop();

        DBGL(("\r\nTimeout"));

      }

      // delay 1ms for timeout timing
      delay(1);
    }

    DBGL((F("disconnected")));

  }
}

void sendFileNotFound(EthernetClient thisClient) {
  char tBuf[64];
  strcpy_P(tBuf,PSTR("HTTP/1.0 404 File Not Found\r\n"));
  thisClient.write(tBuf);
  strcpy_P(tBuf,PSTR("Content-Type: text/html\r\nConnection: close\r\n\r\n"));
  thisClient.write(tBuf);
  strcpy_P(tBuf,PSTR("<html><body><H1>FILE NOT FOUND</H1></body></html>"));
  thisClient.write(tBuf);
  thisClient.stop();  

  DBGL((F("disconnected")));

}

void sendBadRequest(EthernetClient thisClient) {
  char tBuf[64];
  strcpy_P(tBuf,PSTR("HTTP/1.0 400 Bad Request\r\n"));
  thisClient.write(tBuf);
  strcpy_P(tBuf,PSTR("Content-Type: text/html\r\nConnection: close\r\n\r\n"));
  thisClient.write(tBuf);
  strcpy_P(tBuf,PSTR("<html><body><H1>BAD REQUEST</H1></body></html>"));
  thisClient.write(tBuf);
  thisClient.stop();  

  DBGL((F("disconnected")));

}

void  strtoupper(char* aBuf) {

  for(int x = 0; x<strlen(aBuf);x++) {
    aBuf[x] = toupper(aBuf[x]);
  }
}

byte socketStat[MAX_SOCK_NUM];

void ShowSockStatus()
{
  for (int i = 0; i < MAX_SOCK_NUM; i++) {
    DBG((F("Socket#")));
    DBG((i));
    uint8_t s = W5100.readSnSR(i);
    socketStat[i] = s;
    DBG((F(":0x")));
    DBG((s,16));
    DBG((F(" ")));
    DBG((W5100.readSnPORT(i)));
    DBG((F(" D:")));
    uint8_t dip[4];
    W5100.readSnDIPR(i, dip);
    for (int j=0; j<4; j++) {
      DBG((dip[j],10));
      if (j<3) DBG(("."));
    }
    DBG((F("(")));
    DBG((W5100.readSnDPORT(i)));
    DBGL((F(")")));
  }
}

void checkSockStatus()
{
  unsigned long thisTime = millis();

  for (int i = 0; i < MAX_SOCK_NUM; i++) {
    uint8_t s = W5100.readSnSR(i);

    if((s == 0x17) || (s == 0x1C)) {
        if(thisTime - connectTime[i] > 30000UL) {
          DBG((F("\r\nSocket frozen: ")));
          DBGL((i));
          close(i);
        }
    }
    else connectTime[i] = thisTime;

    socketStat[i] = W5100.readSnSR(i);
  }
}

void printDirectory(File dir, int numTabs) {
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
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}
