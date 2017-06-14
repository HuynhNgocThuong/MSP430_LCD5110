#include "msp430g2553.h"
#include "LCD_NOKIA_5110.h"
#include "UART.h"
#define SIZE_DATA_BUFFER 13

char c[SIZE_DATA_BUFFER];
int i;

unsigned char serialRead()
{
	while(!(IFG2&UCA0RXIFG));   //USCI_A0 RX buffer ready ?
	return UCA0RXBUF;

}
void serialWrite(unsigned char c)
{
	while(!(IFG2&UCA0TXIFG));  // USCI_A0 TX buffer ready ?
	UCA0TXBUF=c; // TX
}
void serialwriteString(const char *str)
{
	while(*str)
		serialWrite(*str++);
}
void serialWrite_Int(unsigned long n)
{
     unsigned char buffer[16];
     unsigned char i,j;

     if(n == 0) {
    	 serialWrite('0');
          return;
     }

     for (i = 15; i > 0 && n > 0; i--) {
          buffer[i] = (n%10)+'0';
          n /= 10;
     }

     for(j = i+1; j <= 15; j++) {
    	 serialWrite(buffer[j]);
     }
}

void main (void) 
{ 
WDTCTL = WDTPW + WDTHOLD;  //Stop Watchdog Timer 
BCSCTL1 = CALBC1_1MHZ;
DCOCTL = CALDCO_1MHZ;//chon thach anh dao dong noi la 1MHZ
LCD5110_Init(30,0,4);
P1DIR |= BIT6;
P1SEL = BIT1 + BIT2 ;                     // P1.1 = RXD, P1.2=TXD
P1SEL2 = BIT1 + BIT2 ;                     // P1.1 = RXD, P1.2=TXD
UCA0CTL1 |= UCSSEL_2;                     // SMCLK
UCA0BR0 = 104;                            // 1MHz 9600
UCA0BR1 = 0;                              // 1MHz 9600
UCA0MCTL = UCBRS0;                        // Modulation UCBRSx = 1
UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
__bis_SR_register(GIE);  //cho phep ngat
while(1){
  if(i == 12){
    __delay_cycles(1000);
    LCD5110_Clr();
    LCD5110_GotoXY(0,0);
    LCD5110_String(c);
    i = 0;
  }
    P1OUT ^= BIT6;
    __delay_cycles(10000);
  }
}
//  Echo back RXed character, confirm TX buffer is ready first
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
  c[i] = serialRead();
  i++;
  serialWrite_Int(i);
}
