/*
 * Button based delay program for MSP430G2553
 * Blinks LED at speed defined by how long a button is pressed
 *
 * Created: 10/11/19
 * Last Edited: 10/11/19
 * Author: Andrew Hollabaugh
 */

#include <msp430.h> //msp identifiers

//use P1.0 for led; define generic names for gpio registers and gpio pin
#define LED_DIR_R P1DIR
#define LED_OUT_R P1OUT
#define LED_PIN 0

//use P1.3 for button; define generic names for gpio registers and gpio pin
#define BUTTON_DIR_R P1DIR
#define BUTTON_IN_R P1IN
#define BUTTON_PIN 3

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD; //stop watchdog timer

    //led setup
    LED_DIR_R |= (1 << LED_PIN); //set led direction to output
    LED_OUT_R &= ~(1 << LED_PIN); //ensure LED is off

    //button setup
    BUTTON_DIR_R &= ~(1 << BUTTON_PIN); //set button direction to input
    P1IFG = 0x00; //clear interrupt flags
    P1IES |= (1 << BUTTON_PIN); //set interrupt to be generated on rising edge
    P1IE |= (1 << BUTTON_PIN); //enable interrupt
  
    //timer0 setup, used for LED blink
    TA0CTL |= TASSEL_2; //select SMCLK as clk src
    TA0CTL |= ID_3; //set clk division to 8
    TA0CTL |= MC_1; //set to up mode
    TA0CCR0 = 0xFFFF; //set value timer will reset at; defines blink period, set to default value
    TA0CTL |= TAIE; //enable timer interrupts
    
    //timer1 setup, used for button timing
    TA1CTL |= TASSEL_2; //select SMCLK as clk src
    TA1CTL |= ID_3; //set clk division to 8
    TA1CTL |= MC_0; //ensure off

    __bis_SR_register(LPM0_bits + GIE); //enter low power mode and enable interrupts

    while(1); //loop infinitely
}

//interrupt vector for timerA0
void __attribute__((interrupt(TIMER0_A1_VECTOR))) Timer_A0 (void)
{
    if(TA0IV == 0x0A) //if interrupt was generated from a timer overflow
        LED_OUT_R ^= (1 << LED_PIN); //toggle LED0 output using XOR
}

//interrupt vector for button interrupt
void __attribute__ ((interrupt(PORT1_VECTOR))) Port_1 (void)
{
    P1IFG &= ~(1 << BUTTON_PIN); //clear interrupt flags

    if(!(BUTTON_IN_R & (1 << BUTTON_PIN))) //button press
    {
        LED_OUT_R &= ~(1 << LED_PIN); //toggle LED0 output using XOR
        TA0CTL &= ~TAIE; //disable timer interrupts
        P1IES &= ~(1 << BUTTON_PIN); //set interrupt to be generated on rising edge
        TA1CTL |= MC_0; //turn off timer1
        TA0CCR0 = TA1R; //copy value of button timing to compare register of led timer
        TA1CTL |= TACLR; //reset timer1
        TA1R = 0;
    }
    else //button release
    {
        P1IES |= (1 << BUTTON_PIN); //set interrupt to be generated on rising edge
        TA0CTL |= TAIE; //enable timer interrupts
        TA1CTL |= MC_2; //turn on timer1, set to continuous mode
    }
}

