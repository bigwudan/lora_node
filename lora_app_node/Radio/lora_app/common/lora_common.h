#ifndef _LORA_COMMON_H_
#define _LORA_COMMON_H_

#include <stdint.h>


#define WAIT_TIME 60*60 //秒单位

typedef int lora_err_t;

/* Definitions for error constants. */
#define LORA_OK          0       /*!< esp_err_t value indicating success (no error) */
#define LORA_FAIL        -1      /*!< Generic esp_err_t code indicating failure */

typedef enum{
	BEG_FSM, //开始
	
	IDIE_FSM, //空闲
	
	SEND_FSM, //发送开始
	SENDING_FSM, //发送ing
	SEND_END_FSM, //发送结束
	SEND_OVERTIME_FSM, //发送超时
	
	RECV_FSM, //接收开始
	RECVING_FSM, //等待接收
	RECV_END_FSM, //接收结束
	RECV_OVERTIME_FSM, //接收超时
	
	CAD_CHECK_FSM, //CAD检查
	CAD_CHECKING_FSM, //正在检查
	CAD_IDIE_FSM, //空闲
	CAD_BUSY_FSM, //繁忙
	
	AGAIN_FSM, //开始等待
	AGAINING_FSM, //等待中
	AGAIN_END_FSM,//等待结束
	
	NEXT_TASK_FSM, //等到下次
	
	SLEEP_FSM, //休眠
	
	END_FSM

}lora_fsm_state;



//定义lora基础属性
typedef struct{
	lora_fsm_state fsm_state; //状态机状态
	uint32_t time; //当前时间 只计算秒/分/小时
	uint32_t record_time; //间隔时间
}lora_base_data_t;


/*
开始
*/
lora_err_t lora_common_snd_beg();


/*
加入属性值
@param attr 属性值
@param buf 缓存
@param len 长度
return 
*/
lora_err_t lora_common_snd_join(uint8_t attr, uint8_t *buf, uint8_t len);

/*
计算CRC值
@param buf 需要计算的缓存
@param len 长度
return crc
*/
uint8_t lora_common_crc(uint8_t *buf, uint8_t len);

/*
结束发送缓存
*/
lora_err_t lora_common_snd_end();


/*
得到发送的值
@param buf 指针发送的
@param len 长度指针
*/
void lora_common_get_data(uint8_t **buf, uint8_t *len);

void lora_node_task();

void lora_node_init();

void lora_change_state(lora_fsm_state state);
#endif

