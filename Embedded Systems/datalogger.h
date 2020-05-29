#include <SD.h>
#define setupFile "setup.txt"
#define toWriteFile "datalog.txt"
class Datalogger{
  public:
  Datalogger();
  int writeSD(String text);
  String readSD();
  private:
  File file;
  
 

  
};
