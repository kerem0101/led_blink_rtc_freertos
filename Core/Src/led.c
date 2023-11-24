/*
 * led.c
 *
 *  Created on: 21 Kas 2023
 *      Author: kerem
 */

#include "main.h"

void led_effect_stop(void){

}

void led_effect(int8_t num){

	TickType_t xDelay;

	if(num == 1){
		xDelay = 250 / portTICK_PERIOD_MS;
	}else if(num == 2){
		xDelay = 500 / portTICK_PERIOD_MS;
	}else if(num == 3){
		xDelay = 1000 / portTICK_PERIOD_MS;
	}else{
		xDelay = 2000 / portTICK_PERIOD_MS;
	}



	for(int8_t i = 0; i < 10; ++i){
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
		vTaskDelay(xDelay);
	}
}

