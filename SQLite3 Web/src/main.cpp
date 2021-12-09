/*
    For more information, visit https://github.com/siara-cc/esp32_arduino_sqlite3_lib

    Copyright (c) 2018, Siara Logics (cc)
*/

/*
   Copyright (c) 2015, Majenko Technologies
   All rights reserved.

   Redistribution and use in source and binary forms, with or without modification,
   are permitted provided that the following conditions are met:

 * * Redistributions of source code must retain the above copyright notice, this
     list of conditions and the following disclaimer.

 * * Redistributions in binary form must reproduce the above copyright notice, this
     list of conditions and the following disclaimer in the documentation and/or
     other materials provided with the distribution.

 * * Neither the name of Majenko Technologies nor the names of its
     contributors may be used to endorse or promote products derived from
     this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
   ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
   ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <SPI.h>
#include <FS.h>
#include "SD.h"

#include <WiFi.h>
#include <WebServer.h>
#include "credits.h"
#include "web_page.h"
WebServer server(80);

sqlite3 *company_db;
const char* data = "Callback function called";
char *zErrMsg = 0;
int openDb(const char *, sqlite3 **);
int db_exec(sqlite3 *, const char *);
static int callback(void *data, int argc, char **argv, char **azColName);

void handleRoot();
void handleNotFound();
void insertRecord();
void deleteRecord();
void showRecords();
void newRecordTable();
String web_content = "";


void setup() {
  Serial.begin(115200);
  Serial.print("\n Connecting to Wifi");
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }

  Serial.print("\n Connected. IP adress: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);
  server.on("/insert",insertRecord);
  server.on("/delete",deleteRecord);
  server.on("/show",showRecords);
  server.on("/newRecordTable",newRecordTable);

  server.begin();
  
 
  int rc;

  SPI.begin();
  SD.begin();

   sqlite3_initialize();

  if (openDb("/sd/company.db", &company_db))
      return;
}



void loop() {
  server.handleClient();
}





/*--------------------------------------------------------*/
int openDb(const char *filename, sqlite3 **db) {
   int rc = sqlite3_open(filename, db);
   if (rc) {
       Serial.printf("Can't open database: %s\n", sqlite3_errmsg(*db));
       return rc;
   } else {
       Serial.printf("Opened database successfully\n");
   }
   return rc;
}
/*--------------------------------------------------------*/

int db_exec(sqlite3 *db, const char *sql) {
   Serial.println('\n');
   int rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
   if (rc != SQLITE_OK) {
       Serial.printf("SQL error: %s\n", zErrMsg);
       sqlite3_free(zErrMsg);
   }
   return rc;
}
/*--------------------------------------------------------*/
/* static int callback(void *data, int argc, char **argv, char **azColName){
   int i;
   for (i = 0; i<argc; i++){
       Serial.printf("%s = %s  ", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   Serial.printf("\n");
   return 0;
} */

static int callback(void *data, int argc, char **argv, char **azColName){
  web_content += "<tr>";
  for (int i = 1; i<argc; i++){
    web_content +="<td>";
    web_content += argv[i];
    web_content +="</td>";
  }
  web_content +="<td><button onClick =\"del('";
  web_content += argv[1];
  web_content += "')\">Delete</button></td></tr>"; 
  return 0;
}

/*--------------------------------------------------------*/
void handleRoot() {

  server.send(200, "text/html", web_page);

}
/*--------------------------------------------------------*/

void handleNotFound() {

  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
      message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }

  server.send ( 404, "text/plain", message );

}

/*--------------------------------------------------------*/
void insertRecord(){
  web_content ="";
  String sql = "";
  String personal_id = server.arg("personal_id");
  String name = server.arg("name");
  String surname = server.arg("surname");
  String start_date = server.arg("start_date");
  String salary = server.arg("salary");
  sql="insert into employee(personal_id,name,surname,start_date,salary) values("+personal_id+",'"+name+"','"+surname+"','"+start_date+"',"+salary+")";
  Serial.println(sql);
  if (db_exec(company_db, sql.c_str()) == SQLITE_OK) {
      web_content += "OK";   
      Serial.println(web_content);    
    }
  else 
    web_content += "FAIL";  
  server.send (200, "text/html", web_content ); 

}
/*--------------------------------------------------------*/
void deleteRecord(){
  web_content ="";
  String personal_id = server.arg("personal_id");
  String sql = "delete from employee where personal_id="+personal_id;
  if (db_exec(company_db, sql.c_str()) == SQLITE_OK) {
      web_content += "OK";   
      Serial.println(web_content);    
    }
  else 
    web_content += "FAIL";  
  server.send (200, "text/html", web_content ); 
}
/*--------------------------------------------------------*/
void showRecords(){
  web_content ="<table style='width:100%'><tr><th>ID</th><th>NAME</th><th>SURNAME</th><th>START DATE</th><th>SALARY</th><th>DEL</th></tr>";
  String sql = "Select * from employee";
    if (db_exec(company_db, sql.c_str()) == SQLITE_OK) {  
      web_content += "</table>";   
    }
  else 
    web_content = "FAIL";  
  server.send (200, "text/html", web_content );  

}
/*--------------------------------------------------------*/
void newRecordTable(){
   const char c_web_content[] PROGMEM =R"rawliteral(<table style='width:100%'>
   <tr><th>ID</th><th>NAME</th><th>SURNAME</th><th>START DATE</th><th>SALARY</th><th>INSERT</th><th>CANCEL</th></tr>
   <tr>
   <td> <input type='text' id='pid'></td>
   <td><input type='text' id='pname'></td>
   <td><input type='text' id='psname'></td>
   <td><input type='text' id='sdate'></td>
   <td><input type='text' id='salary'></td>
   <td><button onClick='insert()'>Insert</button></td>
   <td><button onClick='cancel()'>Cancel</button></td>
   </tr>
   </table>
   )rawliteral";
   server.send (200, "text/html", c_web_content );  
}
