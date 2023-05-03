#include <STM32SD.h>
#include <stdio.h>
#include "flash.h"

Sd2Card card;
SdFatFs fatFs;

#define SDBuffLen 512   //SD sector size

///////////////////////////////////////////////////////////////////////////////
void DebugPrint(char Name[], uint32_t Value, uint8_t Type){
  char debug_str[80];
  if(Type==0){ //byte
    sprintf (debug_str, "%s %02X", Name, Value);
  }else
  if(Type==1){ //words
    sprintf (debug_str, "%s %04X", Name, Value);
  }else
  if(Type==2){ //int
    sprintf (debug_str, "%s %08X", Name, Value);
  }   
  if(Type==3){ //string
    sprintf (debug_str, "%s", Name);
  }   
  Serial.println(debug_str);
}

///////////////////////////////////////////////////////////////////////////////   
char GetSerialChar(){
  char Ch;
  while (Serial.available() == 0); //Wait for char
  Ch = Serial.read(); //read char

  //read the rest
  while (Serial.available() != 0) Serial.read();
  return Ch;
}

void GetSerialString(char Str[], uint8_t MaxLen){
    char Ch=0xff;
    uint8_t Index=0;

    while (1){ 
      if(Index >= (MaxLen-1)) break;  
      
      while (Serial.available() == 0); //Wait for char   
      Ch = Serial.read();   // read char
      if(Ch=='\n' || Ch=='\r') break;

      if(Ch==0x7f){
        if(Index > 0){
          Index--; //backspace
          Serial.print(Ch);
        }
      }else{
        Serial.print(Ch);
        Str[Index] = Ch;
        Index++;
      }
    }
    Serial.println();
    Str[Index]=0;
}




  /*
  // Setup Flash Pins 
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  //OUTPUTS
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; // режим - выход
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; // высокая скорость
  GPIO_InitStruct.Pull = GPIO_NOPULL;

  GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14;
  HAL_GPIO_Init (GPIOD, &GPIO_InitStruct);

  // All control Pin - Up
  HAL_GPIO_WritePin (GPIOD, BYTE_pin, Up); // Word mode
  HAL_GPIO_WritePin (GPIOD, WP_pin, Up);   // Write protect Off
  HAL_GPIO_WritePin (GPIOD, CE_pin, Up);   // Chip Enable Off
  HAL_GPIO_WritePin (GPIOD, OE_pin, Up);   // Output Enable off
  HAL_GPIO_WritePin (GPIOD, WE_pin, Up);   // Write Enable off
  HAL_GPIO_WritePin (GPIOD, RESET_pin, Up); // NO reset

  // A0-A15  - PE0-PE15 
  GPIO_InitStruct.Pin = GPIO_PIN_All; // вывод
  HAL_GPIO_Init (GPIOE, &GPIO_InitStruct);

  // A16-A24  - PA0-PA8
  GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8;
  HAL_GPIO_Init (GPIOA, &GPIO_InitStruct);

  //INPUTS
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT; // режим - вход
  
  // D0-D7  - PC0-PC7
  GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
  HAL_GPIO_Init (GPIOC, &GPIO_InitStruct);

  // D8-D15  - PB8-PB15
  GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
  HAL_GPIO_Init (GPIOB, &GPIO_InitStruct);
  
  //RY/BY# - PD11
  GPIO_InitStruct.Pin = GPIO_PIN_11;
  HAL_GPIO_Init (GPIOD, &GPIO_InitStruct);/**/
  

///////////////////////////////////////////////////////////////////////////////
void TestProgramFlash(){    //test program flash in normal mode

    Serial.println("\nTest Write flash!"); 

    Serial.println("Pin setup."); 
    FlashInitSetup();
    WriteDataLinesSetup();
    
    Serial.println("Hardware reset flash."); 
    ResetChip();
    
    char DebugStr[40];
    Serial.println("Start write."); 
    for(uint32_t Counter = 0; Counter <= 0x1FF; Counter++){
      sprintf(DebugStr,"Write 0x%08X  0x%04X", Counter, Counter);
      Serial.println(DebugStr);
      ProgramWord(Counter, Counter);
    }
    Serial.println("Test write done.\n");
    FlashSetupControlPinDefault();
}


///////////////////////////////////////////////////////////////////////////////
void TestAllProgramFlash(){    //test program all flash in normal mode

    Serial.println("\nTest Write flash!"); 

    Serial.println("Pin setup."); 
    FlashInitSetup();
    WriteDataLinesSetup();
    
    Serial.println("Hardware reset flash."); 
    ResetChip();
    
    char DebugStr[40];
    Serial.println("Start write."); 
    for(uint32_t Counter = 0; Counter <= 0x1FFFFFF; Counter++){
      if( (Counter & 0x0FFFF) == 0){
        sprintf(DebugStr,"Write block: 0x%08X",Counter);
        Serial.println(DebugStr);
      }
      ProgramWord(Counter, Counter);
    }
    Serial.println("Test write done.\n");
    FlashSetupControlPinDefault();
}

///////////////////////////////////////////////////////////////////////////////
void TestProgramFlashBuffer(){    //test program flash in normal mode
  
    Serial.println("\nTest Write flash using Buffer!");

    Serial.println("Pin setup."); 
    FlashInitSetup();
    WriteDataLinesSetup();
    
    Serial.println("Hardware reset flash."); 
    ResetChip();

    Serial.println("Start write."); 

    uint16_t Buffer[BufferLen];
    uint8_t BufferCounter = 0;
    uint32_t StartBufferAddress;
    
    for(uint32_t Counter = 0; Counter <= 0x1FF; Counter++){

      if(BufferCounter==0){
          StartBufferAddress = Counter;
      }

      Buffer[BufferCounter] = Counter;
      BufferCounter++;

      if(BufferCounter == BufferLen){
        BufferCounter=0;
        char DebugStr[60];
        sprintf(DebugStr,"Program Start Address: 0x%08X, Lenght: 0x%02X", StartBufferAddress, BufferLen);
        Serial.println(DebugStr);
        
        ProgramBuffer(StartBufferAddress, BufferLen, Buffer);
      }
    }
    
    Serial.println("Test write buffer done.\n");
    FlashSetupControlPinDefault();
}



///////////////////////////////////////////////////////////////////////////////
void TestReadFlash(){
    Serial.println("\nPin setup."); 
    FlashInitSetup();
    
    Serial.println("Hardware reset flash."); 
    ResetChip();
    
    Serial.println("Data lines setup.");   
    ReadDataLinesSetup();

    char DebugStr[40];
    
    Serial.println("Start read Flash."); 
    for(uint32_t Counter = 0; Counter <= 0x1FF; Counter++){
      if( (Counter & 0xF) == 0){
        sprintf(DebugStr,"\n\r%08X  ", Counter);
        Serial.print(DebugStr);    
      }
      uint16_t Data = ReadCycle(Counter);
      sprintf(DebugStr,"%04X ", Data);
      Serial.print(DebugStr);
    }
    Serial.println();
    for(uint32_t Counter = 0x1FFFE00; Counter <= 0x1FFFFFF; Counter++){
      if( (Counter & 0xF) == 0){
        sprintf(DebugStr,"\n\r%08X  ", Counter);
        Serial.print(DebugStr);    
      }
      uint16_t Data = ReadCycle(Counter);
      sprintf(DebugStr,"%04X ",Data);
      Serial.print(DebugStr);
    }

    FlashSetupControlPinDefault();

    Serial.println("\n\rRead complete...\n\r");
}



///////////////////////////////////////////////////////////////////////////////
void TestAllProgramFlashBuffer(){    //test program flash in normal mode
  
    Serial.println("\nTest write all flash using Buffer!");

    Serial.println("Pin setup."); 
    FlashInitSetup();
    WriteDataLinesSetup();
    
    Serial.println("Hardware reset flash."); 
    ResetChip();

    Serial.println("Start write."); 

    uint16_t Buffer[BufferLen];
    uint8_t BufferCounter = 0;
    uint32_t StartBufferAddress;

    char DebugStr[50];
    for(uint32_t Counter = 0x0; Counter <= 0x1FFFFFF; Counter++){

      if( (Counter & 0x0FFFF) == 0){
        sprintf(DebugStr,"Write block: 0x%08X",Counter);
        Serial.println(DebugStr);
      }

      if(BufferCounter==0){
          StartBufferAddress = Counter;
      }

      Buffer[BufferCounter] = uint16_t(Counter);
      BufferCounter++;

      if(BufferCounter == BufferLen){
        BufferCounter=0;
        ProgramBuffer(StartBufferAddress, BufferLen, Buffer);
      }
    }
    
    Serial.println("Test write buffer done.\n");
    FlashSetupControlPinDefault();
}

///////////////////////////////////////////////////////////////////////////////
void WrireSDToFlashBuffer(uint32_t StartAddress, char FileName[]){
    uint16_t WriteBuffer[BufferLen];
    uint32_t Counter;
    uint8_t ReadCounter = 0;  
    char DebugStr[200];
    File myFile;

    Serial.println("\nWrite flash from SD.");  
    Serial.print("Open file : ");
    Serial.println(FileName);   
    
    myFile = SD.open(FileName);
    if (!myFile) {
        Serial.print("Error opening file: ");
        Serial.println(FileName);        
        return;
    }

    Serial.println("Pin setup."); 
    FlashInitSetup();
    WriteDataLinesSetup();
    
    Serial.println("Hardware reset flash."); 
    ResetChip();
    
    Serial.println("Start write."); 
    Counter = StartAddress;
    
    while(myFile.available()!=0){
      if(Counter >= 0x1ffffff ){
        Serial.println("Flash is full!");
        return;
      }
      //read bufer from file
      ReadCounter = myFile.read(WriteBuffer,sizeof(WriteBuffer)) / 2;

      //write bufer to flash
      int8_t Ret = ProgramBuffer(Counter, ReadCounter, WriteBuffer);
      if(Ret !=0){
        sprintf(DebugStr,"Error write. Address: 0x%08X Buffer len: %d Error code: %d", Counter, ReadCounter, Ret);
        Serial.println(DebugStr);        
      }
      
      if( (Counter & 0x0FFFF) == 0){
        sprintf(DebugStr,"Write block: 0x%08X",Counter);
        Serial.println(DebugStr);
      }
      
      if(ReadCounter != (sizeof(WriteBuffer) / 2)){
        Serial.println("End of file!");
        break;
      }
      Counter += ReadCounter;
      ReadCounter = 0;
    }
    
    FlashSetupControlPinDefault();
    ReadDataLinesSetup();
    
    myFile.close();                        // close file
    Serial.println("Write flash Completed");

    sprintf(DebugStr,"Write start: 0x%08X, write end: 0x%08X\n\n",StartAddress, Counter-1);    
    Serial.print(DebugStr);
}



///////////////////////////////////////////////////////////////////////////////
void WrireSDToFlash(uint32_t StartAddress, char FileName[]){
    uint16_t WriteBuffer[BufferLen];
    uint32_t Counter;
    uint8_t ReadCounter = 0;  
    char DebugStr[200];
    File myFile;

    Serial.println("\nWrite flash from SD.");  
    Serial.print("Open file : ");
    Serial.println(FileName);   
    
    myFile = SD.open(FileName);
    if (!myFile) {
        Serial.print("Error opening file: ");
        Serial.println(FileName);        
        return;
    }

    Serial.println("Pin setup."); 
    FlashInitSetup();
    WriteDataLinesSetup();
    
    Serial.println("Hardware reset flash."); 
    ResetChip();
    
    Serial.println("Start write."); 
    Counter = StartAddress;
    
    while(myFile.available()!=0){
      if(Counter >= 0x1ffffff ){
        Serial.println("Flash is full!");
        return;
      }
      //read bufer from file
      ReadCounter = myFile.read(WriteBuffer,sizeof(WriteBuffer)) / 2;

      //write bufer to flash
      int8_t Ret = ProgramBuffer(Counter, ReadCounter, WriteBuffer);
      if(Ret !=0){
        sprintf(DebugStr,"Error write. Address: 0x%08X Buffer len: %d Error code: %d", Counter, ReadCounter, Ret);
        Serial.println(DebugStr);        
      }
      
      if( (Counter & 0x0FFFF) == 0){
        sprintf(DebugStr,"Write block: 0x%08X",Counter);
        Serial.println(DebugStr);
      }
      
      if(ReadCounter != (sizeof(WriteBuffer) / 2)){
        Serial.println("End of file!");
        break;
      }
      Counter += ReadCounter;
      ReadCounter = 0;
    }
    
    FlashSetupControlPinDefault();
    ReadDataLinesSetup();
    
    myFile.close();                        // close file
    Serial.println("Write flash Completed");

    sprintf(DebugStr,"Write start: 0x%08X, write end: 0x%08X\n\n",StartAddress, Counter-1);    
    Serial.print(DebugStr);
}



///////////////////////////////////////////////////////////////////////////////
void ReadFlashToSDBuffer(uint32_t StartAddress, uint32_t EndAddress, char FileName[]){
    char  Buffer[SDBuffLen];
    uint16_t BufferCounter = 0;
    char DebugStr[80];
    File myFile;

    Serial.println("\nRead flash and write to SD.");   
    
    Serial.println("Try init card.");
    while (!SD.begin(SD_DETECT_NONE))
    {
      Serial.println("Init card error. Retry.");
      delay(1000);
    }
    Serial.println("Init card success.");


    Serial.print("File open : ");
    Serial.println(FileName);   

    myFile = SD.open(FileName, FILE_WRITE);
    if (!myFile) {
        Serial.print("Error opening file: ");
        Serial.println(FileName);        
        return;
    }

    Serial.println("Pin setup."); 
    FlashInitSetup();
    
    Serial.println("Hardware reset flash."); 
    ResetChip();
    
    Serial.println("Start read Flash.");   
    ReadDataLinesSetup();
/**/
    for(uint32_t Counter = StartAddress; Counter <= EndAddress; Counter++){
      uint16_t Data = ReadCycle(Counter);

      //write data to buffer
      Buffer[BufferCounter]= uint8_t(Data & 0x00FF); 
      BufferCounter++;
      Buffer[BufferCounter]= uint8_t(Data >>8); 
      BufferCounter++;
   
 /**/    
      // if buffer is full - write to flash
      if(BufferCounter == SDBuffLen){
        BufferCounter = 0;

        uint16_t WriteCount = myFile.write( Buffer,  SDBuffLen);   // Write buffer
        
        if(WriteCount != SDBuffLen){
          Serial.print("\nError: write buffer to file, ");
          sprintf(DebugStr,"try write %d byte, writed %d byte, address: 0x%08X", SDBuffLen, WriteCount, Counter);
          Serial.println(DebugStr);
          myFile.close();
          return;
        }
      }

      if( (Counter & 0x0FFFF) == 0){
        sprintf(DebugStr,"Read block: 0x%08X",Counter);
        Serial.println(DebugStr);
      }/**/
      
    }

    // if bufer is not empty write to SD
    if(BufferCounter > 0){
      myFile.write( Buffer,  SDBuffLen);   // Write buffer
    }
    Serial.println("Read complete...");
    myFile.close();
    
    Serial.println("Write File Completed\n");
    FlashSetupControlPinDefault();
}

////////////////////////////////////////////////////////////////////////////////
void sd_info(){
  bool disp = false;
  
  Serial.println("\nInitializing SD card...");
  while(!card.init(SD_DETECT_NONE)) { 
    if (!disp) {
      Serial.println("initialization failed. Is a card inserted?");
      disp = true;
    }
    delay(100);
  }

  Serial.println("A card is present.");

  // print the type of card
  Serial.print("Card type: ");
  switch (card.type()) {
    case SD_CARD_TYPE_SD1:
      Serial.println("SD1");
      break;
    case SD_CARD_TYPE_SD2:
      Serial.println("SD2");
      break;
    case SD_CARD_TYPE_SDHC:
      Serial.println("SDHC");
      break;
    default:
      Serial.println("Unknown");
  }

  if (!fatFs.init()) {
    Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
    return;
  }

  uint64_t volumesize;
  Serial.print("Volume type is FAT");
  Serial.println(fatFs.fatType(), DEC);

  volumesize = fatFs.blocksPerCluster();    // clusters are collections of blocks
  volumesize *= fatFs.clusterCount();       // we'll have a lot of clusters
  volumesize *= 512;                        // SD card blocks are always 512 bytes
  Serial.print("Volume size (Mbytes): ");
  volumesize /= 1024*1024;
  Serial.println(volumesize);
  Serial.println();
}

////////////////////////////////////////////////////////////////////////////////
void sd_ls(){
  Serial.println("\nFiles on the card");
  File root = SD.openRoot();

  // list all files in the card with date and size
  root.ls(LS_R);

  root.close(); /**/  
  Serial.println("\n");

}

////////////////////////////////////////////////////////////////////////////////
void sd_ls_l(){
  Serial.println("\nFiles on the card (name, date and size in bytes)");
  File root = SD.openRoot();

  // list all files in the card with date and size
  //  root.ls(LS_R | LS_DATE | LS_SIZE);
  root.ls(LS_R | LS_DATE | LS_SIZE);

  root.close(); /**/  
  Serial.println("\n");

}


///////////////////////////////////////////////////////////////////////////////
void setup()
{

  //InitFlash
  FlashInitSetup();

  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial);

  sd_info();
}






///////////////////////////////////////////////////////////////////////////////
void loop(void) {

    Serial.println("FLASH READ/WRITE/INFO programm");
    Serial.println("1 - read flash to file on SD");
    Serial.println("2 - write file to flash");
    Serial.println("3 - write file to flash in slow dumb mode");
    Serial.println("4 - erase flash!!!");    
    Serial.println("5 - get info about flash");
    Serial.println("6 - list files in root");
    Serial.println("7 - test write all flash via buffer");
    Serial.println("8 - test read flash (fierst and last 512 word)");
       
    while(1){
        Serial.print(">");
        Serial.flush();
        char Choise = GetSerialChar();
        Serial.println(Choise);
        if( Choise == '1' ){
          sd_ls();
          Serial.print("Enter file name: ");
          char FileName[MaxFileNameLen+1];
          GetSerialString(FileName, MaxFileNameLen);
          ReadFlashToSDBuffer(0x0, 0x1ffffff, FileName);
          break;
        }  
        else if( Choise == '2' ){
          sd_ls();
          Serial.print("Enter file name: ");
          char FileName[MaxFileNameLen+1];
          GetSerialString(FileName, MaxFileNameLen);
          WrireSDToFlashBuffer(0, FileName);
          break;
        }  
        else if( Choise == '3' ){
          sd_ls();
          Serial.print("Enter file name: ");
          char FileName[MaxFileNameLen+1];
          GetSerialString(FileName, MaxFileNameLen);
          WrireSDToFlash(0, FileName);
          break;
        }
        else if( Choise == '4' ){
          EraseAllFlash();
          break;
        }
        else if( Choise == '5' ){
          GetFlashInfo();
          break;
        }
        else if( Choise == '6' ){
          sd_ls_l();
          break;
        }
        else if( Choise == '7' ){
          TestAllProgramFlashBuffer();
          break;
        }
        else if( Choise == '8' ){
          TestReadFlash();
          break;
        } else {
            Serial.print("Type only 1-7\n>");          
        } 
    }
}
