/*
 * task.c
 *
 *  Created on: Nov 4, 2023
 *      Author: kerem
 */

#include "main.h"

const char *msg_inv = "////Invalid option////\n";

void led_task(void* param){

	uint32_t cmd_addr;
	command_t* cmd;
	const char* msg_led = "========================\n"
			  "|      LED Effect     |\n"
			  "========================\n"
			  "(none,e1,e2,e3,e4)\n"
			  "Enter your choice here : ";


	while(1){
		xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);

		xQueueSend(q_print,&msg_led, portMAX_DELAY);

		xTaskNotifyWait(0, 0, &cmd_addr, portMAX_DELAY);

		cmd = (command_t*) cmd_addr;

		if(cmd->len <= 4){
			if( !strcmp((char*)cmd->payload, "none")){
				led_effect_stop();
			}
			else if( !strcmp((char*)cmd->payload, "e1")){
				led_effect(1);
			}
			else if( !strcmp((char*)cmd->payload, "e2")){
				led_effect(2);
			}
			else if( !strcmp((char*)cmd->payload, "e3")){
				led_effect(3);
			}
			else if( !strcmp((char*)cmd->payload, "e4")){
				led_effect(4);
			}
			else{
				xQueueSend(q_print,&msg_inv, portMAX_DELAY);
			}
		}
		else{
			xQueueSend(q_print,&msg_inv, portMAX_DELAY);
		}

		curr_state = sMainMenu;

		xTaskNotify(defaultTaskHandle, 0, eNoAction);
	}
}


//void rtc_task(void* param){
//
//	while(1){
//
//	}
//}

void print_task(void* param){

	uint32_t* msg;

	while(1){

		xQueueReceive(q_print, &msg, portMAX_DELAY);
		HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen((char*)msg), HAL_MAX_DELAY);

	}
}

void cmd_task(void* param){

	BaseType_t ret;
	command_t cmd;

	while(1){
		ret = xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);

		if(pdTRUE == ret){

			process_command(&cmd);
		}
	}
}

void process_command(command_t* cmd){

	extract_command(cmd);

	switch(curr_state){
	case sMainMenu:

		xTaskNotify(defaultTaskHandle, (uint32_t)cmd, eSetValueWithOverwrite);

		break;
	case sLedEffect:
		xTaskNotify(handle_led_task, (uint32_t)cmd, eSetValueWithOverwrite);
		break;
	case sRtcMenu:
	case sRtcTimeConfig:
	case sRtcDateConfig:
	case sRtcReport:
		xTaskNotify(handle_rtc_task, (uint32_t)cmd, eSetValueWithOverwrite);
		break;
	}

}

int extract_command(command_t* cmd){
	uint8_t item;
	BaseType_t status;

	status = uxQueueMessagesWaiting(q_data);
	if(!status) return -1;

	uint8_t i = 0;

	do{

		status = xQueueReceive(q_data, &item, 0);
		if(pdTRUE == status) cmd->payload[i++] = item;

	}while(item != '\n');

	cmd->payload[i-1] = '\0';
	cmd->len = i-1;
}
