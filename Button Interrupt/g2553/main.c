/*
 * Button interrupt blink program for MSP430G2553
 * Cycles through led states with off and three blinking speeds
 *
 * Created: 9/29/19
 * Last Edited: 9/29/19
 * Author: Andrew Hollabaugh
 */

#include <msp430.h> //msp identifiers
#include <stdint.h>

//use P1.0 for led; define generic names for gpio registers and gpio pin
#define LED_DIR_R P1DIR
#define LED_OUT_R P1OUT
#define LED_PIN 0

//use P1.3 for button; define generic names for gpio registers and gpio pin
#define BUTTON_DIR_R P1DIR
#define BUTTON_PIN 3

#define NUM_STATES 4
uint16_t states[] = {0x0000, 0x2000, 0x8000, 0xF000}; //TA0CCR0 values to cycle through
uint8_t state_index = 0;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD; //stop watchdog timer

    //led setup
    LED_DIR_R |= (1 << LED_PIN); //set led0 direction to output

    //button setup
    BUTTON_DIR_R &= ~(1 << BUTTON_PIN); //set button direction to input
    P1IFG = 0x00; //clear interrupt flags
    P1IES |= (1 << BUTTON_PIN); //set interrupt to be generated on rising edge
    P1IE |= (1 << BUTTON_PIN); //enable interrupt
  
    //timer setup
    TA0CTL |= TASSEL_2; //select SMCLK as clk src
    TA0CTL |= ID_3; //set clk division to 8
    TA0CTL |= MC_1; //set to up mode
    TA0CCR0 = 0x0000; //set value timer will reset at; defines blink period; set to 0 to start off
    TA0CTL |= TAIE; //enable timer interrupts

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
    if(state_index >= NUM_STATES - 1) //check if it is at the last state
        state_index = 0; //return to state 0
    else
        state_index++; //increment state
    TA0CCR0 = states[state_index]; //set blink period to the current state's blink period
    if(state_index == 0) //if in state 0, ensure led is off
        LED_OUT_R |= (1 << LED_PIN);
}

