/*
 * Timer blink program for MSP430G2553
 *
 * Created: 10/2/19
 * Last Edited: 10/2/19
 * Author: Andrew Hollabaugh
 */

#include <msp430.h> //msp identifiers

//use P1.0 for led0 and P1.6 for led1
#define LED0_DIR_R P1DIR //define generic name for led0 port direction reg
#define LED0_OUT_R P1OUT //define generic name for led0 port out reg
#define LED0_PIN 0 //define led0 pin number of port
#define LED1_DIR_R P1DIR //define generic name for led0 port direction reg
#define LED1_OUT_R P1OUT //define generic name for led0 port out reg
#define LED1_PIN 6 //define led0 pin number of port

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD; //stop watchdog timer

    //led setup
    LED0_DIR_R |= (1 << LED0_PIN); //set led0 direction to output
    LED1_DIR_R |= (1 << LED1_PIN); //set led1 direction to output

    //timerA0 setup
    TA0CTL |= TASSEL_2; //select SMCLK as clk src
    TA0CTL |= ID_3; //set clk division to 8
    TA0CTL |= MC_1; //set to up mode
    TA0CCR0 = 0xC468; //set value timer will reset at; defines blink period
    TA0CTL |= TAIE; //enable timer interrupts

    //timerA1 setup
    TA1CTL |= TASSEL_2; //select SMCLK as clk src
    TA1CTL |= ID_3; //set clk division to 8
    TA1CTL |= MC_1; //set to up mode
    TA1CCR0 = 0x3234; //set value timer will reset at; defines blink period
    TA1CTL |= TAIE; //enable timer interrupts

    __bis_SR_register(LPM0_bits + GIE); //enter low power mode and enable interrupts

    while(1); //loop infinitely
}

//interrupt vector for timerA0
void __attribute__((interrupt(TIMER0_A1_VECTOR))) Timer_A0 (void)
{
    if(TA0IV == 0x0A) //if interrupt was generated from a timer overflow
        LED0_OUT_R ^= (1 << LED0_PIN); //toggle LED0 output using XOR
}

//interrupt vector for timerA1
void __attribute__((interrupt(TIMER1_A1_VECTOR))) Timer_A1 (void)
{
    if(TA1IV == 0x0A) //if interrupt was generated from a timer overflow
        LED1_OUT_R ^= (1 << LED1_PIN); //toggle LED1 output using XOR
}

