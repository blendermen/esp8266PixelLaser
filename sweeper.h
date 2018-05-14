#include <Servo.h> 
//klasa silniczkow
class Sweeper
{
  Servo servo;              // the servo
  int pos;              // current servo position 
  int increment;        // increment to move for each interval
  int  updateInterval;      // interval between updates
  unsigned long lastUpdate; // last update of position
  int nowaPozycja;
  int pozycja;
  int tmpP;
  bool waiter;
  int lastPosition;
public: 
  Sweeper(int interval);
  void Attach(int pin);
  void Detach();
  void Update();
  void UpdateManual(int & newpos);

};