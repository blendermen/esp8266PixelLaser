#include "sweeper.h"
//klasa silniczkow
  Sweeper::Sweeper(int interval)
  {
    updateInterval = interval;
    increment = 1;
    nowaPozycja = random(5,70);
    tmpP=servo.read();
    pozycja = servo.read();
    waiter = false;
	servo.write(90);
	lastPosition = 0;
  }
  
  void Sweeper::Attach(int pin)
  {
    servo.attach(pin,500, 2400);
  }
  
  void Sweeper::Detach()
  {
    servo.detach();
  }
  
  void Sweeper::Update()
  {
    if((millis() - lastUpdate) > updateInterval)  // time to update
    {
       lastUpdate = millis();
 if(!waiter){
 
  
  if(pozycja < nowaPozycja) { 
    //++pozycja;
      servo.write(++pozycja);
      //delay(predkosc); 
      Serial.printf("zmien pozycje - a %i %i \n",pozycja,nowaPozycja);
   
  }  
    if (pozycja > nowaPozycja) { 
    //--pozycja;
     servo.write(--pozycja);
      //delay(predkosc);
      Serial.printf("zmien pozycje - b %i %i \n",pozycja,nowaPozycja);
    }
 }
      
    //after the engine moves to the destination position pause at the momeent
     if(pozycja == nowaPozycja ){
     
  
      if(!waiter){
       
        waiter = true;
        updateInterval+=700;
        }else{
           
          nowaPozycja = random(5,70);
           updateInterval = random(5,20);
          waiter = false;
          }
      
      }
      
  }
  }

  void Sweeper::UpdateManual(int &nowaPozycja)
  {
    if((millis() - lastUpdate) > updateInterval)  // time to update
    {
       lastUpdate = millis();
    //++pozycja;
	
	 
	 if (lastPosition != nowaPozycja )
	 {
      servo.write(nowaPozycja);
	  lastPosition = nowaPozycja;
	  }
      //delay(1); 
     // Serial.printf("zmien pozycje manualnie -  %i \n",nowaPozycja);
 
      
  }
  }
