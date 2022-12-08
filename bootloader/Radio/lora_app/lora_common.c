#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "lora_common.h"
#include "rtc.h"

#define MAX_SEND_NUM 24

//[head][len][attr][data][crc]


typedef struct {
	uint8_t idx; //当前索引
	uint8_t buf[MAX_SEND_NUM]; //发送缓存

}lora_send_data_tag;

static lora_send_data_tag _send_data_info;

/*
开始
*/
lora_err_t lora_common_snd_beg(){
#define HEAD 0x9F	
	
	memset(&_send_data_info, 0, sizeof(lora_send_data_tag));
	_send_data_info.buf[_send_data_info.idx++] = HEAD;
	_send_data_info.buf[_send_data_info.idx++] = 0x00;
	return LORA_OK;
}


/*
加入属性值
@param attr 属性值
@param buf 缓存
@param len 长度
return 
*/
lora_err_t lora_common_snd_join(uint8_t attr, uint8_t *buf, uint8_t len){
	if(_send_data_info.idx + len  + 1> MAX_SEND_NUM || buf == NULL || len == 0) return LORA_FAIL;	
	_send_data_info.buf[_send_data_info.idx++] = attr;	
	memcpy(_send_data_info.buf +_send_data_info.idx, buf, len );
	_send_data_info.idx += len;
	return LORA_OK;
}

/*
计算CRC值
@param buf 需要计算的缓存
@param len 长度
return crc
*/
uint8_t lora_common_crc(uint8_t *buf, uint8_t len){
	uint8_t crc = 0;
	for(int i=0; i< len; i++){
		crc = (uint8_t)((crc + buf[i])&0xff);
	}
	return crc;
}

/*
结束发送缓存
*/
lora_err_t lora_common_snd_end(){
	if(_send_data_info.idx == 2) return LORA_FAIL;
	uint8_t crc = 0;
	//计算长度
	_send_data_info.buf[1] = _send_data_info.idx;
	//_send_data_info.idx++;
	
	crc = lora_common_crc(_send_data_info.buf, _send_data_info.idx);
	_send_data_info.buf[_send_data_info.idx++] = crc;
	return LORA_OK;
}

/*
得到发送的值
@param buf 指针发送的
@param len 长度指针
*/
void lora_common_get_data(uint8_t **buf, uint8_t *len){
	*buf = _send_data_info.buf;
	*len = _send_data_info.idx;
	return ;
}



/*
延迟秒
*/
void lora_common_delay_s(int num){
	uint32_t count  = 0;
	count = rtc_get_sec();
	count = count + num;
	while(1){
		if(rtc_get_sec() > count ){
			break;
		}
	}
	return ;
}

