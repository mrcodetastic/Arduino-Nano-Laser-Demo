// C Program to shuffle a given array 
#include <stdio.h> 
#include <stdlib.h> 
#include <time.h> 

// FastLED is also running in parallel on a seperate PIN which isn't connected to a laser
// as I have a 6 or so RGB leds I was using for testing the sync/lighting before connecting
// all the PWM pins to a laser. You can ignore this.
#include <FastLED.h>

// For led chips like Neopixels, which have a data line, ground, and power, you just
// need to define DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806 define both DATA_PIN and CLOCK_PIN
#define DATA_PIN 2
#define NUM_LEDS    6
#define BRIGHTNESS  64
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

// Define the array of leds
CRGB leds[NUM_LEDS];

const uint8_t active_leds =6 ; // can't be any greater than 6.
uint8_t random_led[active_leds] = { 0 } ;

uint16_t bpm_duration_ms = 60000 / 145;
uint16_t leds_min_bpm = bpm_duration_ms / active_leds ; // minimum bpm between leds
uint8_t light_show_num = 0; // from serial input

enum light_show_states {WAITING, RUNNING};
light_show_states light_show_state = WAITING;

uint8_t counter = 0;
boolean flipper = true;

unsigned long curr_ms;
unsigned long prev_ms;

const uint8_t pwm_ports[6] = { 3, 5,6,9,10,11 };


// BOARD PWM PINS  PWM FREQUENCY
/*
Uno, Nano, Mini

3, 5, 6, 9, 10, 11

490 Hz (pins 5 and 6: 980 Hz)
*/

void setup() { 

      delay(100);
      Serial.begin(115200);
      delay(100);

      // Setup the random led assignment array
      for (uint8_t led_num = 0; led_num < active_leds; led_num++)     
        random_led[led_num] = led_num;

      uint8_t n = sizeof(random_led)/ sizeof(random_led[0]); 
      randomize (random_led, n);          
      
      // Setup LEDs WS2812B
      FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
      FastLED.setBrightness(  BRIGHTNESS );
      fill_solid(leds, NUM_LEDS, CRGB::Black); // set to black
      FastLED.show(); pwm_pins_update();


      Serial.print("CRGB::Black is (in DEC): ");
      Serial.println(CRGB::Black, HEX);

      Serial.print("CRGB::White is (in DEC): ");
      Serial.println(CRGB::White, HEX);      

      Serial.print("CRGB::Green is (in DEC): ");
      Serial.println(CRGB::Green, HEX);            

      Serial.println("Awaiting track to play on Serial Input...");           
       
}

void pwm_pins_update()
{
  
 for (int i  = 0; i < active_leds; i++) // active leds should = 6
 {
    //  analogWrite(pwm_ports[i], 
    switch (leds[i])
    {
      case 0x00:
        analogWrite(pwm_ports[i], 0);
        break;
/*
      case 0xFFFFFF:
        analogWrite(pwm_ports[i], 255);
        break;
*/
       default:
        analogWrite(pwm_ports[i], 255);  //128); // make it extra bright
        break;
    } // end swtich       

 }

} // end pwm_pins_updatea

void pwm_pins_set_all(uint8_t pwmval)
{
  

 for (int i  = 0; i < active_leds; i++) // active leds should = 6
 {
    analogWrite(pwm_ports[i], pwmval);
 }

  
}

void loop_seq4_rand()
{
   int delayx= bpm_duration_ms / active_leds ; // for 8 leds
     
  for (int lednum = 0; lednum < active_leds; lednum++)
  {
        fill_solid(leds, active_leds, CRGB::Black); // set to black    
        
        for (uint8_t i = 0; i < 2; i++)
        {
            if ((i+lednum) < active_leds)
              leds[random_led[lednum+i]] = CRGB::White;
        }
        FastLED.show(); pwm_pins_update();
        delay(delayx);
  }
}



void loop_seq4(CRGB ledcolor)
{
   int delayx= bpm_duration_ms / active_leds ; // for 8 leds
     
  for (int lednum = 0; lednum < active_leds; lednum++)
  {
        fill_solid(leds, active_leds, CRGB::Black); // set to black    
        
        for (uint8_t i = 0; i < 1; i++)
        {
            if ((i+lednum) < active_leds)
              leds[lednum+i] = ledcolor;
        }
        FastLED.show(); pwm_pins_update();
        delay(delayx);
  }
}


void loop_seq4_flash_first(CRGB ledcolor)
{
   int delayx= bpm_duration_ms / active_leds ; // for 8 leds
     
  for (int lednum = 0; lednum < active_leds; lednum++)
  {
        fill_solid(leds, active_leds, CRGB::Black); // set to black  

        if (lednum == 0)
            fill_solid(leds, active_leds, CRGB::White); // set to black  
        
        for (uint8_t i = 0; i < 1; i++)
        {
            if ((i+lednum) < active_leds)
              leds[lednum+i] = ledcolor;
        }
        FastLED.show(); pwm_pins_update();
        delay(delayx);
  }
}

void pwm_fade_in_beat()
{
      prev_ms = millis();

      uint8_t pwm_value = 255;
      while (true)
      {           
            if ( (millis() - prev_ms) >=  bpm_duration_ms)
            {
                    Serial.println("Duration exceeded");
                    break;
            }     

            if (pwm_value > 0)
            {
                 //Serial.println("Value reduced to zero");
                 pwm_pins_set_all(pwm_value--);
                 //analogWrite(11, pwm_value--);
                 delay(1);
                 //break;             
            }
            //analogWrite(11, pwm_value--);
          //  delay(1);

       //   Serial.print("pwm value: ");
       //   Serial.println(pwm_value, DEC);
      }

      pwm_pins_set_all(0);
      //analogWrite(11, 0); 
}

void pwm_flash(uint8_t pwmval)
{
       prev_ms = millis();

       //analogWrite(11, pwmval);
       pwm_pins_set_all(pwmval);
       delay(bpm_duration_ms/2);


       //analogWrite(11, 0);
       pwm_pins_set_all(0);       
       delay(bpm_duration_ms/2);         
       
} // end flast

void pwm_set_all(uint8_t pwmval)
{
       prev_ms = millis();

       //analogWrite(11, pwmval);
       pwm_pins_set_all(pwmval);       
       delay(bpm_duration_ms);     
       
} // end flast  


void flash_leds(uint8_t duration, CRGB color)
{
      fill_solid(leds, NUM_LEDS, color); // set to black
      FastLED.show(); pwm_pins_update();     
      delay(duration);
}


void flash_leds(uint8_t duration, CRGB color, uint8_t num_leds) // only flash for a select number of leds, 
{
      Serial.println("Special flash leds!");
        
      fill_solid(leds, NUM_LEDS, CRGB::Black); // set to black

      uint8_t n = sizeof(random_led)/ sizeof(random_led[0]); 
      randomize (random_led, n); 

      //if ( num_leds == 0)
     //   num_leds = random (2, active_leds); // randomly select it for us

      for (uint8_t num = 0; num < active_leds; num++)
      {
         //    Serial.println("----");
        //     Serial.println(num, DEC);        
        //     Serial.println(num_leds, DEC);
         //    Serial.println(num % num_leds, DEC);
             
          if (num % num_leds == 0)
          {
            leds[random_led[num]] = color;              
          }
      }
            
      FastLED.show(); pwm_pins_update();     
      delay(duration);
}


void effect_leds_random(CRGB ledcolor,  uint8_t num_leds_flashing) /// all done within one beat, 
{
      uint8_t n = sizeof(random_led)/ sizeof(random_led[0]); 
      randomize (random_led, n); 
      //  Serial.println("Random leds:");
      //  printArray(random_led, n); 

      uint8_t max_leds = (num_leds_flashing >= active_leds) ? active_leds:num_leds_flashing;

   //     fill_solid(leds, NUM_LEDS, CRGB::Black); // set to black        
        for (uint8_t num = 0; num < max_leds; num++)
        {
            leds[random_led[num]] = ledcolor;
            FastLED.show(); pwm_pins_update();
            delay(leds_min_bpm);                    
        }

      for (uint8_t i = max_leds; i < active_leds; i++)
      {        delay(leds_min_bpm); } // fill any void
      

}


void effect_leds_serial(CRGB ledcolor) /// all done within one beat
{

  for (int lednum = 0; lednum < active_leds; lednum++)
  {
        leds[lednum] = ledcolor;
        FastLED.show(); pwm_pins_update();
        delay(leds_min_bpm);
  }
  
}




  


void loop() 
{ 

        // Wait for input from Serial
        if (light_show_state == WAITING)
        {
              // send data only when you receive data:
              if (Serial.available() > 0) {
                      // read the incoming byte:
                      uint8_t incomingByte = Serial.read();
      
                      // say what you got:
                      Serial.print("We received over serial: ");
                      Serial.println(incomingByte, DEC);

                      if ( incomingByte == 49 || incomingByte == 50) // 49 is decimal for ASCII '1'
                      {

                        light_show_num = incomingByte-48;
                        light_show_state = RUNNING;

                        Serial.print("Starting light show for track ");
                        Serial.println(light_show_num);     

                        delay(20);
                        
                      } // end incoming byte valididty check
                      else
                      {
                        Serial.println("Got some random shit from the Serial input");
                      }
              } // end serial availble check
        } // end check if the light_show_state is waiting...
        else
        {

          light_show_1_mainloop();
          light_show_2_mainloop();
          
        }

        


          
} // end loop


void light_show_2_mainloop()
{
          // quick check
          if (light_show_num != 2) return;
          

          curr_ms = millis();
          
          int delayx= bpm_duration_ms / active_leds ; // for 8 leds
          
          if (counter == 0)
          {
            Serial.println("START2");
          }              

          counter= counter + 1;          
          
          Serial.print("Beat count is: ");
          Serial.println(counter, DEC);
                       

           if ( counter <= 12)
           {

         
             loop_seq4_rand();
            
           }
           else if ( counter <= 14)
           {
                        
              pwm_flash(32);
           }
          else if ( counter <= 16)
           {

              pwm_flash(255);            
           }                      
           else if ( counter <= 20)
           {
  
            // part 1/4
            loop_seq4_flash_first(CRGB::Blue);
         
            // part 2/4
            loop_seq4(CRGB::Red);

            // part 4/4
            loop_seq4(CRGB::Green);
            

            // part 4/4    
            /*        
           fill_solid(leds, active_leds, CRGB::Cyan); // set to black
           FastLED.show(); pwm_pins_update();
           delay(bpm_duration_ms); 
           */
           pwm_fade_in_beat(); 


            // part 1/4
            loop_seq4_flash_first(CRGB::Blue);
         
            // part 2/4
            loop_seq4(CRGB::Red);

            // part 4/4
            loop_seq4(CRGB::Green);

        
            // part 4/4
            leds[0] = CRGB::White;  FastLED.show(); pwm_pins_update(); delay(delayx);
            leds[3] = CRGB::White;  FastLED.show(); pwm_pins_update(); delay(delayx);
            leds[1] = CRGB::White;  FastLED.show(); pwm_pins_update(); delay(delayx);
            leds[2] = CRGB::White;  FastLED.show(); pwm_pins_update(); delay(delayx);     

            // extra leds
            leds[4] = CRGB::White;  FastLED.show(); pwm_pins_update(); delay(delayx);
            leds[5] = CRGB::White;  FastLED.show(); pwm_pins_update(); delay(delayx);            

           }
           else if (counter <= 21) // final section
           {
                delay(10);       
                for (uint8_t i = 0; i <= 23; i++)
                {   

              //    Serial.println(i);
         
                  if (!flipper)
                  {
                   fill_solid(leds, active_leds, CRGB::Black); // set to black
                  }
                  else
                  {
                     fill_solid(leds, active_leds, CRGB::White); // set to black              
                  }

                       FastLED.show(); pwm_pins_update();
                       delay(bpm_duration_ms/2);    
                   
                       flipper = !flipper;
                }



                for (uint8_t i = 0; i <= 22; i++)
                {   

                //  Serial.println(i);
         
                  if (flipper)
                  {
                   fill_solid(leds, active_leds, CRGB::Black); // set to black
                  }
                  else
                  {
                     fill_solid(leds, active_leds, CRGB::White); // set to black              
                  }

                       FastLED.show(); pwm_pins_update();
                       delay(bpm_duration_ms/10);    
                   
                       flipper = !flipper;
                }

             //   delay(10000);

           } /// end last counter check
           else if (counter <= 22) // 5 minutes 40 seconds and 623ms into the song...
           {
        
              delay(bpm_duration_ms/2);
              loop_seq4_rand();
           }

           else if (counter <= 25)
           {
                pwm_set_all(255);
                pwm_set_all(128);
                pwm_set_all(32);  
                pwm_set_all(0);    
                loop_seq4_rand();
                loop_seq4_rand();
                loop_seq4_rand();                
                loop_seq4_rand();                
           }
           else if (counter <= 26)
           {
                pwm_set_all(255);
                pwm_set_all(200);
                pwm_set_all(128);  
                pwm_set_all(64);    
                pwm_set_all(32); 
                loop_seq4_rand();
                loop_seq4_rand();                
                loop_seq4_rand();                
           }           
           else if (counter <= 27) // fade over four beats
           {         
                uint8_t mapval = 255;
                while (true)
                { 
                    // reusing prev_ms variable as  actually the difference in ms
                    prev_ms = millis() - curr_ms; 

              //      Serial.println(prev_ms);

                    if (prev_ms > bpm_duration_ms*4) break;
                    
                    mapval = map (prev_ms, 0, (bpm_duration_ms*4)+10, 255, 0);

               //     Serial.println(mapval);

                    //analogWrite(11, mapval);
                           pwm_pins_set_all(mapval);

                    delay(4);
                    //analogWrite(11, 0);
                     //delay(2);
                }

                pwm_set_all(0);  
           }
           else
           {
                Serial.println("Finished lightshow. ");

                light_show_state = WAITING; light_show_num = 0;
                counter = 0;
           }
}


void light_show_1_mainloop()
{

    // quick check
    if (light_show_num != 1) return;  

    if (counter == 0)
    {
      Serial.println("START1");
    }      

     // Start
     effect_leds_random(CRGB::White, active_leds); 
     delay(bpm_duration_ms*3); 

      // 2nd
     effect_leds_random(CRGB::Black, active_leds);   
     delay(bpm_duration_ms);      

     // https://jsfiddle.net/1nzd5uqm/2/
     flash_leds(87, CRGB::Green, 0 );
     flash_leds(329, CRGB::Red, 2 );
     flash_leds(176, CRGB::Green, 1 );
     flash_leds(87, CRGB::Blue, 2 );
     flash_leds(329, CRGB::Red, 0 );
     flash_leds(176, CRGB::Green, 1 );


      effect_leds_random(CRGB::Black, active_leds);     
      delay(bpm_duration_ms*3); 

     effect_leds_random(CRGB::White, active_leds);  
     delay(bpm_duration_ms);   

     //https://jsfiddle.net/1nzd5uqm/2/


     flash_leds(100, CRGB::Green );
     flash_leds(100, CRGB::Black );
   
     flash_leds(100, CRGB::Red );
     flash_leds(100, CRGB::Black );
   
     flash_leds(50, CRGB::Green );
     flash_leds(50, CRGB::Black );   
   
     flash_leds(50, CRGB::Blue );
     flash_leds(50, CRGB::Black );     
   
     flash_leds(50, CRGB::Red );
     flash_leds(50, CRGB::Black );   
   
     flash_leds(50, CRGB::Green );  
     flash_leds(50, CRGB::Black );          

     effect_leds_random( CRGB::Blue, active_leds); 
      delay(bpm_duration_ms*3);      
     flash_leds(0, CRGB::Black );  
     effect_leds_random( CRGB::Red, active_leds); 

     delay(bpm_duration_ms);   

     flash_leds(103*2, CRGB::Green, 1 );
     flash_leds(103*2, CRGB::Red, 2 );
     flash_leds(103, CRGB::Green, 1 );
     flash_leds(103, CRGB::Blue, 2 );
     flash_leds(103, CRGB::Red,1  );
     flash_leds(103, CRGB::Green,2 );  
            
      flash_leds(0, CRGB::Black );  
     effect_leds_random( CRGB::Yellow, active_leds); 
      delay(bpm_duration_ms*3);      
       flash_leds(0, CRGB::Black );  
     effect_leds_random( CRGB::Pink, active_leds); 
      delay(bpm_duration_ms); 


      uint8_t ledno_prev_3 = 0;  
      uint8_t ledno_prev_2 = 0;
      uint8_t ledno_prev_1 = 0;
      uint8_t ledno = 0;
      for (int i = 1; i < 45; i++)
      {
            if ( ledno < active_leds-1) { ledno++; } else { ledno = 0; }
            Serial.println(ledno);

            fill_solid(leds, NUM_LEDS, CRGB::Black); // set to black 
            leds[ledno] = CRGB( 255, 255, 255);   
            delay(i);
            FastLED.show(); pwm_pins_update(); 
      }

      delay(1000);
      light_show_state = WAITING; light_show_num = 0;
      counter = 0;      


}





// -----------------------------------


// A utility function to swap to integers 
void swap (uint8_t *a, uint8_t *b) 
{ 
    uint8_t temp = *a; 
    *a = *b; 
    *b = temp; 
} 
  
// A utility function to print an array 
void printArray (uint8_t arr[], uint8_t n) 
{ 
    for (uint8_t i = 0; i < n; i++) 
    {
      Serial.print("LED ");    Serial.print(i, DEC); Serial.print(": ");
      Serial.println(arr[i], DEC);
    }
} 
  
// A function to generate a random permutation of arr[] 
//https://www.geeksforgeeks.org/shuffle-a-given-array-using-fisher-yates-shuffle-algorithm/
void randomize ( uint8_t arr[], uint8_t n ) 
{ 
    // Use a different seed value so that we don't get same 
    // result each time we run this program 
    srand ( time(NULL) ); 
  
    // Start from the last element and swap one by one. We don't 
    // need to run for the first element that's why i > 0 
    for (uint8_t i = n-1; i > 0; i--) 
    { 
        // Pick a random index from 0 to i 
        uint8_t j = rand() % (i+1); 
  
        // Swap arr[i] with the element at random index 
        swap(&arr[i], &arr[j]); 
    } 
} 
