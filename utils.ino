
void testHandleData() {
  char* data = "HTTP/1.1 200 OK\nX-Frame-Options: sameorigin\nX-XSS-Protection: 1; mode=block\nContent-Type: text/html;charset=utf-8\nContent-Length: 35\nConnection: keep-alive\nServer: thin 1.3.1 codename Triple Espresso\n\n%%%1023,0,0|0,1023,0|0,0,1023&&&";
  handleData(data, 236);
}

// Function that prints data from the server
void handleData(char* content, int len) {
  // Note that the content is not null-terminated, may be broken up into smaller packets, and 
  // includes the HTTP header.
  
  int perc_count = 0;
  char curr_char;
  boolean reading_data = false;
  
  int data_length = 0;
  char* data_start;
  
  //Serial.println("New request!");
  //Serial.println(len);
  while (len-- > 0) {
    got_data = true;
    curr_char = *(++content);
    if (reading_data) {
      if (curr_char == '&') {
        extractAndProcessRgbStrings(data_start, data_length);
        break;
      }
      if (data_length++ == 0) data_start = content;
    } else {
      if (curr_char == '%') {
        perc_count++;
      } else {
        if (perc_count > 0) Serial.println("Resetting perc_count!");
        perc_count = 0;
      }
      if (perc_count == 3) {
        reading_data = true;
      }
    }
  }
 //Serial.println();
}

void extractAndProcessRgbStrings(char* data, int len) {
  char* rgbString = data;
  int rgbStringLength = 0;
  char currChar = *data;
  
  while (len--) {
    currChar = *(data++);
    if (currChar == '|') {
      processRgbString(rgbString, rgbStringLength);
      rgbStringLength = 0;
      rgbString = data;
    } else rgbStringLength++;
  }
  processRgbString(rgbString, rgbStringLength);
}

void processRgbString(char* rgbString, int rgbStringLength) {
  char* lumosityString = rgbString;
  int lumosityStringLength = 0;
  char currChar = *rgbString;
  int rgb[3];
  int rgbIndex = 0;
  
  Serial.println("processRgbString");
  while (rgbStringLength--) {
    currChar = *(rgbString++);
    Serial.print(currChar);
    if (currChar == ',') {
      rgb[rgbIndex++] = reinventTheWheel(lumosityString, lumosityStringLength);
      lumosityStringLength = 0;
      lumosityString = rgbString;
    } else lumosityStringLength++;
  }
  //Serial.println();
  //Serial.println("reinventing");
  rgb[rgbIndex++] = reinventTheWheel(lumosityString, lumosityStringLength);
  //Serial.println("pushing");
  pushRgb(rgb);
}

void pushRgb(int* rgb) {
  int i = -1;
  if (!isBufferFull()) {
    while(++i < 3) rgbBuffer[rgbBufferNextPush][i] = *(rgb++);
    if (++rgbBufferNextPush == RGB_BUFFER_SIZE) {
      rgbBufferNextPush = 0;
    }
    Serial.println(rgbBufferNextPush);
  } else {
    Serial.println("Full!");
    //outputBuffer();
  }
}

int* popRgb() {
  int* rgb;
  if (!isBufferEmpty()) {
    rgb = rgbBuffer[rgbBufferNextPop];
    if (++rgbBufferNextPop == RGB_BUFFER_SIZE) {
      rgbBufferNextPop = 0;
    }
    return rgb;
  } else {
    //Serial.println("Empty!");
    return 0;
  }
}

boolean isBufferEmpty() {
  return rgbBufferNextPop == rgbBufferNextPush;
}

boolean isBufferFull() {
  if (rgbBufferNextPop == 0) {
    return rgbBufferNextPush+1 == RGB_BUFFER_SIZE;
  } else {
    return rgbBufferNextPush+1 == rgbBufferNextPop;
  }
}

void outputBuffer() {
  int i=-1;
  while(++i<RGB_BUFFER_SIZE) {
    Serial.println(rgbBuffer[i][0]);
    Serial.println(rgbBuffer[i][1]);
    Serial.println(rgbBuffer[i][2]);
    Serial.println("|");
  }
}

void outputRgb(int* rgb) {
  int i=-1;
  //Serial.println("RGB DEBUG");
  while(++i < 3) Serial.println(rgb[i]);
}

//A less fun and more likely to collide name would be stringToInt
int reinventTheWheel(char* numberString, int numberLength) {
  int multiplier = 1;
  int i;
  int number = 0;
  
  while (numberLength--) {
    multiplier = 1;
    i = numberLength;
    while (i--) multiplier*= 10;
    switch(*(numberString++)){
      case '9':
        number+=multiplier;
      case '8':
        number+=multiplier;
      case '7':
        number+=multiplier;
      case '6':
        number+=multiplier;
      case '5':
        number+=multiplier;
      case '4':
        number+=multiplier;
      case '3':
        number+=multiplier;
      case '2':
        number+=multiplier;
      case '1':
        number+=multiplier;
    
    }
  }
  return number;
}
