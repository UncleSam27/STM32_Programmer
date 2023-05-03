#ifndef LOWLEVEL_H
#define LOWLEVEL_H

#include "config.h"


///////////////////////////////////////////////////////////////////////////////
void FlashSetupControlPinDefault(){

  pinMode(BYTE, OUTPUT);
  pinMode(WP, OUTPUT);
  pinMode(CE, OUTPUT);
  pinMode(OE, OUTPUT);
  pinMode(WE, OUTPUT);
  pinMode(RESET, OUTPUT);
            
  // All control Pin - Up
  digitalWrite(BYTE, HIGH); // Word mode
  digitalWrite(WP, HIGH);   // Write protect Off
  digitalWrite(CE, HIGH);   // Chip Enable Off
  digitalWrite(OE, HIGH);   // Output Enable off
  digitalWrite(WE, HIGH);   // Write Enable off
  digitalWrite(RESET, HIGH);// Reset Disable
  
  //RY/BY# - PD11
  pinMode(RYBY, INPUT_PULLUP);  
}


///////////////////////////////////////////////////////////////////////////////
void FlashInitSetup(){
  
  FlashSetupControlPinDefault();
  
  // A0-A15  - PE0-PE15 
  pinMode(PE0, OUTPUT);
  pinMode(PE1, OUTPUT);
  pinMode(PE2, OUTPUT);
  pinMode(PE3, OUTPUT); 
  pinMode(PE4, OUTPUT);
  pinMode(PE5, OUTPUT);
  pinMode(PE6, OUTPUT);
  pinMode(PE7, OUTPUT); 
  pinMode(PE8, OUTPUT);
  pinMode(PE9, OUTPUT);
  pinMode(PE10, OUTPUT);
  pinMode(PE11, OUTPUT); 
  pinMode(PE12, OUTPUT);
  pinMode(PE13, OUTPUT);
  pinMode(PE14, OUTPUT);
  pinMode(PE15, OUTPUT); 

  // A16-A24  - PA0-PA8
  pinMode(PA0, OUTPUT);
  pinMode(PA1, OUTPUT);
  pinMode(PA2, OUTPUT);
  pinMode(PA3, OUTPUT); 
  pinMode(PA4, OUTPUT);
  pinMode(PA5, OUTPUT);
  pinMode(PA6, OUTPUT);
  pinMode(PA7, OUTPUT); 
  pinMode(PA8, OUTPUT);


  //INPUTS
   // D0-D7  - PC0-PC7
  pinMode(PC0, INPUT);
  pinMode(PC1, INPUT);
  pinMode(PC2, INPUT);
  pinMode(PC3, INPUT);
  pinMode(PC4, INPUT);
  pinMode(PC5, INPUT);
  pinMode(PC6, INPUT);
  pinMode(PC7, INPUT);
                

  // D8-D15  - PB8-PB15
  pinMode(PB8, INPUT);
  pinMode(PB9, INPUT);
  pinMode(PB10, INPUT);
  pinMode(PB11, INPUT);
  pinMode(PB12, INPUT);
  pinMode(PB13, INPUT);
  pinMode(PB14, INPUT);
  pinMode(PB15, INPUT);

}


///////////////////////////////////////////////////////////////////////////////
void ReadDataLinesSetup(){
  
  // D0-D7  - PC0-PC7
  pinMode(PC0, INPUT);
  pinMode(PC1, INPUT);
  pinMode(PC2, INPUT);
  pinMode(PC3, INPUT);
  pinMode(PC4, INPUT);
  pinMode(PC5, INPUT);
  pinMode(PC6, INPUT);
  pinMode(PC7, INPUT);
                

  // D8-D15  - PB8-PB15
  pinMode(PB8, INPUT);
  pinMode(PB9, INPUT);
  pinMode(PB10, INPUT);
  pinMode(PB11, INPUT);
  pinMode(PB12, INPUT);
  pinMode(PB13, INPUT);
  pinMode(PB14, INPUT);
  pinMode(PB15, INPUT);

  FlashSetupControlPinDefault();
  delay_ns(tCEH);

  digitalWrite(CE, LOW); // Chip Enable
  delay_ns(tCEH - tOEH);
    
  digitalWrite(OE, LOW); // Output Enable
  delay_ns(tOEH);
  
}


///////////////////////////////////////////////////////////////////////////////
void WriteDataLinesSetup(){

  FlashSetupControlPinDefault();
  delay_us(tRP);
  
  // D0-D7  - PC0-PC7
  pinMode(PC0, OUTPUT);
  pinMode(PC1, OUTPUT);
  pinMode(PC2, OUTPUT);
  pinMode(PC3, OUTPUT);
  pinMode(PC4, OUTPUT);
  pinMode(PC5, OUTPUT);
  pinMode(PC6, OUTPUT);
  pinMode(PC7, OUTPUT);
                
  // D8-D15  - PB8-PB15
  pinMode(PB8,  OUTPUT);
  pinMode(PB9,  OUTPUT);
  pinMode(PB10, OUTPUT);
  pinMode(PB11, OUTPUT);
  pinMode(PB12, OUTPUT);
  pinMode(PB13, OUTPUT);
  pinMode(PB14, OUTPUT);
  pinMode(PB15, OUTPUT);
}

///////////////////////////////////////////////////////////////////////////////
inline void SetAddress(unsigned int Address){
  uint16_t Addr_D00D15 = Address & 0x0000FFFF;
  uint16_t Addr_D16D24 = (Address >> 16) & 0x1FF;

  // Setup low 16 bits address
  // A0-A15  - PE0-PE15
  GPIOE->ODR = Addr_D00D15;    

  // Setup high 9 bits address
  // A16-A24  - PA0-PA8
  uint16_t  TmpRegA = GPIOA->IDR;              //read regA
  TmpRegA = TmpRegA & 0xFE00;                  //clear low 9 bit
  TmpRegA = TmpRegA | Addr_D16D24;             //setup low 9 bit
  GPIOA->ODR = TmpRegA;                        //write regA

}

///////////////////////////////////////////////////////////////////////////////
inline uint16_t GetData(){
  uint16_t DataL = (GPIOC->IDR & 0x00FF);        // D0-D7  - PC0-PC7
  uint16_t DataH = (GPIOB->IDR & 0xFF00);        // D8-D15  - PB8-PB15
  uint16_t Result = (DataH | DataL); 
  return  Result;
}

///////////////////////////////////////////////////////////////////////////////
inline void SetData(uint16_t Data){
  uint16_t LowData =  (Data & 0x00FF);
  uint16_t HighData = (Data & 0xFF00);
  
  //pc0-pc7
  uint16_t  TmpRegC = GPIOC->IDR;                //read regC
  TmpRegC = (TmpRegC & 0xFF00);                  //clear low 8 byte
  TmpRegC = (TmpRegC | LowData);                 //setup low 8 byte 
  GPIOC->ODR = TmpRegC;                          //write regC

  //pb8-pb15
  uint16_t  TmpRegB = GPIOB->IDR;                //read regB
  TmpRegB = (TmpRegB & 0x00FF);                  //clear high 8 byte
  TmpRegB = (TmpRegB | HighData);                //setup high 8 byte 
  GPIOB->ODR = TmpRegB;                          //write regB

}


///////////////////////////////////////////////////////////////////////////////
int8_t WaitFlashReady(){
  uint32_t Count=0;
  while(digitalRead(RYBY) == LOW){
    Count++;
    if(Count == 0xFFFFFE){
      return -1;
    }
  }
  return 0;
}


///////////////////////////////////////////////////////////////////////////////
void ResetChip(){
  //HAL_GPIO_WritePin (GPIOD, RESET_pin, Down); // reset
  digitalWrite(RESET, LOW);// Reset Enable
  delay_us(tRP);
  //HAL_GPIO_WritePin (GPIOD, RESET_pin, Up); // NO reset
  digitalWrite(RESET, HIGH);// Reset Disable
  delay_ns(tRH);

  // Ждем готовности
  WaitFlashReady();
}

///////////////////////////////////////////////////////////////////////////////
uint16_t ReadCycle(uint32_t Address){
    static uint32_t OldPage=0xFFFFFFF8;
    
    SetAddress(Address);           // Setup Address
    
    if(OldPage == (Address & 0xFFFFFFF8) ){ // if some page 
      delay_ns(tPACC);                   // short delay
    }else{                               // if new page 
      delay_ns(tRC);                     // standart delay
      OldPage = Address & 0xFFFFFFF8;       // renew Page num
    }
    return GetData(); // Get Data
}

////////////////////////////////////////////////////////////////////////////////
void WriteCycle(uint32_t Address, uint16_t Data){
    //OE must be HIGH

    SetAddress(Address);           // Setup Address
    digitalWrite(CE, LOW);         // Chip Enable
    delay_ns(tCS);

    SetData(Data);                 // Setup Data

    digitalWrite(WE, LOW);         // Write pulse
    delay_ns(tDS);
    digitalWrite(WE, HIGH);      
    delay_ns(tWP);
    
    digitalWrite(CE, HIGH);        // Chip disable
    delay_ns(tCS);
}

#endif
