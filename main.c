#include <stm32f0xx_gpio.h>
#include <stm32f0xx_rcc.h>

//Define interrupt rates
#define MillisecondsIT 1e3

//LED Definitions
#define GLed GPIO_Pin_9
#define BLed GPIO_Pin_8
#define LEDGPIO GPIOC

//Button Definitions
#define PushButton GPIO_Pin_0
#define PushButtonGPIO GPIOA

//Define time keeping variables
volatile uint32_t Milliseconds = 0, Seconds = 0;

//Systick interrupt handler, interrupts at "interrupt rate" per second
//Currently set to interrupt at millisecond intervals
void SysTick_Handler(void){
	Milliseconds++; //Increment millisecond variable
	if(Milliseconds%1000 == 999){ //If 1000 milliseconds have passed, increment seconds
		Seconds++;
	}
}

//Delay function for millisecond delay
void DelayMil(uint32_t MilS){
	volatile uint32_t MSStart = Milliseconds;
	while((Milliseconds - MSStart)<MilS) asm volatile("nop");
}

//Delay function for second delay
void DelaySec(uint32_t S){
	volatile uint32_t Ss = Seconds;
	while((Seconds - Ss)<S) asm volatile("nop");
}

//GPIO type def initialization
GPIO_InitTypeDef G;

int main(void)
{
	SystemInit(); //Ensure CPU is running at correctly set clock speed
	SystemCoreClockUpdate(); //Update SystemCoreClock variable to current clock speed
	SysTick_Config(SystemCoreClock/MillisecondsIT); //Set up a systick interrupt every millisecond

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE); //Enable GPIO clock for GPIOC (LED GPIOs)
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE); //Enable GPIO clock for GPIOA (Button GPIO)

	//Initialize LED GPIO's as outputs
	G.GPIO_Pin = BLed | GLed;
	G.GPIO_Mode = GPIO_Mode_OUT;
	G.GPIO_PuPd = GPIO_PuPd_NOPULL;
	G.GPIO_Speed = GPIO_Speed_Level_1;
	G.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(LEDGPIO, &G);

	//Initialize push button as input
	G.GPIO_Pin = PushButton;
	G.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(PushButtonGPIO, &G);

	//Initialize a variable to keep current LED state
	uint8_t ToggleVar = 1;

	//Set time variables to zero at the start of the infinite loop
	Milliseconds = Seconds = 0;

	while(1)
	{
		//Toggle the variable ToggleVar between each LED
		ToggleVar = ~ToggleVar;

		//If ToggleVar is equal to 1, turn on green led and turn blue led off
		if(ToggleVar == 1){
			GPIO_SetBits(LEDGPIO, GLed);
			GPIO_ResetBits(LEDGPIO, BLed);
		}
		else{ //Otherwise, flip LEDs
			GPIO_ResetBits(LEDGPIO, GLed);
			GPIO_SetBits(LEDGPIO, BLed);
		}

		if(GPIO_ReadInputDataBit(PushButtonGPIO, PushButton) == SET){ //If Push button is pressed
			DelaySec(1); //Delay for a second between toggles
		}
		else{ //If Push button isn't pressed
			DelayMil(100); //Delay for 100ms between toggles
		}
	}
}
