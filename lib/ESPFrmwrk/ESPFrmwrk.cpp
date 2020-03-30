/*
  ESPFrmwrk.cpp - Library for serving framework files
*/

#include <Arduino.h>
#include <ESPFrmwrk.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>

void ESPFrmwrk::serveFile(AsyncWebServer &server, int file)
{
  server.on(filesToServe[file][0].c_str(), HTTP_GET, [file](AsyncWebServerRequest *request) {
    ESPFrmwrk espf;
    const String locX = espf.filesToServe[file][0];
    const String loc = espf.filesToServe[file][1];
    const String type = espf.filesToServe[file][2];
    if (loc.length() <= 1)
      request->send(LittleFS, locX, type);
    else
      request->send(LittleFS, loc, type);
  });
}

void ESPFrmwrk::setupFrmwrk(AsyncWebServer &server)
{
  for (int i = 0; i < numberOfFiles; ++i)
    serveFile(server, i);
}