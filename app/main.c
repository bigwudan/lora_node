#include <string.h>
#include "stm32f0xx.h"
#include "uart.h"
#include "gpio.h"
void SysClock_48()
{ 

	RCC_DeInit();
	RCC_HSICmd(ENABLE);
	
	while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == 0){
		__NOP;
	
	};
    /* Enable Prefetch Buffer and set Flash Latency */
    FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY; 
    /* HCLK = SYSCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;      
    /* PCLK = HCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE_DIV1;	
	  RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_12);//48M	
		RCC_PLLCmd(ENABLE);	
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == 0){
			__NOP;	
		};
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);		
		while(RCC_GetSYSCLKSource() !=RCC_CFGR_SWS_PLL ){
			__NOP;		
		}

}


static void _Delay_Ms(uint32_t delay )
{
  uint32_t i=0;
  uint32_t j=0;
  
  for(i=0;i<delay;i++)
  {
    for(j=0;j<4540;j++);
  }
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

void HW_int(void)
{

    SysClock_48(); 
    Tick_Configration();
    RCC_Configuration();
		GPIO_int();
		UartInit(9600);
		
}



//用户程序存放地址（也是存放中断向量的flash地址）
#define APPLICATION_ADDRESS ((uint32_t)0x08002000)
 

static void _check_dma(){
		uint16_t data;
	uint8_t d =0;

		//DMA_Cmd(DMA1_Channel3, DISABLE);
		data= 16 - DMA_GetCurrDataCounter(DMA1_Channel3);//获得传输的数据个数
		if(data >0){
			DMA_SetCurrDataCounter(DMA1_Channel3, 16);
			//DMA_Cmd(DMA1_Channel3, ENABLE);
			extern uint8_t rx_buf[16];
			d = rx_buf[0];
			d = rx_buf[1];
			//memset(rx_buf, 0, 16);
		}
}

//设置中断
static void _test_set_intr(){
#if 1	
	//时钟APB2
	RCC->APB2ENR |= 0x01; 
	
	//开启nvic
	NVIC->ISER[0] |= (uint32_t)(1 << 5);

	GPIO_InitTypeDef   GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

	//映射PA0到exit0
	SYSCFG->EXTICR[0] &= ~((uint32_t)(0x0F));
	
	//EXIT上升触发
	EXTI->RTSR |= 0x01;
	EXTI->IMR |= 0x01;
	
#else
	
	  EXTI_InitTypeDef   EXTI_InitStructure;
  GPIO_InitTypeDef   GPIO_InitStructure;
  NVIC_InitTypeDef   NVIC_InitStructure;
  
  /* Enable GPIOA clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

  /* Configure PA4&PA5 pin as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Enable SYSCFG clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  
  /* Connect EXTI4 Line to PA4 pin */
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);

  /* Configure EXTI4&EXTI5 line */
  EXTI_InitStructure.EXTI_Line = EXTI_Line0;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable and set EXTI4&EXTI5 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

#endif	
	
	
	return ;
}

static void _test_io(){

	//开启AHB GPIOA
	RCC->AHBENR |= (uint32_t)(1<<17);
	
	//设置模式
	GPIOA->MODER &= ~(3 << 30);
	GPIOA->MODER |= (1 << 30);
	
	//设置上拉
	GPIOA->PUPDR &= ~(3 << 30);
	GPIOA->PUPDR |= (1 << 30);
	
	//设置输出
	GPIOA->ODR |= (1<<15);
	

	GPIOA->ODR &= ~(uint32_t)(1<<15);
	return ;
}


//测试tim
static void _test_tim3(){

#if 1	
	RCC->APB1ENR |= 0x02;
	//开启nvic
	NVIC->ISER[0] |= (uint32_t)(1 << 16);
	
	//DIR
	TIM3->CR1 |= (uint32_t)(1<<4);
	

	TIM3->CNT = 30000;
	//TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);//使能中断	 TIM2 向上计数 使能这样就可以了	
	TIM3->DIER |= 0x01;
	TIM3->CR1 |= 0x01;	
#else

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);//开启时钟
		
	TIM_InternalClockConfig(TIM3);//选择内部时钟
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseInitStruct;//定义结构体  然后填写参数
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1 ;//选择1分频
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;//计数器模式 这里选择为向上计数
	TIM_TimeBaseInitStruct.TIM_Period = 10000-1;//arr -1是因为有偏差  自动重装载值
	TIM_TimeBaseInitStruct.TIM_Prescaler = 7200-1;//psc  预分频系数  72M分频7200得到10K的
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;//重复计数器高级定时器才有，这里直接给0

	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);//配置实基单元

	TIM_ClearITPendingBit(TIM3,TIM_FLAG_Update);//避免刚初始化就进入中断的问题
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);//使能中断	 TIM2 向上计数 使能这样就可以了
		
	//配置优先级NVIC
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//配置NVIC分组 这里配置为分组2
	NVIC_InitTypeDef NVIC_InitStruct;//配置NVIC的结构体
	NVIC_InitStruct.NVIC_IRQChannel = TIM3_IRQn;//选择定时器2的中断
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;//使能
//	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;//强占优先级
//	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;//响应优先级
		
	NVIC_Init(&NVIC_InitStruct);
	TIM_Cmd(TIM3, ENABLE);//启动定时器

#endif	
	
	return ;
}

 // IO口设置
#define	IIC_GPIOx  GPIOB
#define Pin_SCL		GPIO_Pin_10
#define Pin_SDA		GPIO_Pin_11
 
#define I2C_TIMEOUT       ((uint32_t)0x1000)
#define I2C_LONG_TIMEOUT  ((uint32_t)(10 * I2C_TIMEOUT))
#define Bit16							2
#define Bit8							1 
#define RegAddressBitWide Bit8 // 寄存器位宽	 1:8位    2:16位

__IO uint32_t I2C2Timeout = I2C_LONG_TIMEOUT;


// 初始化IIC2
void I2C2_Hard_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; // 定义GPIO结构体
	I2C_InitTypeDef I2C_InitStruct;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE); // 打开GPIOB口时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE); // 使能I2C2时钟
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; // 复用功能
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD; // 开漏
	GPIO_InitStructure.GPIO_Pin = Pin_SCL | Pin_SDA; // IIC对应IO口
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; // 上拉
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3; // 50MHZ
	GPIO_Init(GPIOB, &GPIO_InitStructure); // 初始化GPIO
	
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_1); // 将 PB10 映射为 I2C_SCL
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_1); // 将 PB11 映射为 I2C_SDA
	
	SYSCFG_I2CFastModePlusConfig(SYSCFG_I2CFastModePlus_I2C1, ENABLE); // 配置I2C1快速模式(1MHz)和驱动能力	
	
	I2C_InitStruct.I2C_Ack = I2C_Ack_Enable; // I2C2应答使能
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; // 7位应答地址
	I2C_InitStruct.I2C_AnalogFilter = I2C_AnalogFilter_Enable; // I2C模拟滤波使能
	I2C_InitStruct.I2C_DigitalFilter = 0x00; // 数字滤波系数
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C; // 工作模式：选择模式为I2C
	I2C_InitStruct.I2C_OwnAddress1 = 0x00; // 设置I2C自身器件地址
	I2C_InitStruct.I2C_Timing = 0x00700818; // 设置I2C时间寄存器的值  1M:0x00700818    400k:0x00901850; 
	I2C_Init(I2C2,&I2C_InitStruct);
	
	I2C_Cmd(I2C2, ENABLE);
}


// 时间溢出返回函数
uint8_t I2C2_TIMEOUT_UserCallback()
{
	return 1;
}
 
// 写多字节函数
uint8_t I2C2_Write_NByte(const uint8_t Slave_Address, const uint16_t REG_Address, const uint8_t* REG_data, const uint8_t len)
{
	I2C2Timeout = I2C_LONG_TIMEOUT; 
	while(I2C_GetFlagStatus(I2C2,I2C_FLAG_BUSY) != RESET) // 设备忙等待
	{
		if((--I2C2Timeout) == 0) return I2C2_TIMEOUT_UserCallback();
	}
	// 配置从机设备地址 发送数据长度 发送模式 发送开始写信号
	I2C_TransferHandling(I2C2, Slave_Address, RegAddressBitWide, I2C_Reload_Mode, I2C_Generate_Start_Write);
	
	I2C2Timeout = I2C_LONG_TIMEOUT; 
	while(I2C_GetFlagStatus(I2C2,I2C_FLAG_TXIS) == RESET) // 等待发送状态复位
	{
		if((--I2C2Timeout) == 0) return I2C2_TIMEOUT_UserCallback();
	}	
	
	// 如果寄存器为16位 发送寄存器高8位地址
	if(RegAddressBitWide == Bit16)
	{
		I2C_SendData(I2C2, REG_Address >> 8); 
		
		I2C2Timeout = I2C_LONG_TIMEOUT; 
		while(I2C_GetFlagStatus(I2C2, I2C_FLAG_TXIS) == RESET) // 等待发送完成
		{
			if((--I2C2Timeout) == 0) return I2C2_TIMEOUT_UserCallback();
		}			
	}
	
  I2C_SendData(I2C2, REG_Address & 0xff); // 发送寄存器地址
	
	I2C2Timeout = I2C_LONG_TIMEOUT; 
	while(I2C_GetFlagStatus(I2C2, I2C_FLAG_TCR) == RESET) // 等待Reload模式下，数据发送完成
	{
		if((--I2C2Timeout) == 0) return I2C2_TIMEOUT_UserCallback();
	}
	// 配置从机设备地址 发送数据长度 发送模式 发送开始写信号
	I2C_TransferHandling(I2C2, Slave_Address, len, I2C_AutoEnd_Mode, I2C_No_StartStop);	
	
	uint8_t length = 0;
	while(length != len)
	{
    I2C2Timeout = I2C_LONG_TIMEOUT;
    while(I2C_GetFlagStatus(I2C2, I2C_ISR_TXIS) == RESET) // 等待发送完成
    {
      if((--I2C2Timeout) == 0) return I2C2_TIMEOUT_UserCallback();
    }  
    
    I2C_SendData(I2C2, (uint8_t)(REG_data[length]));
    
    length++;	
	}
	
  I2C2Timeout = I2C_LONG_TIMEOUT;
  while(I2C_GetFlagStatus(I2C2, I2C_ISR_STOPF) == RESET) // 等待AutoEnd模式下停止信号
  {
    if((--I2C2Timeout) == 0) return I2C2_TIMEOUT_UserCallback();
  }   
  
  I2C_ClearFlag(I2C2, I2C_ICR_STOPCF);
	return 0;	
}


// 读多字节函数
uint8_t I2C2_Read_NByte(const uint8_t Slave_Address, const uint16_t REG_Address, uint8_t *REG_data, const uint8_t len)
{
	I2C2Timeout = I2C_LONG_TIMEOUT; 
	while(I2C_GetFlagStatus(I2C2,I2C_FLAG_BUSY) != RESET) // 设备忙等待
	{
		if((--I2C2Timeout) == 0) return I2C2_TIMEOUT_UserCallback();
	}
 
	// 配置从机设备地址 发送数据长度 发送模式 发送开始写信号
	I2C_TransferHandling(I2C2, Slave_Address, RegAddressBitWide, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);
	
	I2C2Timeout = I2C_LONG_TIMEOUT; 
	while(I2C_GetFlagStatus(I2C2,I2C_FLAG_TXIS) == RESET) // 等待发送状态复位
	{
		if((--I2C2Timeout) == 0) return I2C2_TIMEOUT_UserCallback();
	}
 
	// 如果寄存器为16位 发送寄存器高8位地址
	if(RegAddressBitWide == Bit16)
	{
		I2C_SendData(I2C2, REG_Address >> 8); 
		
		I2C2Timeout = I2C_LONG_TIMEOUT; 
		while(I2C_GetFlagStatus(I2C2,I2C_FLAG_TXIS) == RESET) // 等待发送完成
		{
			if((--I2C2Timeout) == 0) return I2C2_TIMEOUT_UserCallback();
		}			
	}	
	
	I2C_SendData(I2C2, REG_Address & 0xff); // 发送寄存器地址
	
	I2C2Timeout = I2C_LONG_TIMEOUT; 
	while(I2C_GetFlagStatus(I2C2,I2C_ISR_TC) == RESET) // 等待软件停止模式(I2C_SoftEnd_Mode)下，数据发送完成
	{
		if((--I2C2Timeout) == 0) return I2C2_TIMEOUT_UserCallback();
	}	
	
  // 配置从机设备地址 接收数据长度 发送模式 发送开始读信号
	I2C_TransferHandling(I2C2, Slave_Address, len, I2C_AutoEnd_Mode, I2C_Generate_Start_Read);	
	
	uint8_t length = 0;
	while( length != len)
	{
		I2C2Timeout = I2C_LONG_TIMEOUT; 
		while(I2C_GetFlagStatus(I2C2,I2C_ISR_RXNE) == RESET) // 等待接收寄存器非空
		{
			if((--I2C2Timeout) == 0) return I2C2_TIMEOUT_UserCallback();
		}
 
		REG_data[length] = I2C_ReceiveData(I2C2);
		length++;
	}
	
  I2C2Timeout = I2C_LONG_TIMEOUT;
  while(I2C_GetFlagStatus(I2C2, I2C_ISR_STOPF) == RESET) // 等待AutoEnd模式下停止信号
  {
    if((--I2C2Timeout) == 0) return I2C2_TIMEOUT_UserCallback();
  }   
  
  I2C_ClearFlag(I2C2, I2C_ICR_STOPCF);
	return 0;		
}

static void _test_i2c(){
	I2C2_Hard_Init();

	return ;
}


static void _handlesys(){
	RCC_DeInit();
	//启动HSI
	RCC->CR |= 0x01;
	
	//等待HSI启动
	while((RCC->CR & 0x02) == 0){
		__NOP;
	
	}
	
	/* Enable Prefetch Buffer and set Flash Latency */
	FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY; 	
	
	//选择PLLSRC
	//RCC->CFGR |= 0x10000;
	
	//12分频
	//清零mask
	RCC->CFGR &= ~(0x3C0000);
	RCC->CFGR |= 0x280000;
	
	//启动PLL
	RCC->CR |= 0x1000000;
	//等待PLL启动
	while((RCC->CR & 0x2000000) == 0){
		__NOP;
	}
	
	//系统时钟
	RCC->CFGR |= 0x02;
	
	//等待系统时钟转换
	while((RCC->CFGR & 0x08) == 0){
		__NOP;
	}
	
	
#if 0	
	Tick_Configration();
	RCC_Configuration();
	GPIO_int();
	UartInit(9600);
#else
	_test_io();
	_test_set_intr();
	_test_tim3();
#endif	
	return ;
}

int main(void)
{

}


