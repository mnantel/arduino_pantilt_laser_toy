
#include <Servo.h> 
#include <stdio.h>
#include <Time.h>
 
Servo servo1;   
Servo servo2;    

float pie=3.14159; 
float sinsize=1700; //general resolution of the servos =lowest microsecond to highest microsecond,approximately
float event=sinsize * 100;  //some gigantic number so the movef function never runs out of ticks

//specific variables 
// servo default positions 
// WARNING:  when a higher or lower number is input than what the servo can handle, 
//           the servo may rapid to some manufacturer safe value 
// WARNING: refer to technical documentation for each servo to stay within its range. 
//  ccw   \->/   cw
//   600- 1200- 2400   =180 sweep servo  type 1800 point resolution
//  1050- 1550 - 1950   =90 sweep servo  type  900 point resolution
//  1500- 1700 - 1900   =1260 sweep winch type 400 point resolution 

//servo 1 800-2200 decrease to open
int svo1c   =1500;  //servo# closed position
int svo1o;
//servo 2 800-2200 increase to open 
int svo2c   =1500;  //servo# closed position
int svo2o;


//variables for use in all servo functions per servo

float count1 =0; // sine wave x variable counts from 1 to 1700 (servo resolution) only starts counting after wait# reaches its activation value.                 
float count1s=0; 
float speed1=0;  //a value that consists of small increment values that change in magnitude depending on if the wave starts slow and ends fast or vise versa.  
float speedtick1=0; //ticks off how long the hold position for the servo is in very small ms increments giving the illusion of a slower or faster moving servo 
float sinsize1=0; 
                 // y= a*sin(bx+c)+d 
                 // yvar# = a#(sin (b#*count#+c#)+d#  
float yvar1=0;   // actual ms value thrown at servo ranged, paused, speed shifted etc.  
float a1=0;     //a# amplitude higher value taller wave shorter value shorter wave by magnitude:   a=(highest # - lowest #)/2
float b1=0;     //b# lower value = longer wave or higher value=shorter wave this is phase shift or stretch of function b=2pi/(period*2) where period is desired wave size 
float c1=0;     //c# is x frequency offset = what part of curve want to look at 
float d1=0;     //d# is y offset  = 0.5*amplitude shifts the curve so it is wholey in 1st quadrant 
int per1=0;     //trigger value either 0 or 1 to declare that that servo has reached its final position and so servo movement sequence of all servos (once all report per#=1)can end.

float count2 =0; 
float count2s =0; 
float sinsize2=0; 
float speed2=0;
float speedtick2=0; 
float yvar2=0;
float a2=0; 
float b2=0; 
float c2=0; 
float d2=0; 
int per2=0;

char serialout [100];


// Servo positions we randomly pick for either axis to come up with a random new position each time
// Array with 13 elements

const int posArrayLow [] = {1100, 1200, 1300};
const int posArrayHigh [] = {1600, 1700, 1800};
const int posArray [] = {1100,1200,1300,1700, 1800, 1900};


int moveTimer = 0;

//start of primary move function that includes all servos and is called up and activated per each event
void movef(float ecycle,float s1,float w81,float spa1,float spb1,float yprev1,float ynext1,float s2, float w82,float spa2,float spb2,float yprev2,float ynext2)
{
         
         //counter master list: 
         //count = count ticker for primary loop to check to see if each servo needs to move 
         //count# = count for each servo that starts up once wait period for that servo is reached
         //count#s =extended count for each servo to account for multiple cycles in one move function
         //speedtick# =a tiny microsecond pause from 1 to a value between spa# to spb# where servo is held momentary at one value 
         //per# = either 1 or 0 marks end of all movement from that servo for the movef function. 
         
         delay(1);   //master delay 
         //resets and values established 
         int per1=0; 
         int per2=0; 
         count1 = 1;   
         count1s =1;  
         speedtick1 = 1;  
         b1=(2*pie/(sinsize*2));  //coefficient of sine math function 
         
         sinsize1=((s1*2)-1)*sinsize;  //ranges from s1=1,2,3,4,5 sinsize#= 1*1700,3*1700,5*1700,7*1700 
         
         count2 = 1; 
         count2s =1; 
         speedtick2 = 1;    
         b2=(2*pie/(sinsize*2));     
         sinsize2=((s2*2)-1)*sinsize;
         
          //position dependent sine wave coeficients
            if(ynext1 > yprev1)  
            {
              a1= (ynext1-yprev1)/2;     
              c1= (1.5)*pie;            
              d1= yprev1+a1;           
            }
            else  //(ynext# < yprev#)
            {
              a1= (yprev1-ynext1)/2;
              c1= (0.5)*pie;  
              d1= yprev1-a1; 
            }
              if(ynext2 > yprev2)
            {
              a2= (ynext2-yprev2)/2;  
              c2= (1.5)*pie; 
              d2= yprev2+a2; 
            }
            else  //(ynext# < yprev#)
            {
              a2= (yprev2-ynext2)/2;
              c2= (0.5)*pie;  
              d2= yprev2-a2; 
            }
            
         
            
       //##########   GLOBAL LOOP FOR ALL SERVOS #######################                           
   for (float count = 0; count < ecycle; count +=1)  
   {  
     
        //traditional speed values start off as spa# and end up as spb# as count# ticks away on the fly as curve is being drawn. 
        // result is a sine curve that is compressed in the x axis on one end (spa#=large number) and stretched on other end (spb#=small number).   
       
         if (spa1 > spb1) {speed1=((count1s+1)/sinsize)*(spa1-spb1) + spb1;} //start fast end slow 
         else {speed1= ((count1s+1)/sinsize)*(spb1-spa1)+ spa1;} // start slow end fast 
         
         if (spa2 > spb2) {speed2=((count2s+1)/sinsize)*(spa2-spb2) + spb2;} //start fast end slow 
         else  {speed2= ((count2s+1)/sinsize)*(spb2-spa2)+ spa2;} // start slow end fast    
        
        
        
  // servo #1   3 states or cases 
        
        if (count < w81) //condition 1 servo not ready to move yet      
            {
             servo1.writeMicroseconds(yprev1);
            }
        
       
         else if (count > w81 && count1 > sinsize1) //condition 3 motion is done and position is held 
            {
            servo1.writeMicroseconds(ynext1); 
            per1=1; //declares this servo is finished with its movement 
            }
         
         else if (count > w81)   //condition 2 sin wave function active with optional hold position while big loop asks other servos for their turn  
            {
              
                if (count1 < sinsize1 && speedtick1 == 1)  //new position of servo is written 
                {   
                   yvar1= a1*sin((count1)*b1+c1)+d1;  //the math function
                   servo1.writeMicroseconds(yvar1);   //throws a command at the servo 
                   speedtick1 += 1; // start of increment to count for possible pauses at this position to simulate slow 
                   count1 += 1; //increments sine wave operator x in y=f(x)
                   

                }
                else if (speedtick1 > 1 && speedtick1 < speed1)  //sine wave is sustained at old value for 1 to speed# as counted by speedtick# 
                {
                  servo1.writeMicroseconds(yvar1); 
                  speedtick1 += 1;  //increments speedtick1 to delay the servo at one position along its travel to simulate a speed
                }
                else //sine wave is now permitted to continue drawing and moving to points by having speedtick# reset 
                {
                  count1+=1; //locks out the sine function from going past sinsize by ever increasing count# 
                  speedtick1 = 1; //reset for next sin wave increment  through of sine fun
                     
                     if (count1/sinsize <= 1)    //count#s is given a positive or negative counter to follow sin wave so speed can be adjusted
                     {count1s +=1;}
                     else if (count1/sinsize >1 && count1/sinsize <2)
                     {count1s -=1;}
                     else if (count1/sinsize >=2 && count1/sinsize <3)
                     {count1s +=1;}
                     else if (count1/sinsize >=3 && count1/sinsize <4)  
                     {count1s -=1;}
                     else if (count1/sinsize >=4 && count1/sinsize <5)  
                     {count1s +=1;}
                     else if (count1/sinsize >=5 && count1/sinsize <6)  
                     {count1s -=1;} 
                     else if (count1/sinsize >=6 && count1/sinsize <7)  
                     {count1s +=1;}     
                }           
              
   
            }  //end if statement for case 2
 
       //servo #2   
          if (count < w82)     //notes same as servo #1 above 
            {
            servo2.writeMicroseconds(yprev2);  
            }
         
         else if (count>w82 && count2 > sinsize2) 
            {
            servo2.writeMicroseconds(ynext2);
            per2=1; 
            }
         
         else if (count > w82)    
            {
               
              if (count2 < sinsize2 && speedtick2 == 1)
                {   
                   yvar2= a2*sin((count2)*b2+c2)+d2;  
                   servo2.writeMicroseconds(yvar2);
                   speedtick2 += 1;   
                   count2 += 1; 
                 }
                else if (speedtick2 > 1 && speedtick2 < speed2)  
                {
                  servo2.writeMicroseconds(yvar2); 
                  speedtick2 += 1;
                }
                else 
                {
                  count2+=1; 
                 speedtick2 = 1; 
                 if (count2/sinsize <= 1)    
                     {count2s +=1;}
                     else if (count2/sinsize >1 && count2/sinsize <2)
                     {count2s -=1;}
                     else if (count2/sinsize >=2 && count2/sinsize <3)
                     {count2s +=1;}
                     else if (count2/sinsize >=3 && count2/sinsize <4)  
                     {count2s -=1;}
                     else if (count2/sinsize >=4 && count2/sinsize <5)  
                     {count2s +=1;}
                     else if (count2/sinsize >=5 && count2/sinsize <6)  
                     {count2s -=1;} 
                     else if (count2/sinsize >=6 && count2/sinsize <7)  
                     {count2s +=1;}     
                           
                }  
            }    
         
       if(per1 == 1 && per2 == 1)//breaks FOR loop out of further un necessary cycles as all servos report their movement complete
         {
          break; 
         }
    
   }  //############# END OF GLOBAL LOOP FOR ALL SERVOS ############  
 
} //end of void subroutine function for entire move function 


// start of program meat 
void setup() 
{   
  Serial.begin(115200);

  servo1.attach(6);     //signal of servo#1 to pin 3
  servo2.attach(5);     //signal of servo#2 to pin 5
  servo1.writeMicroseconds(1500);  
  servo2.writeMicroseconds(1500);

  pinMode(7, OUTPUT);
  digitalWrite(7, HIGH);

  
} 
void loop() 
{  
  //servo lockdown when power is on

  

  //notes: 
//key: 

// ecycle = time acts as the cut off when all servos have completed their one way motion 
// s# =number of times sine wave repeats
// w8#= 1000=1sec approx. each servo wait time 
// spa# = start speed, range: 1-25 or 1 fast, 25 slow   
// spb# = end speed, range: 1-25 or 1 fast, 25 slow   
// yprev# = previous servo position 
// ynext# = next position 
// 
//Servo servo1;   
//Servo servo2;   
//

 //notes end
  
  svo1c = svo1o;
  svo2c = svo2o;
 
  svo1o = posArray [random (0, 5)];
  svo2o = posArray [random (0, 5)];
  sprintf(serialout, "[Starting move] svo1 %d->%d || svo2 %d->%d", svo1c, svo1o, svo2c, svo2o);
  Serial.println(serialout);
  moveTimer = now();
  //key: movef(ecycle,s1, w81,spa1,spb1,yprev1,ynext1,s2, w82,spa2,spb2,yprev2,ynext2)
         movef(event , 1, 100,3  ,10  ,svo1c ,svo1o ,1 ,100,3  ,10   ,svo2c ,svo2o);
  Serial.println(String("[Move completed] ") + (now()-moveTimer) + "sec");       
  //delay(2000); 

  svo1c = svo1o;
  svo2c = svo2o;
 
  svo1o = posArray [random (0, 5)];
  svo2o = posArray [random (0, 5)];
  sprintf(serialout, "[Starting move] svo1 %d->%d || svo2 %d->%d", svo1c, svo1o, svo2c, svo2o);
  Serial.println(serialout);
    moveTimer = now();
  //key: movef(ecycle,s1, w81,spa1,spb1,yprev1,ynext1,s2, w82,spa2,spb2,yprev2,ynext2)
         movef(event , 1, 100,3  ,10  ,svo1c ,svo1o ,1 ,100,3  ,10   ,svo2c ,svo2o);
  Serial.println(String("[Move completed] ") + (now()-moveTimer) + "sec");              
  //delay(2000); 

} //end void loop      
  







