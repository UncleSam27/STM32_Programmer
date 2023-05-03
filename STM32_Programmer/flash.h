#ifndef FLASH_H
#define FLASH_H

#include "config.h"
#include "lowlevel.h"
#include "extmode.h"




///////////////////////////////////////////////////////////////////////////////
void EraseSector(uint32_t Address){
    uint32_t SectorAddress = (Address &0xFF0000); 
    
    char DebugStr[40];
    sprintf(DebugStr,"\nErase sector: 0x%08X", SectorAddress);
    Serial.println(DebugStr);
    Serial.println("Pin setup."); 
    FlashInitSetup();
    WriteDataLinesSetup();
    
    Serial.println("Hardware reset flash."); 
    ResetChip();
    Serial.println("Start erase sector."); 
    Serial.print("[");

    WriteCycle(0x555,0xAA); //1 bus cycle
    WriteCycle(0x2AA,0x55); //2 bus cycle
    WriteCycle(0x555,0x80); //3 bus cycle
    WriteCycle(0x555,0xAA); //4 bus cycle
    WriteCycle(0x2AA,0x55); //5 bus cycle
    WriteCycle(SectorAddress,0x30); //6 bus cycle

    delay_ns(tBUSY);

    while(digitalRead(RYBY) == LOW){
      Serial.print("."); //print dot evry 1 sec
      delay(1000);
    }/**/
    //delay(tWHWH2);
    
    Serial.println("]");
    Serial.println("Erase sector Done!\n");
}

///////////////////////////////////////////////////////////////////////////////
void EraseAllFlash(){
    Serial.println("\nErase all flash!"); 

    Serial.println("Pin setup."); 
    FlashInitSetup();
    WriteDataLinesSetup();
    
    Serial.println("Hardware reset flash."); 
    ResetChip();
    Serial.println("Start erase Flash."); 
    Serial.print("[");

    WriteCycle(0x555,0xAA); //1 bus cycle
    WriteCycle(0x2AA,0x55); //2 bus cycle
    WriteCycle(0x555,0x80); //3 bus cycle
    WriteCycle(0x555,0xAA); //4 bus cycle
    WriteCycle(0x2AA,0x55); //5 bus cycle
    WriteCycle(0x555,0x10); //6 bus cycle

    delay_ns(tBUSY);

    while(digitalRead(RYBY) == LOW){
      Serial.print("."); //print dot evry 5 sec
      delay(5000);
    }/**/
    //delay(tWHWH3);

    
    Serial.println("]");
    Serial.println("Erase chip Done!\n");
}



///////////////////////////////////////////////////////////////////////////////
void ProgramWord(uint32_t Address, uint16_t Data){

    WriteCycle(0x555,0xAA); //1 bus cycle
    WriteCycle(0x2AA,0x55); //2 bus cycle
    WriteCycle(0x555,0xA0); //3 bus cycle
    WriteCycle(Address,Data); //4 bus cycle
    delay_ns(tBUSY);

    WaitFlashReady();{
    }
    delay_ns(tRB);
}


///////////////////////////////////////////////////////////////////////////////
int8_t ProgramBuffer(uint32_t StartAddress, uint8_t Count, uint16_t Buffer[]){
    uint32_t SectorAddress = (StartAddress & 0xFFFF0000); 

    // if Count more then hardware buffer return error 
    if(Count > HardwareBufferLen)  
        return( -1 );
    // if start and end buffer element not in same sector   
    if((StartAddress & 0xfffffff0) != ((StartAddress + Count - 1)  & 0xfffffff0)) 
        return( -2 ); 
        
    WriteCycle(0x555,0xAA); //1 bus cycle
    WriteCycle(0x2AA,0x55); //2 bus cycle
    WriteCycle(StartAddress, 0x25); //3 bus cycle
    WriteCycle(SectorAddress, Count-1); //4 bus cycle

    for(uint8_t Counter = 0; Counter <Count; Counter++){
      WriteCycle(StartAddress+Counter, Buffer[Counter]); //5-20 bus cycle  
    }

    delay_ns(tBUSY);
    WaitFlashReady();
    WriteCycle(SectorAddress,0x29); //last bus cycle
    if(WaitFlashReady() == -1){
      return -3;
    }
    //delay_us(tWHWH1); //wait program time if RYBY not work!
    return(0);
}



///////////////////////////////////////////////////////////////////////////////
void GetFlashInfo(){
    char DebugStr[40];
    uint16_t Data,DataE,DataF;
    
    Serial.println("\nGet Flash info."); 

    FlashInitSetup();
    WriteDataLinesSetup();
    
    WriteCycle(0x555,0xAA); //1 bus cycle
    WriteCycle(0x2AA,0x55); //2 bus cycle
    WriteCycle(0x555,0x90); //3 bus cycle
    
    ReadDataLinesSetup();
    
    Data  = ReadCycle(0x0001);
    DataE = ReadCycle(0x000E);
    DataF = ReadCycle(0x000F); 

    sprintf(DebugStr,"Device ID  0x%04X", DataE);
    Serial.println(DebugStr);

    if((Data == 0x227E) && (DataF == 0x2201)){      // 0x227E and 0x2201- static value from datasheet
      if(DataE == 0x2223){
        Serial.println("Type: S29GL512NH/L");
      }
      else if(DataE == 0x2222){
        Serial.println("Type: S29GL256NH/L");
      }
      else if(DataE == 0x2221){
        Serial.println("Type: S29GL128NH/L");
      }
    }else{
      Serial.println("Error: Error get device info!\n");
      return;
    }

    Serial.println();

    Data = ReadCycle(0x00);
    sprintf(DebugStr,"Manufacturer ID  0x%04X", Data);
    Serial.println(DebugStr);
    if(Data == 0x01){
      Serial.println("Manufacturer: Spansion - FASL LLC");       
    }
    Serial.println();
    WaitFlashReady();
    
    FlashSetupControlPinDefault();
}

#endif
