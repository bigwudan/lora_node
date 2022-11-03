/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2016 Semtech

Description: Tx Continuous Wave implementation

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis and Gregory Cristian
*/
#include <string.h>
#include "user.h"
#include "sx126x-board.h"
#include "uart.h"
#include "rtc.h"

#include "lora_common.h"

/**************************************************************************************************************************************
Demo ????  EnableMaster=true  ????,???????"PING"????????,?????????"PONG"??LED??

               EnableMaster=false ????,?????????????"PING"???LED???????"PONG"??????
***************************************************************************************************************************************/

#define USE_MODEM_LORA
//#define USE_MODEM_FSK

#define REGION_CN779

#if defined( REGION_AS923 )

#define RF_FREQUENCY                                923000000 // Hz

#elif defined( REGION_AU915 )

#define RF_FREQUENCY                                915000000 // Hz

#elif defined( REGION_CN779 )

#define RF_FREQUENCY                                433000000 // Hz

#elif defined( REGION_EU868 )

#define RF_FREQUENCY                                868000000 // Hz

#elif defined( REGION_KR920 )

#define RF_FREQUENCY                                920000000 // Hz

#elif defined( REGION_IN865 )

#define RF_FREQUENCY                                865000000 // Hz

#elif defined( REGION_US915 )

#define RF_FREQUENCY                                915000000 // Hz

#elif defined( REGION_US915_HYBRID )

#define RF_FREQUENCY                                915000000 // Hz

#else

    #error "Please define a frequency band in the compiler options."

#endif

#define TX_OUTPUT_POWER                             22        // 22 dBm

extern bool IrqFired;




bool EnableMaster=false;//????


static int _statue = 0; //0 idi  1 wait recv 2 next sxd
static int _idx = 0;

uint16_t  crc_value;
/*!
 * Radio events function pointer
 */

static RadioEvents_t RadioEvents;

#if defined( USE_MODEM_LORA )

#define LORA_BANDWIDTH                              0         // [0: 125 kHz,    
                                                              //  1: 250 kHz,
                                                              //  2: 500 kHz,
                                                              //  3: Reserved]
#define LORA_SPREADING_FACTOR                       9         // [SF7..SF12]    
#define LORA_CODINGRATE                             4         // [1: 4/5,       
                                                              //  2: 4/6,
                                                              //  3: 4/7,
                                                              //  4: 4/8]
#define LORA_PREAMBLE_LENGTH                        8         // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT                         0         // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON                  false
#define LORA_IQ_INVERSION_ON                        false

#elif defined( USE_MODEM_FSK )

#define FSK_FDEV                                    5e3      // Hz 
#define FSK_DATARATE                                2.4e3      // bps
#define FSK_BANDWIDTH                               20e3     // Hz >> DSB in sx126x
#define FSK_AFC_BANDWIDTH                           100e3     // Hz
#define FSK_PREAMBLE_LENGTH                         5         // Same for Tx and Rx
#define FSK_FIX_LENGTH_PAYLOAD_ON                   false

#else
    #error "Please define a modem in the compiler options."
#endif

typedef enum
{
    LOWPOWER,
    RX,
    RX_TIMEOUT,
    RX_ERROR,
    TX,
    TX_TIMEOUT,
}States_t;

#define RX_TIMEOUT_VALUE                            1000
#define BUFFER_SIZE                                 64 // Define the payload size here

const uint8_t PingMsg[] = "PING";
const uint8_t PongMsg[] = "PONG";

uint16_t BufferSize = BUFFER_SIZE;
uint8_t TX_Buffer[BUFFER_SIZE];
uint8_t RX_Buffer[BUFFER_SIZE];


States_t State = LOWPOWER;

int8_t RssiValue = 0;
int8_t SnrValue = 0;
void OnTxDone( void );
void OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr );
void OnTxTimeout( void );
void OnRxTimeout( void );
void OnRxError( void );



void SysClock_48()
{ 
   RCC_PLLCmd(DISABLE);
   RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_12);//48M
   RCC_PLLCmd(ENABLE);
   while(!RCC_GetFlagStatus(RCC_FLAG_PLLRDY));
   RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
}

void Tick_Configration()
{
    /* Setup SysTick Timer for 1ms interrupts ( not too often to save power ) */
    if( SysTick_Config( SystemCoreClock / 1000 ) )
    { 
        /* Capture error */ 
        while (1);
    }
}

void RCC_Configuration()
{   
  /* Enable GPIO clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA|RCC_AHBPeriph_GPIOB|RCC_AHBPeriph_GPIOC|RCC_AHBPeriph_GPIOF, ENABLE);
  
  /* Enable peripheral Clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2|RCC_APB1Periph_PWR, ENABLE);  
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_SYSCFG, ENABLE);
}

void timerInit()
{
        TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
        NVIC_InitTypeDef NVIC_InitStructure;
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

        TIM_TimeBaseStructure.TIM_Period=19; //9
        TIM_TimeBaseStructure.TIM_Prescaler=4799;
        TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
        TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
        TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

        TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); 

        NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
        NVIC_InitStructure.NVIC_IRQChannelPriority=1;
        NVIC_Init(&NVIC_InitStructure); 

        TIM_Cmd(TIM3, ENABLE);	
}


void HW_int(void)
{
    SysClock_48(); 
    Tick_Configration();
    RCC_Configuration();
    GPIO_int();
    SPI2_Int();
		UartInit(9600);
		timerInit();
		rtc_set();
}


void LedToggle(void)
{
	return ;
  GPIO_WriteBit( LED1_PORT, LED1_PIN,Bit_RESET);//LED??
  HAL_Delay_nMS(10);
  GPIO_WriteBit( LED1_PORT, LED1_PIN,Bit_SET);
}


static void _CadDone  ( bool channelActivityDetected ){

	printf("caddone[%d]\n", channelActivityDetected);
	if(channelActivityDetected == 0){
        TX_Buffer[0] = 0x55; 
        TX_Buffer[1] = 0x07;
        TX_Buffer[2] = 0x02;
				TX_Buffer[3] = 0x22;
				//Radio.Standby();
				Radio.Send( TX_Buffer, 4);		
	}else{
		_statue  =2;
		_idx = 0;
	
	}
}

//set cad 
void check_cad(){
	printf("check_cad\n");
	SX126xSetDioIrqParams( IRQ_RADIO_ALL,IRQ_RADIO_ALL,IRQ_RADIO_NONE,IRQ_RADIO_NONE );
	
	//SX126xSetCadParams(LORA_CAD_02_SYMBOL, 22, 10, LORA_CAD_ONLY, 0);
	SX126xSetCadParams(LORA_CAD_02_SYMBOL, 28, 10, LORA_CAD_ONLY, 0);
	SX126xSetCad();
	return ;
}



int main(void){
	HW_int();//吴丹
	
	Delay_Ms(5000);
	
	lora_node_init(); //初始化
	while(1){
		Radio.IrqProcess( ); // Process Radio IRQ
		lora_node_task();		

	}
}


#if 0
int main( void )
{   
   // uint8_t data[2]={0x12,0x34};
   // static uint8_t test[2];
    
    HW_int();//吴丹
    
    
    // Radio initialization
    RadioEvents.TxDone = OnTxDone;
    RadioEvents.RxDone = OnRxDone;
    RadioEvents.TxTimeout = OnTxTimeout;
    RadioEvents.RxTimeout = OnRxTimeout;
    RadioEvents.RxError = OnRxError;
		RadioEvents.CadDone = _CadDone;
    
    
    Radio.Init( &RadioEvents );
    Radio.SetChannel( RF_FREQUENCY );
    
  //  Radio.WriteBuffer(0x06C0,data,2);
   // Radio.ReadBuffer(0x06C0,test,2);
    
#if defined( USE_MODEM_LORA )
    
    Radio.SetTxConfig( MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                                   LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                                   LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                                   true, 0, 0, LORA_IQ_INVERSION_ON, 3000 );
    
    Radio.SetRxConfig( MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                                   LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                                   LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                                   0, true, 0, 0, LORA_IQ_INVERSION_ON, true );
    
#elif defined( USE_MODEM_FSK )
    
    Radio.SetTxConfig( MODEM_FSK, TX_OUTPUT_POWER, FSK_FDEV, 0,
                                  FSK_DATARATE, 0,
                                  FSK_PREAMBLE_LENGTH, FSK_FIX_LENGTH_PAYLOAD_ON,
                                  true, 0, 0, 0, 3000 );
    
    Radio.SetRxConfig( MODEM_FSK, FSK_BANDWIDTH, FSK_DATARATE,
                                  0, FSK_AFC_BANDWIDTH, FSK_PREAMBLE_LENGTH,
                                  0, FSK_FIX_LENGTH_PAYLOAD_ON, 0, true,
                                  0, 0,false, false );
#else
    #error "Please define a frequency band in the compiler options."
#endif


		printf("*********************************************\n");
		printf("******************state[%d]******************\n", EnableMaster);
		printf("*********************************************\n");
	
    if(EnableMaster)
    {
			printf("master rev:\n");
       Radio.Rx( RX_TIMEOUT_VALUE ); 
    }
    else
    {
				//biaosi
        TX_Buffer[0] = 0x55; 
        TX_Buffer[1] = 0x07;
        TX_Buffer[2] = 0x02;
				TX_Buffer[3] = 0x22;
				Radio.Send( TX_Buffer, 4);
    }

    while( 1 )
    {
			if(_statue == 0){
					//biaosi
//        TX_Buffer[0] = 0x55; 
//        TX_Buffer[1] = 0x07;
//        TX_Buffer[2] = 0x02;
//				TX_Buffer[3] = 0x22;
//				//Radio.Standby();
//				Radio.Send( TX_Buffer, 4);	

				check_cad();
				_statue = 1;			
				printf("************sxd\n");
			}else if(_statue == 1){
				//printf("1111\n");
			}else if(_statue == 2){
				_idx++;
				//printf("wait next[%d]\n", _idx);
				//if(_idx > 100){
					_statue = 0;
				//}			
			}
			
			
        Radio.IrqProcess( ); // Process Radio IRQ
				lora_node_task();
				//rtc_get_time(&RTC_TimeStructure);
				//printf("xxxxx[%d][%d][%d]\n", RTC_TimeStructure.RTC_Hours, RTC_TimeStructure.RTC_Minutes, RTC_TimeStructure.RTC_Seconds);

    }
}
#endif


void OnTxDone( void )
{   
		printf("[%s][%d]\n", __func__, __LINE__);
    Radio.Standby();
    Radio.Rx( RX_TIMEOUT_VALUE ); //????

}

void OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr )
{
		printf("[%s][%d]\n", __func__, __LINE__);	
    BufferSize = size;
    memcpy( RX_Buffer, payload, BufferSize );
    RssiValue = rssi;
    SnrValue = snr;
    
    Radio.Standby();
	
		printf("rec:");
		MY_PRINTF(payload, size);
		
    
		RTC_TimeTypeDef	  RTC_TimeStructure;
		rtc_get_time(&RTC_TimeStructure);
    if(EnableMaster)
    {
      if(memcmp(RX_Buffer,PongMsg,4)==0)
      {
        LedToggle();//LED??
        
      }
				//biaosi
        TX_Buffer[0] = 0x55; 
        TX_Buffer[1] = 0x07;
			
			
        TX_Buffer[2] = 0x01;
				TX_Buffer[3] = RTC_TimeStructure.RTC_Hours;
				TX_Buffer[4] = RTC_TimeStructure.RTC_Minutes;
				TX_Buffer[5] = RTC_TimeStructure.RTC_Seconds;

        
//        crc_value=RadioComputeCRC(TX_Buffer,6,CRC_TYPE_IBM);//??????????CRC?
//        TX_Buffer[6]=crc_value>>8;
//        TX_Buffer[7]=crc_value;
//        Radio.Send( TX_Buffer, 8);
				Radio.Send( TX_Buffer, 6);
    }
    else
    {
			Radio.Sleep();
			_statue = 2;
			_idx = 0;
//				Delay_Ms(2000);
//        TX_Buffer[0] = 0x55; 
//        TX_Buffer[1] = 0x07;
//        TX_Buffer[2] = 0x02;
//				TX_Buffer[3] = 0x22;
//				Radio.Send( TX_Buffer, 4);			
    }
}

void OnTxTimeout( void )
{
	printf("[%s][%d]\n", __func__, __LINE__);	
	Radio.Standby();	
	Radio.Rx( RX_TIMEOUT_VALUE );   
}

void OnRxTimeout( void )
{
		printf("[%s][%d]\n", __func__, __LINE__);	
    Radio.Standby();
    if(EnableMaster)
    {
			Radio.Rx( RX_TIMEOUT_VALUE ); 
    }
    else
    {
      Radio.Rx( RX_TIMEOUT_VALUE ); 
    }
}

void OnRxError( void )
{
		printf("[%s][%d]\n", __func__, __LINE__);
    Radio.Standby();
    if(EnableMaster)
    {
      Radio.Rx( RX_TIMEOUT_VALUE ); 
    }
    else
    {
      Radio.Rx( RX_TIMEOUT_VALUE ); 
    }
  
}

