#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "lora_common.h"
#include "lora_node.h"

#include "user.h"
#include "sx126x-board.h"
#include "uart.h"
#include "rtc.h"

#define LORA_NODE_RX_TIMEOUT_VALUE                            1000


#define MAX_RECV_NUM 24

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

#define LORA_NODE_ADDR    0x2 //0x1


extern bool IrqFired;

static lora_base_data_t _base_info;
static RadioEvents_t _RadioEvents;

static uint8_t _recv_buf[MAX_RECV_NUM]; //缓存
static uint8_t _recv_len=0; //长度


void _OnTxDone( void );
void _OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr );
void _OnTxTimeout( void );
void _OnRxTimeout( void );
void _OnRxError( void );
void _check_cad();
void _OnCadDone  ( bool channelActivityDetected );

void lora_node_init(){
	memset(&_base_info, 0, sizeof(lora_base_data_t));
	_base_info.fsm_state = IDIE_FSM;

	// Radio initialization
	_RadioEvents.TxDone = _OnTxDone;
	_RadioEvents.RxDone = _OnRxDone;
	_RadioEvents.TxTimeout = _OnTxTimeout;
	_RadioEvents.RxTimeout = _OnRxTimeout;
	_RadioEvents.RxError = _OnRxError;
	_RadioEvents.CadDone = _OnCadDone;	
	
	Radio.Init( &_RadioEvents );
	Radio.SetChannel( RF_FREQUENCY );	
	
	Radio.SetTxConfig( MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
																 LORA_SPREADING_FACTOR, LORA_CODINGRATE,
																 LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
																 true, 0, 0, LORA_IQ_INVERSION_ON, 3000 );
	
	Radio.SetRxConfig( MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
																 LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
																 LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
																 0, true, 0, 0, LORA_IQ_INVERSION_ON, false );	
	printf("config[%d][%d][%d][%d][%d][%d][%d][%d]\n",
	MODEM_LORA,
	TX_OUTPUT_POWER, 
	LORA_BANDWIDTH, 
	LORA_SPREADING_FACTOR ,
	LORA_CODINGRATE,
	LORA_PREAMBLE_LENGTH ,
	LORA_FIX_LENGTH_PAYLOAD_ON,
	LORA_IQ_INVERSION_ON);
	return ;
}

void lora_change_state(lora_fsm_state state){
	_base_info.fsm_state = state;
	return ;
}

static void _send_data(){
	uint8_t buf[] = {LORA_NODE_ADDR};
	uint8_t *snd_buf = NULL;
	uint8_t len = 0;
	lora_common_snd_beg();
	lora_common_snd_join(0x00, buf, sizeof(buf));
	lora_common_snd_end();
	lora_common_get_data(&snd_buf, &len);
	
	printf("send:");
	for(int i=0; i< len; i++){
		printf("[%02X]", snd_buf[i]);
	}
	printf("\n");
	
	Radio.Send( snd_buf, len);
}


static lora_err_t _idie_fsm_func(){

	lora_change_state(CAD_CHECK_FSM);
	return LORA_OK;
}

static lora_err_t _send_fsm_func(){
	_send_data();
	lora_change_state(SENDING_FSM);
	return LORA_OK;
}

static lora_err_t _sending_fsm_func(){

	return LORA_OK;
}


static lora_err_t _send_end_fsm_func(){
	//开始接收数据
	
	lora_change_state(RECV_FSM);
	return LORA_OK;
}




static lora_err_t _send_overtime_fsm_func(){

	return LORA_OK;
}

//开始接收程序
static lora_err_t _recv_fsm_func(){
	Radio.Rx(LORA_NODE_RX_TIMEOUT_VALUE);
	lora_change_state(RECVING_FSM);
	return LORA_OK;
}

static lora_err_t _recving_fsm_func(){

	return LORA_OK;
}

static lora_err_t _recv_end_fsm_func(){
	printf("recv_end\n");
	for(int i=0; i<_recv_len;i++){
		printf("[%02X]", _recv_buf[i]);
	}
	printf("\n");
	if(_recv_buf[3] == LORA_NODE_ADDR){
		printf("join SLEEP_FSM\n");
		lora_change_state(SLEEP_FSM);
	}else{
		printf("join CAD_CHECK_FSM\n");
		lora_change_state(CAD_CHECK_FSM);
	}
	

	return LORA_OK;
}

static lora_err_t _recv_overtime_fsm_func(){

	return LORA_OK;
}

static lora_err_t _sleep_fsm_func(){
	printf("[%s][%d]\n", __func__, __LINE__);	
	Delay_Ms(1000*60);
	lora_change_state(CAD_CHECK_FSM);
	return LORA_OK;
}


static lora_err_t _cad_check_fsm_func(){
	_check_cad();
	lora_change_state(CAD_CHECKING_FSM);
	return LORA_OK;
}


static lora_err_t _cad_idie_fsm_func(){
	//发送
	lora_change_state(SEND_FSM);
	return LORA_OK;
}


static lora_err_t _cad_busy_fsm_func(){
	printf("[%s][%d]\n", __func__, __LINE__);	
	//等待下次重新
	lora_change_state(AGAIN_FSM);
	return LORA_OK;
}


static lora_err_t _again_fsm_func(){
	//等待下次重新
	//lora_change_state(AGAINING_FSM);
	//printf("[%s][%d]\n", __func__, __LINE__);
	lora_change_state(CAD_CHECK_FSM);
	return LORA_OK;
}

static lora_err_t _againing_fsm_func(){
	Delay_Ms(5000);
	lora_change_state(AGAIN_END_FSM);
	return LORA_OK;
}

static lora_err_t _again_end_fsm_func(){
	//重新检查cad
	lora_change_state(CAD_CHECK_FSM);
	return LORA_OK;
}

//状态机任务
void lora_node_task(){
	if(_base_info.fsm_state == IDIE_FSM){
		_idie_fsm_func();
	}else if(_base_info.fsm_state == SEND_FSM){
		_send_fsm_func();
	}else if(_base_info.fsm_state == SENDING_FSM){
		_sending_fsm_func();
	}else if(_base_info.fsm_state == SEND_END_FSM){
		_send_end_fsm_func();
	}else if(_base_info.fsm_state == SEND_OVERTIME_FSM){
		_send_overtime_fsm_func();
	}else if(_base_info.fsm_state == RECV_FSM){
		_recv_fsm_func();
	}else if(_base_info.fsm_state == RECVING_FSM){
		_recving_fsm_func();
	}else if(_base_info.fsm_state == RECV_END_FSM){
		_recv_end_fsm_func();
	}else if(_base_info.fsm_state == RECV_OVERTIME_FSM){
		_recv_overtime_fsm_func();
	}else if(_base_info.fsm_state == SLEEP_FSM){
		_sleep_fsm_func();
	}else if(_base_info.fsm_state == CAD_CHECK_FSM){
		_cad_check_fsm_func();
	}else if(_base_info.fsm_state == CAD_IDIE_FSM){
		_cad_idie_fsm_func();
	}else if(_base_info.fsm_state == CAD_BUSY_FSM){
		_cad_busy_fsm_func();
	}else if(_base_info.fsm_state == AGAIN_FSM){
		_again_fsm_func();
	}


	return ;
}




//set cad 
static void _check_cad(){
	printf("check_cad\n");
	SX126xSetDioIrqParams( IRQ_RADIO_ALL,IRQ_RADIO_ALL,IRQ_RADIO_NONE,IRQ_RADIO_NONE );
	//SX126xSetCadParams(LORA_CAD_02_SYMBOL, 22, 10, LORA_CAD_ONLY, 0);
	SX126xSetCadParams(LORA_CAD_16_SYMBOL, 32, 20, LORA_CAD_ONLY, 0);
	SX126xSetCad();
	return ;
}

//CAD检查回调
static void _OnCadDone  ( bool channelActivityDetected ){
	printf("caddone[%d]\n", channelActivityDetected);
	if(channelActivityDetected == 1){
		lora_change_state(CAD_BUSY_FSM);
	}else{
		lora_change_state(CAD_IDIE_FSM);
	}
	return ;
}

//接收完成回调
static void _OnTxDone( void )
{
	printf("[%s][%d]\n", __func__, __LINE__);
	//准备接收
	lora_change_state(RECV_FSM);
	return ;
}

//读完成回调
static void _OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr )
{
	printf("[%s][%d]\n", __func__, __LINE__);
	memcpy(_recv_buf, payload, size);
	_recv_len = size;
	lora_change_state(RECV_END_FSM);
	return ;
}

//发送超时
static void _OnTxTimeout( void )
{
	//继续检查
  lora_change_state(CAD_CHECK_FSM);
	return ;
}

//读超时
static void _OnRxTimeout( void )
{
	printf("[%s][%d]\n", __func__, __LINE__);
	//未收到发送,等待
	lora_change_state(AGAIN_FSM);
	return ;
}

//错误回调
static void _OnRxError( void )
{
	printf("[%s][%d]\n", __func__, __LINE__);	
	lora_change_state(AGAIN_FSM);
	return ;
}

