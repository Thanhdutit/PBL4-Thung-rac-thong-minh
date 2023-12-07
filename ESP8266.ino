#include <Arduino.h>
#include <ESP8266WiFi.h>
#include<Servo.h>
#include <ESP8266WebServer.h>
#include <WebSocketsClient.h> 
ESP8266WebServer webServer(80);
Servo vanthanh;
unsigned long oldtime = 0;

WebSocketsClient webSocket;
const char* ssid = "Uzumaki"; //Đổi thành wifi của bạn
const char* password = "12351235"; //Đổi pass luôn
const char* ip_host = "192.168.208.140"; //Đổi luôn IP host của PC nha
const uint16_t port = 8765; //Port thích đổi thì phải đổi ở server nữa
const int LED = 2;
const int BTN = 0;

IPAddress staticIP(192,168,208,99);
IPAddress gateway(192,168,208,1);
IPAddress subnet(255,255,255,0);
IPAddress dns(192,168,208,1);

//=========Biến chứa mã HTLM Website==//
const char MainPage[] PROGMEM = R"=====(
  <!DOCTYPE html> 
  <html>
   <head> 
       <title>TRẠNG THÁI THÙNG RÁC</title> 
       <style> 
          body {text-align:center;}
          h1 {color:#003399;}
          a {text-decoration: none;color:#FFFFFF;}
          .bt_on {height:50px; width:100px; margin:10px 0;background-color:#FF6600;border-radius:5px;}
          .bt_off {height:50px; width:100px; margin:10px 0;background-color:#00FF00;border-radius:5px;}
       </style>
       <meta charset="UTF-8">
   </head>
   <body> 
      <h1>TRẠNG THÁI THÙNG RÁC</h1> 
      <div style="color:#008080;font-size:40px">Trạng thái thùng rác TR001: <b><pan id="trangthaiD1"><pan></b></div>
      <div style="color:#008080;font-size:40px">Tình trạng: <b><pan id="trangthaiD2"><pan></b></div>
      

      <script>
        //-----------Tạo đối tượng request----------------
        function create_obj(){
          td = navigator.appName;
          if(td == "Microsoft Internet Explorer"){
            obj = new ActiveXObject("Microsoft.XMLHTTP");
          }else{
            obj = new XMLHttpRequest();
          }
          return obj;
        }
        var xhttp = create_obj();
      
        //------------Kiểm tra trạng thái chân D1 D2------
        function getstatusD1D2(){
          xhttp.open("GET","getstatusD1D2",true);
          xhttp.onreadystatechange = process_json;
          xhttp.send();
        }
        //-----------Kiểm tra response-------------------------------
        function process_json(){
          if(xhttp.readyState == 4 && xhttp.status == 200){
            //------Update data sử dụng javascript-------------------
            var trangthaiD1D2_JSON = xhttp.responseText;
            var Obj = JSON.parse(trangthaiD1D2_JSON);
            document.getElementById("trangthaiD1").innerHTML = Obj.D1;
            document.getElementById("trangthaiD2").innerHTML = Obj.D2;
          }
        }
        //---Ham update duu lieu tu dong---
        setInterval(function() {
          getstatusD1D2();
        }, 5000);
      </script>
   </body> 
  </html>
)=====";
//=========================================//

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  
  String cmd = "";
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[WSc] Disconnected!\n");
      break;
    case WStype_CONNECTED:
      {
        Serial.printf("[WSc] Connected to url: %s\n", payload);
      }
      break;
    case WStype_TEXT:
      Serial.printf("[WSc] get text: %s\n", payload);
      cmd = "";
      for (int i = 0; i < length; i++) {
        cmd = cmd + (char) payload[i];
      }
        if(cmd =="1")
        {
        
          vanthanh.write(180);
          delay(3000);
          vanthanh.write(0);
          
        }
        if(cmd =="2")
        {
          vanthanh.write(180);
          delay(3000);
          vanthanh.write(0);  
        }
           if(cmd =="3")
        {
          vanthanh.write(180);
          delay(3000);
          vanthanh.write(0);  
        }
        
      break;
    case WStype_BIN:
      Serial.printf("[WSc] get binary length: %u\n", length);
      break;
  }
}
void setup() {
   unsigned long duration;
  int distance;
  pinMode(D6, OUTPUT);
  pinMode(D5, INPUT);
  vanthanh.attach(D4);
  pinMode(LED, OUTPUT);
  pinMode(BTN, INPUT);
  Serial.begin(115200);
  WiFi.config(staticIP,gateway,subnet,dns,dns);
  Serial.println("ESP8266 Websocket Client");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(WiFi.localIP());
  webSocket.begin(ip_host, port);
  webSocket.onEvent(webSocketEvent);
  webServer.on("/",mainpage);
  webServer.on("/getstatusD1D2",get_statusD1D2);
  webServer.begin();
}
void loop() {
 webSocket.loop();

  webServer.handleClient();
  
  

  
 
}
int sieu_am()
{
  unsigned long duration;
  int distance;
  digitalWrite(D6, LOW);  
  delayMicroseconds(2); 
  
  digitalWrite(D6, HIGH);
  delayMicroseconds(10); 
  
  digitalWrite(D6, LOW);
  duration = pulseIn(D5, HIGH);
  distance = duration * 0.034 / 2;
 return distance; 
//   if (distance>75.0)
//  return 1;
//  else if (distance<=75.0 && distance>50.0)
//  return 2;
//  else if (distance<=50.0 && distance>25.0)
//  return 3;
//  else if (distance<=25.0 && distance>10.0)
//  return 4;
//  else
//  return 5;
 
 }
void mainpage(){
  String s = FPSTR(MainPage);
  webServer.send(200,"text/html",s);
}

void get_statusD1D2(){
String d1,d2;
int kt =sieu_am();
    if(kt>75)
    {
    d1 ="0%";
    d2="Chưa đầy";
    }
    
      else if(kt<=75&&kt>50)
    {
    d1 ="25%";
    d2="Chưa đầy";
    }
    
    else if(kt<=50&&kt>25)
    {
    d1 ="50%";
    d2="Chưa đầy";
    }
    
    else if(kt<=25 && kt>15)
    {
    d1 ="75%";
    d2="Chưa đầy";
    }
    else if(kt<=15 && kt>8)
    {
    d1 ="85%";
    d2="Chưa đầy";
    }
    else 
    {
    d1 ="100%";
    d2="đã đầy";
    }
 Serial.println(kt);
 Serial.println(d1);
 Serial.println(d2);
  String s = "{\"D1\": \""+ d1 +"\",\"D2\": \""+ d2 +"\"}";
  webServer.send(200,"application/json",s);
}
