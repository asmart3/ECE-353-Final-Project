#include "interrupts.h"
#include "main.h"
int DUTY_CYCLE = 0;
static volatile uint16_t PS2_X_DATA = 0;
static volatile uint16_t PS2_Y_DATA = 0;
volatile PS2_DIR_t PS2_DIR = PS2_DIR_CENTER;
volatile bool TIMER2_ALERT = false;
volatile PS2_DIR_t check = NULL;
bool trigger_timer1 = false;
volatile bool TIMER3_ALERT = false;
volatile bool TIMER1_ALERT = false;
bool rising = true;
uint8_t count=0;
//*****************************************************************************
// Returns the most current direction that was pressed.
//*****************************************************************************
PS2_DIR_t ps2_get_direction(void)
{
	float conversion_x;
	float conversion_y;
	PS2_DIR_t return_val;
	
	//converts joystick x and y readings to voltages
	conversion_x = (PS2_X_DATA)*(3.3)/(0xFFF);
	conversion_y = (PS2_Y_DATA)*(3.3)/(0xFFF);
	
	//determines which direction the joystick is in
	if(conversion_x > 2.4){
		//sets the current direction
		return_val = PS2_DIR_LEFT;
	} else if (conversion_x < 0.85){
		//sets the current direction
		return_val = PS2_DIR_RIGHT;
	} else if (conversion_y > 2.4){
		//sets the current direction
		return_val = PS2_DIR_UP;
	} else if (conversion_y < 0.85){
		//sets the current direction
		return_val = PS2_DIR_DOWN;
	} else if (conversion_x > 0.85 && conversion_x < 2.40){
		//sets the current direction
		return_val = PS2_DIR_CENTER;
	}
	
  return return_val;
}

void TIMER1A_Handler(void){
	TIMER1_ALERT = true;
	//Blink LED
	if(count < DUTY_CYCLE)
		lp_io_set_pin(BLUE_M);
	else
		lp_io_clear_pin(BLUE_M);
	
	count = (count+1) %100;
	// Clear the interrupt
	TIMER1->ICR |= TIMER_ICR_TATOCINT;
}

//*****************************************************************************
// TIMER2 ISR is used to determine when to move the tank
//*****************************************************************************
void TIMER2A_Handler(void)
{	
	
	TIMER2_ALERT = true;
	//change duty cycle of LED in addition to moving tank
	if(rising){
		if(DUTY_CYCLE>100)
			rising = false;
		else
			DUTY_CYCLE++;
	}
	else{
		if(DUTY_CYCLE<0)
			rising = true;
		else
			DUTY_CYCLE--;
	}
	
   // Clear the interrupt
	TIMER2->ICR |= TIMER_ICR_TATOCINT;
}

//*****************************************************************************
// TIMER3 ISR is used to determine when to move the enemie's tank
//*****************************************************************************
void TIMER3A_Handler(void)
{	
	TIMER3_ALERT = true;
	// Clear the interrupt
	TIMER3->ICR |= TIMER_ICR_TATOCINT;  
}

//*****************************************************************************
// TIMER4 ISR is used to trigger the ADC
//*****************************************************************************
void TIMER4A_Handler(void)
{	
	ADC0->PSSI |= ADC_PSSI_SS2;
	
	// Clear the interrupt
	TIMER4->ICR |= TIMER_ICR_TATOCINT; 
	
}

//*****************************************************************************
// ADC0 SS2 ISR
//*****************************************************************************
void ADC0SS2_Handler(void)
{
	PS2_X_DATA = ADC0->SSFIFO2;
	
	PS2_Y_DATA = ADC0->SSFIFO2;
	PS2_DIR = ps2_get_direction();

  // Clear the interrupt
  ADC0->ISC |= ADC_ISC_IN2;
}

