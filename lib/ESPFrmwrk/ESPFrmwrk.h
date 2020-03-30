/*
  ESPFrmwrk.h - Library for serving framework files
*/
#ifndef ESPFrmwrk_h
#define ESPFrmwrk_h

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

#define numberOfFiles 18

class ESPFrmwrk
{
public:
  // GET String, file location, file type
  String filesToServe[numberOfFiles][3] = {
      {"/", "/index.html", "text/html"},
      {"/index.html", "", "text/html"},
      {"/settings.html", "", "text/html"},
      {"/css/main.css", "", "text/css"},
      {"/css/util.css", "", "text/css"},
      {"/images/favicon.ico", "", "image/x-icon"},
      {"/images/SousVide.png", "", "image/png"},
      {"/js/main.js", "", "application/javascript"},
      {"/fw/bootstrap.min.css", "", "text/css"},
      {"/fw/bootstrap-theme.min.css", "", "text/css"},
      {"/fw/bootstrap.min.js", "", "application/javascript"},
      {"/fw/jquery-3.2.1.min.js", "", "application/javascript"},
      {"/fonts/glyphicons-halflings-regular.eot", "/fonts/glyphicons.eot", "application/vnd.ms-fontobject"},
      {"/fonts/glyphicons-halflings-regular.svg", "/fonts/glyphicons.svg", "image/svg+xml"},
      {"/fonts/glyphicons-halflings-regular.ttf", "/fonts/glyphicons.ttf", "font/ttf"},
      {"/fonts/glyphicons-halflings-regular.woff", "/fonts/glyphicons.woff", "font/woff"},
      {"/fonts/glyphicons-halflings-regular.woff2", "/fonts/glyphicons.woff2", "font/woff2"}};

  void serveFile(AsyncWebServer &, int);
  void setupFrmwrk(AsyncWebServer &);

  // private:
  //     int _pin;
};

#endif