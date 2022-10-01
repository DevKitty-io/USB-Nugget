#include "utils.h"

fileOp saveFileBase64(String path, String content) {
  fileOp ret;
  ret.ok = false;

  // construct directories as needed
  FILINFO filinfo;
  int pathSoFarIdx = 1;
  while(true) {
    int nextDir = path.indexOf("/", pathSoFarIdx);
    if (nextDir == -1){
      break;
    }
    String pathSoFar = path.substring(0, nextDir);
    if (FR_OK != f_stat(pathSoFar.c_str(), &filinfo)){
      if (f_mkdir(pathSoFar.c_str()) != FR_OK) {
        //server.send(500, "text/plain", "Could not create directory");
        ret.err_msg = String("could not create directory");
        return ret;
      }
    }
    pathSoFarIdx = nextDir+1;
  }

  // Create file
  FIL file;
  if (FR_OK != f_open(&file, path.c_str(), FA_WRITE | FA_CREATE_ALWAYS)){
    //server.send(500, "text/plain", "Could not open file for writing");
    ret.err_msg = String("could not open file for writing");
    return ret;
  }

  // Write to file
  content.replace(" ", "/"); // why
  const char* contentBase64 = content.c_str();
  size_t payloadLength = BASE64::decodeLength(contentBase64);
  uint8_t payloadContent[payloadLength];
  BASE64::decode(contentBase64, payloadContent);
  UINT written = 0;
  if (FR_OK != f_write(&file, payloadContent, payloadLength, &written)){
    //server.send(500, "text/plain", "Could not write to file");
    f_close(&file);
    ret.err_msg = String("Could not write to file");
    return ret;
  }
  f_close(&file);
  ret.ok = true;
  return ret;
}
