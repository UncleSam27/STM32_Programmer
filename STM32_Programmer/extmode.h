#ifndef EXTMODE_H
#define EXTMODE_H

#include "config.h"
#include "lowlevel.h"


///////////////////////////////////////////////////////////////////////////////
void SecSiSectorEntry(){
    WriteCycle(0xAAA,0xAA); //1 bus cycle
    WriteCycle(0x555,0x55); //2 bus cycle
    WriteCycle(0xAAA,0x88); //3 bus cycle
    delay_ns(tBUSY);

    WaitFlashReady();
    delay_ns(tRB);
}

///////////////////////////////////////////////////////////////////////////////
void SecSiSectorExit(){
    WriteCycle(0xAAA,0xAA); //1 bus cycle
    WriteCycle(0x555,0x55); //2 bus cycle
    WriteCycle(0xAAA,0x90); //3 bus cycle
    WriteCycle(0x000,0x00); //4 bus cycle
    delay_ns(tBUSY);

    WaitFlashReady();
    delay_ns(tRB);
}

///////////////////////////////////////////////////////////////////////////////
void BypassModeEntry(){
    WriteCycle(0x555,0xAA); //1 bus cycle
    WriteCycle(0x2AA,0x55); //2 bus cycle
    WriteCycle(0x555,0x20); //3 bus cycle
    delay_ns(tBUSY);

    WaitFlashReady();
    delay_ns(tRB);
}

///////////////////////////////////////////////////////////////////////////////
void BypassModeProgramWord(uint32_t Address, uint16_t Data){
#define tBUSY 110  //Write/Busy dely  90ns in Manual
#define tRB   50   //Ready/Busy recovery time 0ns in Manual

    WriteCycle(Address,0xA0); //1 bus cycle
    WriteCycle(Address,Data); //2 bus cycle
    delay_ns(tBUSY);

    WaitFlashReady();
    delay_ns(tRB);
}

///////////////////////////////////////////////////////////////////////////////
void BypassModeReset(){
#define tBUSY 110  //Write/Busy dely  90ms in Manual
#define tRB   50   //Ready/Busy recovery time 0ms in Manual

    WriteCycle(0x0,0x90); //1 bus cycle
    WriteCycle(0x0,0x0);  //2 bus cycle
    delay_ns(tBUSY);

    WaitFlashReady();
    delay_ns(tRB);
}

///////////////////////////////////////////////////////////////////////////////
void BypassModeTestProgramFlash(){  //test program flash in bypass mode

    Serial.println("\nTest Write flash in bypass mode!"); 

    Serial.println("Pin setup."); 
    FlashInitSetup();
    WriteDataLinesSetup();
    
    Serial.println("Hardware reset flash."); 
    ResetChip();
    
    Serial.println("Bypass mode entry");     
    BypassModeEntry();
    
    Serial.println("Start write."); 
    char DebugStr[40];
    for(uint32_t Counter=0; Counter <= 0x1FF; Counter++){
      sprintf(DebugStr,"Write: 0x%08X  0x%04X", Counter, Counter);
      Serial.println(DebugStr);
      BypassModeProgramWord(Counter, Counter);
    }
    Serial.println("Bypass mode reset"); 
    BypassModeReset();
    
    Serial.println("Test write in bypass mode done.\n");    
    FlashSetupControlPinDefault();
}

#endif
