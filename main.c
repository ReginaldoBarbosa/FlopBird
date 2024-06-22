#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#include "driver/gpio.h"
#include "esp_system.h"
#include "esp_rom_sys.h"//usa no delay

#include "freertos/FreeRTOS.h"//usa para o milis
#include "freertos/task.h"

#include "esp_random.h"
#include "esp_timer.h"

//Controlador do Placar
//centena está na D7
#define Dezena  GPIO_NUM_32
#define Unidade GPIO_NUM_26

//Control of LCD
#define RS GPIO_NUM_27
#define E  GPIO_NUM_25

//Data bus
#define D0 GPIO_NUM_2
#define D1 GPIO_NUM_4
#define D2 GPIO_NUM_5
#define D3 GPIO_NUM_18
#define D4 GPIO_NUM_19
#define D5 GPIO_NUM_21
#define D6 GPIO_NUM_22
#define D7 GPIO_NUM_23

#define BOT1 GPIO_NUM_33
#define BOT2 GPIO_NUM_34


uint32_t tela = 0;
uint32_t tela_um = 0;
uint16_t tela_dois = 0;
uint32_t bird_pos = 0x8000;
uint32_t pontos = 0;


//Numeros display
void display_num(int i){
	switch(i){
		case 1:
			gpio_set_level(D1, 1);
			gpio_set_level(D2, 1);
			gpio_set_level(D3, 1);
			gpio_set_level(D4, 0);
			gpio_set_level(D5, 1);
			gpio_set_level(D6, 1);
			gpio_set_level(D7, 0);
			break;
		case 2:
			gpio_set_level(D1, 0);
			gpio_set_level(D2, 1);
			gpio_set_level(D3, 0);
			gpio_set_level(D4, 0);
			gpio_set_level(D5, 0);
			gpio_set_level(D6, 0);
			gpio_set_level(D7, 1);
			break;
		case 3:
			gpio_set_level(D1, 0);
			gpio_set_level(D2, 1);
			gpio_set_level(D3, 0);
			gpio_set_level(D4, 0);
			gpio_set_level(D5, 1);
			gpio_set_level(D6, 0);
			gpio_set_level(D7, 0);
			break;
		case 4:
			gpio_set_level(D1, 0);
			gpio_set_level(D2, 0);
			gpio_set_level(D3, 1);
			gpio_set_level(D4, 0);
			gpio_set_level(D5, 1);
			gpio_set_level(D6, 1);
			gpio_set_level(D7, 0);
			break;
		case 5:
			gpio_set_level(D1, 0);
			gpio_set_level(D2, 0);
			gpio_set_level(D3, 0);
			gpio_set_level(D4, 1);
			gpio_set_level(D5, 1);
			gpio_set_level(D6, 0);
			gpio_set_level(D7, 0);
			break;
		case 6:
			gpio_set_level(D1, 0);
			gpio_set_level(D2, 0);
			gpio_set_level(D3, 0);
			gpio_set_level(D4, 0);
			gpio_set_level(D5, 0);
			gpio_set_level(D6, 0);
			gpio_set_level(D7, 0);
			break;
		case 7:
			gpio_set_level(D1, 1);
			gpio_set_level(D2, 1);
			gpio_set_level(D3, 0);
			gpio_set_level(D4, 0);
			gpio_set_level(D5, 1);
			gpio_set_level(D6, 1);
			gpio_set_level(D7, 0);
			break;
		case 8:
			gpio_set_level(D1, 0);
			gpio_set_level(D2, 0);
			gpio_set_level(D3, 0);
			gpio_set_level(D4, 0);
			gpio_set_level(D5, 0);
			gpio_set_level(D6, 0);
			gpio_set_level(D7, 0);
			break;
		case 9:
			gpio_set_level(D1, 0);
			gpio_set_level(D2, 0);
			gpio_set_level(D3, 0);
			gpio_set_level(D4, 0);
			gpio_set_level(D5, 1);
			gpio_set_level(D6, 0);
			gpio_set_level(D7, 0);
			break;
	}
}


//Printa no display
void display_level(int i){

	gpio_set_level(D0, 1);
	display_num((i%1000)/100);
	gpio_set_level(D0, 0);
	gpio_set_level(Dezena, 1);
	display_num((i%100)/10);
	gpio_set_level(Dezena, 0);
	gpio_set_level(Unidade, 1);
	display_num(i%10);
	gpio_set_level(Unidade, 0);

}

//Função para captar o tempo passado desde o inicio do código
uint64_t millis(void) {
	return esp_timer_get_time() / 1000;
}

//Set do modo dos botões
void botoes(void){
	gpio_set_direction(BOT1, GPIO_MODE_INPUT);
	gpio_set_direction(BOT2, GPIO_MODE_INPUT);
}

//Set do modo dos gpio
void lcd_gpio_modes(void){
	//Configure GPIO of Control
	gpio_set_direction(RS, GPIO_MODE_OUTPUT);
	gpio_set_direction(E, GPIO_MODE_OUTPUT);

	//Configure GPIO of Data Bus
	gpio_set_direction(D0, GPIO_MODE_OUTPUT);
	gpio_set_direction(D1, GPIO_MODE_OUTPUT);
	gpio_set_direction(D2, GPIO_MODE_OUTPUT);
	gpio_set_direction(D3, GPIO_MODE_OUTPUT);
	gpio_set_direction(D4, GPIO_MODE_OUTPUT);
	gpio_set_direction(D5, GPIO_MODE_OUTPUT);
	gpio_set_direction(D6, GPIO_MODE_OUTPUT);
	gpio_set_direction(D7, GPIO_MODE_OUTPUT);

	//Controlador do placar
	//centena é o d07
	gpio_set_direction(Dezena, GPIO_MODE_OUTPUT);
	gpio_set_direction(Unidade, GPIO_MODE_OUTPUT);

}

//Cria um delay de 2ms
void delay(void){
	esp_rom_delay_us(2000);
}

//seta os valores de D7 D6 D5 D4 D3 D2 D1 D0
void lcd_bits(int8_t bits){
	gpio_set_level(D0, (bits>>0) & 1);
	gpio_set_level(D1, (bits>>1) & 1);
	gpio_set_level(D2, (bits>>2) & 1);
	gpio_set_level(D3, (bits>>3) & 1);
	gpio_set_level(D4, (bits>>4) & 1);
	gpio_set_level(D5, (bits>>5) & 1);
	gpio_set_level(D6, (bits>>6) & 1);
	gpio_set_level(D7, (bits>>7) & 1);
}

//Pulso de acionameno do Enable
void lcd_enable(void){
	gpio_set_level(E, 0);
	delay();
	gpio_set_level(E, 1);
	delay();
	gpio_set_level(E, 0);
	delay();
}

//Limpa o display
void lcd_clean(void){
	lcd_bits(0b00000001);
	lcd_enable();
}

//Seleciona o modo 8bits (0011), 2 linhas e 5x8 pontos(1000)
void lcd_mode(void){
	lcd_bits(0b00111000);
	lcd_enable();
}

//Habilita o cursor sem blink 1DCB D=Set display	C=cursor	B= blink On / Off
void lcd_display(void){
	lcd_bits(0b00001100);
	lcd_enable();
}

void lcd_init(void){

//Inicia o LCD
	lcd_bits(0b00000000);
	delay();

//Limpa a entrada
	lcd_clean();

//Seleciona o modo 8bits (0011), 2 linhas e 5x8 pontos(1000)
	lcd_mode();

//Habilita o cursor sem blink
	lcd_display();

//Habilita incremento direita p esquerda, desabilita scroll
	lcd_bits(0b00000110);
	lcd_enable();

}

//Envia comandos para o lcd
void lcd_send_comand(void){
	gpio_set_level(RS, 0);
	lcd_enable();
}

//Envia o char para o lcd
void lcd_send_char(void){

	gpio_set_level(RS, 1);
	lcd_enable();
}

//Enviar o cursor para a linha 2
void lcd_line_two(void){
	gpio_set_level(RS,0);
	lcd_bits(0b11000000);
	lcd_enable();
}

//Enviar o cursor para a linha 1
void lcd_line_one(void){
	gpio_set_level(RS,0);
	lcd_bits(0b10000000);
	lcd_enable();
}

//Cria os desenhos do avião e da torre
void lcd_cria_bird_e_torre(void){
	lcd_bits(0b01000000);
	lcd_send_comand();

	lcd_bits(0b00000000);
	lcd_send_char();
	lcd_bits(0b00000000);
	lcd_send_char();
	lcd_bits(0b00000100);
	lcd_send_char();
	lcd_bits(0b00001111);
	lcd_send_char();
	lcd_bits(0b00011110);
	lcd_send_char();
	lcd_bits(0b00000100);
	lcd_send_char();
	lcd_bits(0b00000000);
	lcd_send_char();
	lcd_bits(0b00000000);
	lcd_send_char();

	lcd_bits(0b00001110);
	lcd_send_char();
	lcd_bits(0b00001110);
	lcd_send_char();
	lcd_bits(0b00001110);
	lcd_send_char();
	lcd_bits(0b00011111);
	lcd_send_char();
	lcd_bits(0b00011111);
	lcd_send_char();
	lcd_bits(0b00011111);
	lcd_send_char();
	lcd_bits(0b00000000);
	lcd_send_char();
	lcd_bits(0b00000000);
	lcd_send_char();

	lcd_bits(0b00000000);
	lcd_send_char();
	lcd_bits(0b00000000);
	lcd_send_char();
	lcd_bits(0b00011111);
	lcd_send_char();
	lcd_bits(0b00011111);
	lcd_send_char();
	lcd_bits(0b00011111);
	lcd_send_char();
	lcd_bits(0b00001110);
	lcd_send_char();
	lcd_bits(0b00001110);
	lcd_send_char();
	lcd_bits(0b00001110);
	lcd_send_char();

	lcd_bits(0b10000000);
	lcd_send_comand();

}

//Plota o desenho do bird
void lcd_print_bird(void){
	lcd_bits(0b00000000);
	lcd_send_char();
}

//Plota o desenho da torre 1
void lcd_print_torre_um(void){
	lcd_bits(0b00000001);
	lcd_send_char();
}

//Plota o desenho da torre 2
void lcd_print_torre_dois(void){
	lcd_bits(0b00000010);
	lcd_send_char();
}

//Plota um espaço vazio
void lcd_print_vazio(void){
	lcd_bits(0b00100000);
	lcd_send_char();
}

//Update o lcd
void lcd_update(void){
	while(1){
		lcd_line_one();
		for(int i = 31; i>=0; i--){
			if(i==15)lcd_line_two();
			if((bird_pos == 0x80000000) && (i==31)){
				lcd_print_bird();
			}
			else if ((bird_pos == 0x8000)&& (i==15)){
					lcd_print_bird();
				 }
				 else if((tela>>i) & 1){
						if(i>=16) lcd_print_torre_um();
						else lcd_print_torre_dois();
					  }
					  else lcd_print_vazio();
		}
		display_level(pontos);
	}
}

//posição do passaro
void get_bird_pos(void){
	if(gpio_get_level(BOT1) && !gpio_get_level(BOT2)){
		bird_pos = 0x80000000;
		tela |= bird_pos;
	}
	if(gpio_get_level(BOT2) && !gpio_get_level(BOT1)){
		bird_pos = 0x8000;
		tela |= bird_pos;
	}
}

void game_over(){
	lcd_clean();
	esp_rom_delay_us(800);

	lcd_line_one();
	lcd_bits(0b00100000);
	lcd_send_char();
	lcd_bits(0b00100000);
	lcd_send_char();
	lcd_bits(0b00100000);
	lcd_send_char();
	lcd_bits(0b00100000);
	lcd_send_char();
	lcd_bits(0b00100000);
	lcd_send_char();
	lcd_bits(0b00100000);
	lcd_send_char();

	lcd_bits(0b01000111);
	lcd_send_char();

	lcd_bits(0b01000001);
	lcd_send_char();

	lcd_bits(0b01001101);
	lcd_send_char();

	lcd_bits(0b01000101);
	lcd_send_char();

	lcd_bits(0b00100000);
	lcd_send_char();
	lcd_bits(0b00100000);
	lcd_send_char();
	lcd_bits(0b00100000);
	lcd_send_char();
	lcd_bits(0b00100000);
	lcd_send_char();
	lcd_bits(0b00100000);
	lcd_send_char();
	lcd_bits(0b00100000);
	lcd_send_char();

	lcd_line_two();
	lcd_bits(0b00100000);
	lcd_send_char();
	lcd_bits(0b00100000);
	lcd_send_char();
	lcd_bits(0b00100000);
	lcd_send_char();
	lcd_bits(0b00100000);
	lcd_send_char();
	lcd_bits(0b00100000);
	lcd_send_char();
	lcd_bits(0b00100000);
	lcd_send_char();

	lcd_bits(0b01001111);
	lcd_send_char();

	lcd_bits(0b01010110);
	lcd_send_char();

	lcd_bits(0b01000101);
	lcd_send_char();

	lcd_bits(0b01010010);
	lcd_send_char();

	lcd_bits(0b00100000);
	lcd_send_char();
	lcd_bits(0b00100000);
	lcd_send_char();
	lcd_bits(0b00100000);
	lcd_send_char();
	lcd_bits(0b00100000);
	lcd_send_char();
	lcd_bits(0b00100000);
	lcd_send_char();
	lcd_bits(0b00100000);
	lcd_send_char();

	esp_rom_delay_us(800);
}

//O jogo roda aqui
void jogo(void){
	uint32_t current_time = 0;
	int flag = 0;
	int acelera = 0;
	while(1){
		get_bird_pos();
		if((millis() - current_time)>1000){
			acelera++;
			pontos++;
		}
		if(((bird_pos & (tela_um<<16))>0) || ((bird_pos & tela_dois)>0)){
			game_over();
			break;
		}

		else if((millis() - current_time)>(700 - acelera)){
			tela_um = tela_um<<1;
			tela_dois = tela_dois<<1;
			if((esp_random()%2)&&(flag <= 0)){
				esp_rom_delay_us(5);
				if(esp_random()%2){
					tela_um |= 1;
					flag = 2;
				}
				else{
					tela_dois |= 1;
					flag = 2;
				}
			}
			else flag--;
			tela = 0;
			tela = bird_pos;
			tela = (tela_um<<16) + tela_dois;
			current_time = millis();
		}
	}
}

void app_main(void){
	lcd_gpio_modes();
	botoes();
	lcd_init();
	lcd_cria_bird_e_torre();


	xTaskCreate(&jogo,"jogo", 4096, NULL,5,NULL);
	xTaskCreate(&lcd_update,"lcd_update", 4096, NULL,1,NULL);
}

