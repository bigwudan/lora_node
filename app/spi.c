#include "stm32f0xx.h"

void SPI2_Int()
{
    SPI_InitTypeDef  SPI_InitStruct;
    
    SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//ȫ˫��ģʽ
    SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
    SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;//��һ������
    SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;//�����ز���
    SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16; // 6MHz
    SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStruct.SPI_CRCPolynomial = 7;
    SPI_Init(SPI2,&SPI_InitStruct);
    
    SPI_RxFIFOThresholdConfig(SPI2, SPI_RxFIFOThreshold_QF);
    SPI_Cmd(SPI2, ENABLE );
}

/*!
 * @brief Sends txBuffer and receives rxBuffer
 *
 * @param [IN] txBuffer Byte to be sent
 * @param [OUT] rxBuffer Byte to be sent
 * @param [IN] size Byte to be sent
 */
uint8_t SpiInOut( uint8_t txBuffer)
{
    
      while( SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);//������bufferΪ��ʱ(˵����һ�������Ѹ��Ƶ���λ�Ĵ�����)�˳�,��ʱ������buffer����д����
      SPI_SendData8(SPI2, txBuffer);
    
      while( SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);//������bufferΪ�ǿ�ʱ�˳�
      return SPI_ReceiveData8(SPI2);
      
   
}

void SpiIn( uint8_t *txBuffer, uint16_t size )
{
    uint16_t i;
    
    for(i=0;i<size;i++)
    {
      while( SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);//������bufferΪ��ʱ(˵����һ�������Ѹ��Ƶ���λ�Ĵ�����)�˳�,��ʱ������buffer����д����
      SPI_SendData8(SPI2, txBuffer[i]);
      
      while( SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);//������bufferΪ�ǿ�ʱ�˳�
      SPI_ReceiveData8(SPI2);
    }
}


