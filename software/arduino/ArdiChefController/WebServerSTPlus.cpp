#include "WebServerSTPlus.h"
const char legalChars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890/.-_~";

/***********************************************************************************//**
 * @brief WebServerSTPlus() Initializer
 *    Once the WebServer is initialezed network communications are active
 * @param[in] mac - The mac address of the WebServer; must be unique on network
 * @param[in] ip - The ip address for the WebServer if DHCP is not used
 * @param[in] gateway - Set the networks gateway
 * @param[in] subnet - Set the networks subnet mask
 * @param[in] UseDHCP - Attempt to connect using DHCP (IP Address will be dynamic)
 * @param[in] Port - The network port number to use (HTML is always 80)
 * @notes
 *    Requires Arduino Ethernet Library - include <Ethernet.h>
 *    
 * <B>Example:</B>@code{.cpp}
 *    WebServerSTPlus *webserver;
 *    webserver = new WebServerSTPlus(mac,ip,gateway,subnet,false,80); @endcode
 **************************************************************************************/
WebServerSTPlus::WebServerSTPlus(byte mac[], IPAddress ip, IPAddress gateway, IPAddress subnet, bool UseDHCP, int Port) {

  server = new EthernetServer(Port);      // Create EthernetServer object
  digitalWrite(W5100_CSPIN,HIGH);         // disable w5100 SPI while starting SD

  DB1((F("Starting SD..")));
  if(!SD.begin(SD_CSPIN)) DB1L((F("failed"))); else DB1L((F("ok")));

  //---------- Start Ethernet Connection --------------------------------
  if(UseDHCP){ 
    if (Ethernet.begin(mac) == 0) {
      DB1L(("Failed to configure Ethernet using DHCP"));
      UseDHCP = false; } //Default to Static Settings
  }
  if(!UseDHCP) Ethernet.begin(mac, ip, gateway, gateway, subnet);
  
  DB1(("Server IP address: "));         // Print local IP address
  for (byte b = 0; b < 4; b++) {
    DB1((Ethernet.localIP()[b], DEC)); DB1(("."));
  } DB1L(());
  
  delay(2000);
  server->begin();
  
  unsigned long thisTime = millis();
  for(int i=0;i<MAX_SOCK_NUM;i++) { connectTime[i] = thisTime; }
  DB1L((F("Ready")));
}

/***********************************************************************************//**
 * @brief doloop()
 *    Trigger function - needs to be called inside the main sketch loop()
 **************************************************************************************/
void WebServerSTPlus::doloop(char *nc_code) {

  EthernetClient client = server->available();
  if(client) {
    boolean currentLineIsBlank = true;
    boolean currentLineIsGet = true;
    int tCount = 0;
    char tBuf[256];
    int r,t;
    char *pch;
    char ext[5] = "";
    char methodBuffer[8];
    char requestBuffer[238];
    char pageBuffer[48];
    char paramBuffer[238];
    char protocolBuffer[9];
    char fileName[32];
    char fileType[4];
    int clientCount = 0;
    int loopCount = 0;              // this controls the timeout
    
    requestNumber++;
    DB1((F("\r\nClient request #"))); DB1((requestNumber)); DB1((F(": ")));
    

    while (client.connected()) {
      while(client.available()) {
        // if packet, reset loopCount  //        loopCount = 0;

        char c = client.read();
        if(currentLineIsGet && tCount < 255)
        {
          tBuf[tCount] = c;
          tCount++;
          tBuf[tCount] = 0;
        }

        if (c == '\n' && currentLineIsBlank) {
          DB1L((tBuf));
          while(client.available()) client.read();

          int scanCount = sscanf(tBuf,"%7s %237s %8s",methodBuffer,requestBuffer,protocolBuffer);
          if(scanCount != 3) { DB1L((F("bad request"))); sendBadRequest(client); return; }

          // Process ?nc= (numeric control) tags
          pch = strtok(requestBuffer,"?");
          if(pch != NULL) {
            //strncpy(fileName,pch,31);
            //strncpy(tBuf,pch,255);
            strcpy(fileName,pch);
            strcpy(tBuf,pch);
            pch = strtok(NULL,"?");
            if(pch != NULL) {

              // Process URL parameters
              strcpy(paramBuffer,pch);
              
              // Numeric Control (nc=)
              if( strncmp(pch,"nc=",3) == 0 ) {
                strcpy(nc_code,pch+3); 
                DB1((F("param nc_code: ")));DB1L((nc_code));               

              // File Write (fw=)
              } else if ( strncmp(pch,"fw=",3) == 0 ) {
                //strcpy(paramBuffer,pch+3);
                urldecode(paramBuffer,pch+3);
                sdWrite(fileName,paramBuffer,WRITE);

              // File Append (fa=)
              } else if ( strncmp(pch,"fa=",3) == 0 ) {
                //strcpy(paramBuffer,pch+3);
                urldecode(paramBuffer,pch+3);
                sdWrite(fileName,paramBuffer,APPEND);
              
              // Undefined
              } else {
                DB1((F("undefined param: ")));DB1L((paramBuffer));
              }

              client.stop();
              return; 
            } else {
              paramBuffer[0] = 0;
            }
          }
          
          strtoupper(requestBuffer);
          strtoupper(tBuf);

          DB1((F("method = "))); DB1L((methodBuffer));

          if(strcmp(methodBuffer,"GET") != 0 && strcmp(methodBuffer,"HEAD") != 0) {
            sendBadRequest(client);
            return;
          }
          
          for(int x = 0; x < strlen(requestBuffer); x++) {
            if(strchr(legalChars,requestBuffer[x]) == NULL) {
              DB1L((F("bad character")));  
              sendBadRequest(client);
              return;
            }            
          }
          
          DB1((F("params = "))); DB1L((paramBuffer));
          DB1((F("protocol = "))); DB1L((protocolBuffer));
          
          // if dynamic page name (this code looks in-complete
          if(strcmp(requestBuffer,"/MYTEST.PHP") == 0) {
            DB1L((F("dynamic page")));            
          }
          else {
            //----------- PARSE ----------------------
            
            // Home page
            if(strcmp(fileName,"/") == 0) { strcpy(fileName,"/INDEX.HTM"); }

            //vvv[ File Name (Modify for SFN-8.3) ]vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
            DB1((F("SD file = "))); DB1L((fileName));
            pch = strrchr(fileName,'.');                          //ptr to extension
            if (pch!=NULL) {
              int extlen = strlen(pch);             //record original extension length
              memcpy(ext,pch,4);pch[0]='\0';        //ext=4-digit copy, truncate name
              strcpy(fileType,ext+1);               //Copy file type
              strtoupper(fileType);                 //All-caps for matching
              if(extlen>4) strcat(fileName,"~1");   //Truncate files with long ext
            }
            
            pch = strrchr(fileName,'/');                          //ptr to filename
            if (pch!=NULL) {
              if (strlen(pch)>9) {
                pch[7]='~';pch[8]='1';pch[9]='\0';    //Truncate fileName to 8-digits
              }
            }
            strcat(fileName,ext);                                 //Concate extension
            DB1((F("SD Mod file(FSN-8.3) = "))); DB1L((fileName));
            //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
            
            // Check FileName
            if(strlen(fileName) > 30) {
              DB1L((F("filename too long")));
              sendBadRequest(client);
              return;
            }
            else if(strlen(fileType) > 3 || strlen(fileType) < 1) {
              DB1((F("fileType '")));DB1((fileType));
              DB1((F("' is wrong size of")));DB1((strlen(fileType)));
              DB1L((F("file type invalid size")));
              sendBadRequest(client);
              return;
            }
            else {
              
              DB1L((F("filename format ok")));
              if(SD.exists(fileName)) {
                // SRAM check
                DB1((F("SRAM = ")));
                DB1L((freeRam()));
                DB1((F("file found..")));                
                File myFile = SD.open(fileName);
                if(!myFile) {
                  DB1L((F("open error")));
                  sendFileNotFound(client);
                  return;
                }
                else DB1((F("opened..")));
                
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
                  DB1((F("send..")));
                  while(myFile.available()) {
                    clientCount = myFile.read(tBuf,64);
                    client.write((byte*)tBuf,clientCount);
                  }
                }
                myFile.close();              
                DB1L((F("closed")));
                client.stop();                
                DB1L((F("disconnected")));
                return;
                
              }
              else {

                DB1L((F("File not found")));
                sendFileNotFound(client);
                root = SD.open("/");
                printDirectory(root, 3);
                return;
              }

            }
          }

          pch = strtok(paramBuffer,"&");

          while(pch != NULL) {
            if(strncmp(pch,"t=",2) == 0) {
              t = atoi(pch+2);
              DB1(("t=")); DB1L((t,DEC));            
            }
            if(strncmp(pch,"r=",2) == 0) {
              r = atoi(pch+2);
              DB1(("r=")); DB1L((r,DEC));
            }
            pch = strtok(NULL,"& ");
          }

          DB1L((F("Sending response")));


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
      if(loopCount > 1000) {  // if 1000ms has passed since last packet
        client.stop();        // close connection
        DB1L(("\r\nTimeout"));
      }
      
      // delay 1ms for timeout timing
      delay(1);
    }
    DB1L((F("disconnected")));
  }
}

//----------------------- UTILITY ----------------------------------------
/***********************************************************************************//**
 * @brief strtoupper(text)
 *    Change all characters to upper-case
 **************************************************************************************/
void  WebServerSTPlus::strtoupper(char* aBuf) {
  for(int x = 0; x<strlen(aBuf);x++) {
    aBuf[x] = toupper(aBuf[x]);
  }
}

/***********************************************************************************//**
 * @brief urldecode(return, request-text)
 *    Decode %xx URL encodes back into their original characters
 **************************************************************************************/
void WebServerSTPlus::urldecode(char *dst, const char *src) {
  char a, b;
  while (*src) {
    // If %dd found
    if ((*src == '%') && ((a = src[1]) && (b = src[2])) && (isxdigit(a) && isxdigit(b))) {
      if (a >= 'a') { a -= 'a'-'A'; }
      if (a >= 'A') { a -= ('A' - 10);} else { a -= '0';}
      if (b >= 'a') { b -= 'a'-'A';}
      if (b >= 'A') { b -= ('A' - 10);} else { b -= '0';}
      *dst++ = 16*a+b;
      src+=3;
    } else { *dst++ = *src++; }
  }
  *dst++ = '\0';
}

/***********************************************************************************//**
 * @brief freeRam()
 * @return Returns the amount of free RAM memory on the Arduino
 **************************************************************************************/
int WebServerSTPlus::freeRam() {
  extern int __heap_start,*__brkval;
  int v;
  return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int) __brkval);  
}

/***********************************************************************************//**
 * @brief checkSockStatus()
 *    
 **************************************************************************************/
void WebServerSTPlus::checkSockStatus()
{
  unsigned long thisTime = millis();

  for (int i = 0; i < MAX_SOCK_NUM; i++) {
    uint8_t s = W5100.readSnSR(i);

    if((s == 0x17) || (s == 0x1C)) {
        if(thisTime - connectTime[i] > 30000UL) {
          DB1((F("\r\nSocket frozen: ")));
          DB1L((i));
          close(i);
        }
    }
    else connectTime[i] = thisTime;

    socketStat[i] = W5100.readSnSR(i);
  }
}

/***********************************************************************************//**
 * @brief ShowSockStatus()
 *    
 **************************************************************************************/
void WebServerSTPlus::ShowSockStatus()
{
  for (int i = 0; i < MAX_SOCK_NUM; i++) {
    DB1((F("Socket#")));
    DB1((i));
    uint8_t s = W5100.readSnSR(i);
    socketStat[i] = s;
    DB1((F(":0x")));
    DB1((s,16));
    DB1((F(" ")));
    DB1((W5100.readSnPORT(i)));
    DB1((F(" D:")));
    uint8_t dip[4];
    W5100.readSnDIPR(i, dip);
    for (int j=0; j<4; j++) {
      DB1((dip[j],10));
      if (j<3) DB1(("."));
    }
    DB1((F("(")));
    DB1((W5100.readSnDPORT(i)));
    DB1L((F(")")));
  }
}

//----------------------- STATUS MESSAGES --------------------------------
/***********************************************************************************//**
 * @brief sendBadRequest(client)
 *    send a "BAD REQUEST" html page
 **************************************************************************************/
void WebServerSTPlus::sendBadRequest(EthernetClient thisClient) {
  char tBuf[64];
  strcpy_P(tBuf,PSTR("HTTP/1.0 400 Bad Request\r\n"));
  thisClient.write(tBuf);
  strcpy_P(tBuf,PSTR("Content-Type: text/html\r\nConnection: close\r\n\r\n"));
  thisClient.write(tBuf);
  strcpy_P(tBuf,PSTR("<html><body><H1>BAD REQUEST</H1></body></html>"));
  thisClient.write(tBuf);
  thisClient.stop();  

  DB1L((F("disconnected")));

}

/***********************************************************************************//**
 * @brief sendBadRequest(client)
 *    send a "FILE NOT FOUND" html page
 **************************************************************************************/
void WebServerSTPlus::sendFileNotFound(EthernetClient thisClient) {
  char tBuf[64];
  strcpy_P(tBuf,PSTR("HTTP/1.0 404 File Not Found\r\n"));
  thisClient.write(tBuf);
  strcpy_P(tBuf,PSTR("Content-Type: text/html\r\nConnection: close\r\n\r\n"));
  thisClient.write(tBuf);
  strcpy_P(tBuf,PSTR("<html><body><H1>FILE NOT FOUND</H1></body></html>"));
  thisClient.write(tBuf);
  thisClient.stop();  

  DB1L((F("disconnected")));

}


//----------------------------- SD Utility ------------------------------------------------------------
/***********************************************************************************//**
 * @brief printDirectory(dir,numTabs)
 *    List the files in the 'dir' path of the SD-card on the Serial Monitor
 **************************************************************************************/
void WebServerSTPlus::printDirectory(File dir, int numTabs) {
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

/***********************************************************************************//**
 * @brief sdWrite(fileName,text,type)
 * @param fileName The full path and fileName to write to on the SD-Card   
 * @param text The text to be written
 * @param WriteType 'WRITE' or 'APPEND'
 * @notes
 *    'fileName' will be automatically parsed to the 8.3-SFN standard
 **************************************************************************************/
void WebServerSTPlus::sdWrite(char* fileName, char* text, sdWriteType WriteType) {
  DB1((F("WebServerSTPlus::sdWrite( '")));DB1((fileName));DB1((F("','")));DB1((text));DB1L((F("')")));

  char *pch;
  char ext[5] = "";
  char fileType[4];
  
  //vvv[ File Name (Modify for SFN-8.3) ]vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
  DB1((F("SD file = "))); DB1L((fileName));
  pch = strrchr(fileName,'.');                          //ptr to extension
  if (pch!=NULL) {
    int extlen = strlen(pch);             //record original extension length
    memcpy(ext,pch,4);pch[0]='\0';        //ext=4-digit copy, truncate name
    strcpy(fileType,ext+1);               //Copy file type
    strtoupper(fileType);                 //All-caps for matching
    if(extlen>4) strcat(fileName,"~1");   //Truncate files with long ext
  }
            
  pch = strrchr(fileName,'/');                          //ptr to filename
  if (pch!=NULL) {
    if (strlen(pch)>9) {
      pch[7]='~';pch[8]='1';pch[9]='\0';    //Truncate fileName to 8-digits
    }
  }
  strcat(fileName,ext);                                 //Concate extension
  DB1((F("SD Mod file(FSN-8.3) = "))); DB1L((fileName));
  //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
            
  if( WriteType==WRITE ) {
    DB1((F("WRITE ")));
    File myFile = SD.open(fileName,O_CREAT | O_TRUNC | O_WRITE);
    myFile.write(text,strlen(text));
    myFile.close();
    DB1L((F("complete...")));
    
  } else if( WriteType==APPEND ) {
    DB1((F("APPEND ")));
    if(!SD.exists(fileName)) { DB1L((F("file not found..."))); return; }
    DB1((F("file found..")));                
    File myFile = SD.open(fileName,O_WRITE | O_APPEND);
    myFile.write(text,strlen(text));
    myFile.close();
    DB1L((F("complete...")));
  }
}

