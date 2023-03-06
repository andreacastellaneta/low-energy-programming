/* Project L476_ats_blink for STM32L476 mounted on Nucleo board:
 * the user LED (mounted on pin PA-5) is flashed every second for 50 ms.
 * The time base is provided by Systick (1000 ticks per second).
 * The clock configuration is the default one (Sysclk = 80 MHz, derived from MSI and PLL).
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "gpio.h"

#define T_LED_OFF 950 //ms
#define T_LED_ON 50 //ms

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

//Definition variables
RTC_TypeDef * RTC0 = RTC ;
uint32_t expe = 1;
uint8_t blue_mode = 0;
uint8_t expe1_loop = 0;
uint8_t impulse_time = 5;
uint8_t led_period = 200;

int main(void)
{

  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);


  /* Configure the system clock */
  //SystemClock_Config();


  /* Initialize all configured peripherals */
  // config GPIO
  GPIO_init();

  //Enable RTC-MPU
  LL_APB1_GRP1_EnableClock( LL_APB1_GRP1_PERIPH_PWR );
  LL_PWR_EnableBkUpAccess();

  //COLD BOOT
  if (!LL_RCC_LSE_IsReady()){
	  //Reset Backup Domain
	  LL_PWR_EnableBkUpAccess();
	  LL_RCC_ForceBackupDomainReset();
	  LL_RCC_ReleaseBackupDomainReset();

	  LL_RCC_LSE_Enable();
	  while ( !LL_RCC_LSE_IsReady());

	  LL_RCC_SetRTCClockSource(LL_RCC_RTC_CLKSOURCE_LSE);
	  LL_RCC_EnableRTC();

	  LL_RTC_InitTypeDef RTC_Str_Init;
	  RTC_Str_Init.HourFormat = LL_RTC_HOURFORMAT_24HOUR;
	  RTC_Str_Init.AsynchPrescaler = 255;
	  RTC_Str_Init.SynchPrescaler = 127;

	  //LL_RTC_Init(RTC0, &RTC_Str_Init);
		/* Disable the write protection for RTC registers */
	  LL_RTC_DisableWriteProtection(RTC0);

		/* Set Initialization mode */
	  if (LL_RTC_EnterInitMode(RTC0) != ERROR)
	  {
		while (!LL_RTC_IsActiveFlag_INIT(RTC0));
		/* Set Hour Format */
		LL_RTC_SetHourFormat(RTC0, RTC_Str_Init.HourFormat);

		/* Configure Synchronous and Asynchronous prescaler factor */
		LL_RTC_SetSynchPrescaler(RTC0, RTC_Str_Init.SynchPrescaler);
		LL_RTC_SetAsynchPrescaler(RTC0, RTC_Str_Init.AsynchPrescaler);

		/* Exit Initialization mode */
		LL_RTC_DisableInitMode(RTC0);
		while (LL_RTC_IsActiveFlag_INIT(RTC0));

	  }
	  /* Enable the write protection for RTC registers */
	  LL_RTC_EnableWriteProtection(RTC0);

	  LL_RTC_BAK_SetRegister(RTC0, LL_RTC_BKP_DR0, 1);
  }

  //EXPERIENCE UPDATE
  expe = LL_RTC_BAK_GetRegister(RTC0, LL_RTC_BKP_DR0);
  if ( BLUE_BUTTON() ){
	  if (expe==8){expe=0;}
	  LL_RTC_BAK_SetRegister(RTC0, LL_RTC_BKP_DR0, expe+1);
  }
  while (BLUE_BUTTON());
  expe = LL_RTC_BAK_GetRegister(RTC0, LL_RTC_BKP_DR0);

  switch (expe){
  	    case 1:
  	    	Init_Expe1();
  	    	//Init Sleep Config for expe=1
  	    	LL_LPM_EnableSleep();
  	      break;

  	    case 2:
  	    	Init_Other_Expe(1,1);
  	      break;

  	    case 3:
  	    	Init_Other_Expe(2,3);
  	    	//Init Sleep Config for expe=3
  	    	LL_LPM_EnableSleep();
  		  break;

  		case 4:
  			Init_Other_Expe(2,3);
  		  break;

  		case 5:
  			Init_Other_Expe(2,3);
  			//STOP0
  			LL_LPM_EnableSleep();
  			LL_RCC_MSI_EnablePLLMode();

  		  break;

  		case 6:
  			Init_Other_Expe(2,3);
  			//STOP1
  			LL_LPM_EnableSleep();
  			LL_RCC_MSI_EnablePLLMode();
  		  break;

  		case 7:
  			Init_Other_Expe(2,3);
  			//STOP2
  			LL_LPM_EnableSleep();
  			LL_RCC_MSI_EnablePLLMode();
  		  break;

  		case 8:
  			Init_Other_Expe(2,3);
  			//SHUTDOWN
  			LL_LPM_EnableSleep();
  			LL_RCC_MSI_EnablePLLMode();
  		  break;

  	    default:
  	    	//set expe à 1
  	    	expe = 1;
  	    	LL_RTC_BAK_SetRegister(RTC0, LL_RTC_BKP_DR0, expe);
  	    	//LL_RTC_BAK_SetRegister(RTC0, LL_RTC_BKP_DR0, 1);
  	    	//expe = LL_RTC_BAK_GetRegister(RTC0, LL_RTC_BKP_DR0);
  	    	Init_Expe1();
  	    	//Init Sleep Config for expe=1
  	    	LL_LPM_EnableSleep();
  	}





  // Clear Reset Flag before run
  //LL_RCC_ClearResetFlags();


  while (1)
  {
	  if (blue_mode){
		  switch (expe){
		  	    case 1:
		  	    	__WFI();
		  	      break;

		  	    case 2:
		  	    	//if lse ready = toujours le cas
		  	    	//msppllen
		  	    	LL_RCC_MSI_EnablePLLMode();
		  	      break;

		  	    case 3:
		  	    	__WFI();
		  		  break;

		  		case 4:
		  			//if lse ready = toujours le cas
					//msppllen
					LL_RCC_MSI_EnablePLLMode();
		  		  break;

		  		case 5:
		  			LL_LPM_EnableDeepSleep();
		  			LL_PWR_SetPowerMode(LL_PWR_MODE_STOP0);
		  			RTC_wakeup_init_from_stop(10);

		  			__WFI();
		  			blue_mode=0;
		  			LL_LPM_EnableSleep();
		  		  break;

		  		case 6:
					LL_LPM_EnableDeepSleep();
					LL_PWR_SetPowerMode(LL_PWR_MODE_STOP1);
					RTC_wakeup_init_from_stop(10);

		  			__WFI();
		  			blue_mode=0;
		  			LL_LPM_EnableSleep();
		  		  break;

		  		case 7:
		  			LL_LPM_EnableDeepSleep();
					LL_PWR_SetPowerMode(LL_PWR_MODE_STOP2);
					RTC_wakeup_init_from_stop(10);
		  			__WFI();
		  			blue_mode=0;
		  			LL_LPM_EnableSleep();
		  		  break;

		  		case 8:
		  			LL_LPM_EnableDeepSleep();
					LL_PWR_SetPowerMode(LL_PWR_MODE_SHUTDOWN);
					RTC_wakeup_init_from_standby_or_shutdown(10);
		  			__WFI();
		  			blue_mode=0;
		  			LL_LPM_EnableSleep();
		  		  break;
		  	    default:
		  	    	__WFI();
		  	}

	  }
	  else
	  {
		  switch (expe){
		  		case 5:
		  			__WFI();
		  		  break;

		  		case 6:
		  			__WFI();
		  		  break;

		  		case 7:
		  			__WFI();
		  		  break;

		  		case 8:
		  			__WFI();
		  		  break;
		  	    default:
		  	    	;
		  	}
	  }

  }
}


// systick interrupt handler
void SysTick_Handler()
{
	if ( BLUE_BUTTON() ){
		  	blue_mode = 1;
	}
	//Toggle Pin 10 - 50 Hz Signal
	LL_GPIO_TogglePin(GPIOC, LL_GPIO_PIN_10);

	//Reset Systick flag
	CLEAR_BIT(SysTick->CTRL, SysTick_CTRL_COUNTFLAG_Pos);

	//Management of LED blinking
	if ((expe1_loop >= 0) && (expe1_loop <=(impulse_time*expe)-1)){
		LED_GREEN(1);
		expe1_loop +=1;
	}
	else if (((expe1_loop >= impulse_time) && (expe1_loop <led_period))){
		LED_GREEN(0);
		expe1_loop +=1;
	}
	//Reset Cycle
	if (expe1_loop ==led_period){
		expe1_loop =0;
	}
}



/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follows :
  *            System Clock source            = PLL (MSI)
  *            SYSCLK(Hz)                     = 80000000
  *            HCLK(Hz)                       = 80000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            MSI Frequency(Hz)              = 4000000
  *            PLL_M                          = 1
  *            PLL_N                          = 40
  *            PLL_R                          = 2
  *            Flash Latency(WS)              = 4
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{

}

void Init_Expe1(void)
{
	LL_FLASH_SetLatency(LL_FLASH_LATENCY_4);
	while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_4)
	{
	}
	LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
//	LL_RCC_LSI_Enable();
//
//	/* Wait till LSI is ready */
//	while(LL_RCC_LSI_IsReady() != 1)
//	{
//
//	}
	LL_RCC_MSI_Enable();

	/* Wait till MSI is ready */
	while(LL_RCC_MSI_IsReady() != 1)
	{

	}
	LL_RCC_MSI_EnableRangeSelection();
	LL_RCC_MSI_SetRange(LL_RCC_MSIRANGE_6);
	LL_RCC_MSI_SetCalibTrimming(0);


//	LL_PWR_EnableBkUpAccess();
//	LL_RCC_SetRTCClockSource(LL_RCC_RTC_CLKSOURCE_LSI);
//	LL_RCC_EnableRTC();

	LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_MSI, LL_RCC_PLLM_DIV_1, 40, LL_RCC_PLLR_DIV_2);
	LL_RCC_PLL_EnableDomain_SYS();
	LL_RCC_PLL_Enable();

	/* Wait till PLL is ready */
	while(LL_RCC_PLL_IsReady() != 1)
	{

	}
	LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

	/* Wait till System clock is ready */
	while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
	{

	}
	LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
	LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
	LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

	LL_Init1msTick(80000000);

	LL_SetSystemCoreClock(80000000);

	//Systick Config
	// Init SysTick
	LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK_DIV8);
	WRITE_REG(SysTick->LOAD, 100000);
	SET_BIT(SysTick->CTRL, SysTick_CTRL_CLKSOURCE_Pos);
	SET_BIT(SysTick->CTRL, SysTick_CTRL_TICKINT_Pos);
	SET_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Pos);
}

void Init_Other_Expe(int Volt, int Latency)
{
	//PLL Off
	LL_RCC_PLL_Disable();

	//Flash Latency

	if (Latency == 3) {
		LL_FLASH_SetLatency(LL_FLASH_LATENCY_3);
		while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_3);
	}
	else{
		LL_FLASH_SetLatency(LL_FLASH_LATENCY_1);
		while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_1);
	}

	if (Volt == 2) {
		//Voltage Scaling
		LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE2);
	}
	else{
		//Voltage Scaling
		LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
	}

	//MSI Config
	LL_RCC_MSI_Enable();
	/* Wait till MSI is ready */
	while(LL_RCC_MSI_IsReady() != 1);
	LL_RCC_MSI_EnableRangeSelection();
	LL_RCC_MSI_SetRange(LL_RCC_MSIRANGE_9);
	LL_RCC_MSI_SetCalibTrimming(0);
	LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_MSI);
	/* Wait till System clock is ready */
	while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_MSI);

	//Clock System
	LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
	LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
	LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

	//A demander
	//LL_Init1msTick(24000000);
	LL_SetSystemCoreClock(24000000);

	// Init SysTick
	LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK_DIV8);
	WRITE_REG(SysTick->LOAD, 30000);
	SET_BIT(SysTick->CTRL, SysTick_CTRL_CLKSOURCE_Pos);
	SET_BIT(SysTick->CTRL, SysTick_CTRL_TICKINT_Pos);
	SET_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Pos);

}

// partie commune a toutes les utilisations du wakeup timer
static void RTC_wakeup_init( int delay )
{
	LL_RTC_DisableWriteProtection( RTC );
	LL_RTC_WAKEUP_Disable( RTC );
	while	( !LL_RTC_IsActiveFlag_WUTW( RTC ) );
	// connecter le timer a l'horloge 1Hz de la RTC
	LL_RTC_WAKEUP_SetClock( RTC, LL_RTC_WAKEUPCLOCK_CKSPRE );
	// fixer la duree de temporisation
	LL_RTC_WAKEUP_SetAutoReload( RTC, delay );	// 16 bits
	LL_RTC_ClearFlag_WUT(RTC);
	LL_RTC_EnableIT_WUT(RTC);
	LL_RTC_WAKEUP_Enable(RTC);
	LL_RTC_EnableWriteProtection(RTC);
}

// Dans le cas des modes STANDBY et SHUTDOWN, le MPU sera reveille par reset
// causé par 1 wakeup line (interne ou externe) (le NVIC n'est plus alimenté)
void RTC_wakeup_init_from_standby_or_shutdown( int delay )
{
RTC_wakeup_init( delay );
// enable the Internal Wake-up line
LL_PWR_EnableInternWU();	// ceci ne concerne que Standby et Shutdown, pas STOPx
}

// Dans le cas des modes STOPx, le MPU sera reveille par interruption
// le module EXTI et une partie du NVIC sont encore alimentes
// le contenu de la RAM et des registres étant préservé, le MPU
// reprend l'execution après l'instruction WFI
void RTC_wakeup_init_from_stop( int delay )
{
RTC_wakeup_init( delay );
// valider l'interrupt par la ligne 20 du module EXTI, qui est réservée au wakeup timer
LL_EXTI_EnableIT_0_31( LL_EXTI_LINE_20 );
LL_EXTI_EnableRisingTrig_0_31( LL_EXTI_LINE_20 );
// valider l'interrupt chez NVIC
NVIC_SetPriority( RTC_WKUP_IRQn, 1 );
NVIC_EnableIRQ( RTC_WKUP_IRQn );
}

// wakeup timer interrupt Handler (inutile mais doit etre defini)
void RTC_WKUP_IRQHandler()
{
LL_EXTI_ClearFlag_0_31( LL_EXTI_LINE_20 );
}
