/*
* ----------------------------------------------------------------------------
* "THE BEER-WARE LICENSE" (Revision 42):
* <fakufaku@gmail.com> wrote this file. As long as you retain this notice you
* can do whatever you want with this stuff. If we meet some day, and you think
* this stuff is worth it, you can buy me a beer in return -- Robin Scheibler
* ----------------------------------------------------------------------------
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <limits.h>
#include <stdlib.h>

// Allows to disable interrupt in some parts of the code
#define ENTER_CRIT()    {char volatile saved_sreg = SREG; cli()
#define LEAVE_CRIT()    SREG = saved_sreg;}

// We assume 1MHz CPU clock divided by 16384
#define TWENTY_FIVE_MINUTES 360
#define FIVE_MINUTES 72

// OVF period is ~4.2s
#define TIMER1_CLKDIV16384 ((1 << CS13) | (1 << CS12) | (1 << CS11) | (1 << CS10))
// OVF period is ~1.05s
#define TIMER1_CLKDIV4096  ((1 << CS13) | (1 << CS12) | (1 << CS10))
// OVF period is ~0.26s
#define TIMER1_CLKDIV1024  ((1 << CS13) | (1 << CS11) | (1 << CS10))
// OVF period is ~0.13s
#define TIMER1_CLKDIV512   ((1 << CS13) | (1 << CS11))
// disable TIMER1
#define TIMER1_DISABLED 0
// macro to set timer1
#define SET_TIMER1(x) {TCCR1 = x;}

// beginning of work 
unsigned long timer_counter = 0;

// LED on/off array
unsigned char LED_array = 0;

// the state variable
#define IDLE 0
#define WORK 1
#define BUZZ_END_WORK 2
#define WAIT 3
#define REST 4
#define BUZZ_END_REST 5
unsigned char state = IDLE;

// macros to access individual LEDs
void led_on(int p)
{
  switch (p)
  {
    case 2: // LED1
      DDRB |= (1 << DDB3);
      DDRB |= (1 << DDB1);
      DDRB &= ~(1 << DDB4);

      PORTB &= ~(1 << 3);
      PORTB &= ~(1 << 4);
      PORTB |= (1 << 1);

      break;

    case 3: // LED2
      DDRB |= (1 << DDB3);
      DDRB |= (1 << DDB1);
      DDRB &= ~(1 << DDB4);

      PORTB &= ~(1 << 1);
      PORTB &= ~(1 << 4);
      PORTB |= (1 << 3);

      break;

    case 4: // LED3
      DDRB |= (1 << DDB3);
      DDRB |= (1 << DDB4);
      DDRB &= ~(1 << DDB1);

      PORTB &= ~(1 << 1);
      PORTB &= ~(1 << 4);
      PORTB |= (1 << 3);

      break;

    case 5: // LED4
      DDRB |= (1 << DDB3);
      DDRB |= (1 << DDB4);
      DDRB &= ~(1 << DDB1);

      PORTB &= ~(1 << 1);
      PORTB &= ~(1 << 3);
      PORTB |= (1 << 4);

      break;

    case 0: // LED5
      DDRB |= (1 << DDB1);
      DDRB |= (1 << DDB4);
      DDRB &= ~(1 << DDB3);

      PORTB &= ~(1 << 3);
      PORTB &= ~(1 << 1);
      PORTB |= (1 << 4);

      break;

    case 1: // LED6
      DDRB |= (1 << DDB1);
      DDRB |= (1 << DDB4);
      DDRB &= ~(1 << DDB3);

      PORTB &= ~(1 << 3);
      PORTB &= ~(1 << 4);
      PORTB |= (1 << 1);

      break;

    default:
      DDRB &= ~(1 << DDB1) & ~(1 << DDB3) & ~(1 << DDB4);
      PORTB &= ~(1 << 1) & ~(1 << 3) & ~(1 << 4);;
      break;
  }
}

// the start and stop buzz functions
void buzz_start()
{
  // set PB0 as output
  DDRB |= (1 << DDB0);

  // Fast PWM, OC0A, non-inverting
  // no clock divider (~4kHz @ clk 1MHz)
  TCCR0A = (1 << COM0A1) | (1 << WGM01) | (1 << WGM00);
  TCCR0B = (1 << CS00);
  OCR0A = 128;
}

void buzz_stop()
{
  TCCR0A = 0x0;
  DDRB &= ~(1 << DDB0);
}

// The timer overflow interrupt routine
SIGNAL(TIMER1_OVF_vect)
{
  ENTER_CRIT();

  switch(state)
  {
    case WORK:
      if (timer_counter > 0)
      {
        LED_array = ((1 << 5)-1) ^ (1 << ((TWENTY_FIVE_MINUTES-timer_counter)/FIVE_MINUTES))-1;
        timer_counter--;
      }
      else
      {
        // go to WAIT state
        state = BUZZ_END_WORK;
        timer_counter = 3;
        SET_TIMER1(TIMER1_CLKDIV512);
        LED_array = 0x20;
      }
      break;

    case BUZZ_END_WORK:
      // control buzzer
      if (timer_counter % 2 == 1)
        buzz_start();
      else
        buzz_stop();
      // control time
      if (timer_counter > 0)
      {
        timer_counter--;
      }
      else
      {
        // go to WAIT state
        state = WAIT;
        timer_counter = 0;
        LED_array = 0x1F;
        SET_TIMER1(TIMER1_CLKDIV4096);
      }
      break;

    case WAIT:
      if (timer_counter == 0)
      {
        LED_array = 0x20;
        timer_counter = 1;
      }
      else
      {
        LED_array = 0x1F;
        timer_counter = 0;
      }
      break;

    case REST:
      if (timer_counter > 0)
      {
        LED_array = (1 << 5);
        timer_counter--;
      }
      else
      {
        state = BUZZ_END_REST;
        timer_counter = 3;
        SET_TIMER1(TIMER1_CLKDIV512);
        LED_array = 0x00;
      }
      break;
      
    case BUZZ_END_REST:
      if (timer_counter > 0)
      {
        if (timer_counter % 2 == 1)
          buzz_start();
        else
          buzz_stop();
        timer_counter--;
      }
      else
      {
        // go to IDLE state
        state = IDLE;
        timer_counter = 0;
        LED_array = 0x00;
        SET_TIMER1(TIMER1_DISABLED);
      }
      break;

  }

  LEAVE_CRIT();
}

// The button interrupt routine
SIGNAL(INT0_vect)
{
  // go into safe mode
  ENTER_CRIT();
  
  // state machine
  switch (state)
  {
    case IDLE:
      // go into WORK time
      state = WORK;
      timer_counter = TWENTY_FIVE_MINUTES;
      SET_TIMER1(TIMER1_CLKDIV16384);
      LED_array = 0x1F;
      break;

    case WORK:
    case BUZZ_END_WORK:
    case WAIT:
      // go into REST time
      state = REST;
      timer_counter = FIVE_MINUTES;
      SET_TIMER1(TIMER1_CLKDIV16384);
      LED_array = 0x20;
      break;

    case BUZZ_END_REST:
    case REST:
      // go back to IDLE mod
      state = IDLE;
      SET_TIMER1(TIMER1_DISABLED);
      LED_array = 0x00;
      break;
  }
  
  // leave safe mode
  LEAVE_CRIT();
}

// prepare and go to sleep.
void go_to_sleep()
{
  // turn off all LEDs
  LED_array = 0;

  // Turn all ports into inputs and disable pull-ups (except for interrupt)
  DDRB &= ~(1 << DDB0) & ~(1 << DDB1) & ~(1 << DDB3) & ~(1 << DDB4);
  PORTB &= ~(1 << 0) & ~(1 << 1) & ~(1 << 3) & ~(1 << 4);

  // deepest sleep-mode
  MCUCR |= (1 << SM1); // power-down mode

  // Power reduction
  PRR |= (1 << PRADC);

  // sleep now
  MCUCR |= (1 << SE);

  // clear sleepy bit
  MCUCR &= ~(1 << SE);
}

int main()
{
  int i, j;
  unsigned int el;

  // enable interrupts
  sei();

  // setup button on PB2 
  DDRB &= ~(1 << DDB2);
  PORTB |= (1 << 2);     // enable pull-up
  MCUCR |= (1 << ISC01); // interrupt on falling edge
  GIMSK |= (1 << INT0);  // enable interrupt 0
 

  // set up timer 1 as system clock
  //TCCR1 = (1 << CS13) | (1 << CS12) | (1 << CS11) | (1 << CS10); // T1 clock to clk/16384
  TCCR1 = (1 << CS13) | (1 << CS12) | (1 << CS10); // T1 clock to clk/16384
  TIMSK = (1 << TOIE1);   // set the overflow interrupt

  LED_array = (1 << 7)-1;

  // The infinite loop
  while (1)
  {
    // turn off if IDLE
    if (state == IDLE)
      go_to_sleep();

    // display LEDs
    for (i = 0 ; i < 6 ; i++)
      if ((LED_array >> i) & 1)
        led_on(i);
  }

}
