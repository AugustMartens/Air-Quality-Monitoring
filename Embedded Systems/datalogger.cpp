#include "datalogger.h"


Datalogger::Datalogger(){
 
}

 int Datalogger::writeSD(String dataString){
  file = SD.open(toWriteFile, FILE_WRITE);

        // if the file is available, write to it:
        if (file) {
            file.println(dataString);
            file.close();
        }
        else{
          return 0;
        }
  
  
  return 1;
  }

  String Datalogger::readSD(){
    
    file = SD.open(setupFile);
char message[200];
int i = 0;
    if(file){
      while(file.available()){
        message[i] =(char)file.read();
        //Serial.write(file.read());
        i++;
      }
      file.close();
    }
    else{
      return "fileUnreachable";
    }
    return String(message);
  }
