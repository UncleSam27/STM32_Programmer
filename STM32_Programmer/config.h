#ifndef CONFIG_H
#define CONFIG_H


/* Flash Pins
A0-A15  - PE0-PE15
A16-A24  - PA0-PA8
D0-D7  - PC0-PC7
D8-D15  - PB8-PB15
*/

#define CE    PD14
#define OE    PD13
#define BYTE  PD12
#define WP    PD10
#define RESET PD9
#define WE    PD8
#define RYBY  PD11

#define Up     GPIO_PIN_SET
#define Down   GPIO_PIN_RESET


// Set Timings
//reset
#define tRP 1500  //reset pulse width 1ms or 1000us in manual
#define tRH 50  //reset high before read 50ns in manual
//read
#define tPACC 30 // Page Access 25ns in manual
#define tRC 110  // Read Cycle 110ns in manual
#define tCEH 35  // Chip Enable hold time 35ns in manual
#define tOEH 10  // Output Enable hold time 10ns in manual
//write
#define tBUSY 110  //Write/Busy dely  90ms in Manual
#define tRB 10 //Ready/Busy recovery time 0ns in Manual
#define tWP 35 // Write Pulse width 35ns in manual
#define tCS 10 // Chip Select width 0ns in manual
#define tDS 50 // Data Setup 45ns in manual

//if RYBY not work
#define tWHWH1 250     //Write Buffer Program Operation 240µs in manual
#define tWHWH2 1500    //Sector Erase Operation Typ: 1000ms Max: 3500ms in manual 
#define tWHWH3 600000  //All flash Erase Operation S29GL512N Typ: 512sec Max: 1024sec in manual


#define MaxFileNameLen 25

#define BufferLen 16
#define HardwareBufferLen 16


///////////////////////////////////////////////////////////////////////////////
void delay_us(unsigned long us){ //Самая тупая задержка но работает
  for(unsigned long Counter = 0; Counter < us; Counter++){ // 42cycle = 1000μs
    for(unsigned long Nop = 0; Nop < 42; Nop++) __ASM volatile ("NOP");  //Да волшебное число 42, подобрано эксперементально
  }
}


///////////////////////////////////////////////////////////////////////////////
void delay_ns(unsigned long ns){ // 42cycle = 1000ns    1cycle = 23ns
  for(unsigned long Counter = 0; Counter < ns; Counter+=23){  //волшебное число 23, подобрано эксперементально.
    __ASM volatile ("NOP"); 
  }
}




#endif
