/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define cursor_off 0x0C
#define cursor_on 0x0E
#define cursor_blink 0x0F

#define E_0 GPIOC->BRR = (1<<7)
#define E_1 GPIOC->BSRR = (1<<7)

#define RS_0 GPIOA->BRR = (1<<9)
#define RS_1 GPIOA->BSRR = (1<<9)

#define D7_0 GPIOA->BRR = (1<<8)
#define D7_1 GPIOA->BSRR = (1<<8)
#define D6_0 GPIOB->BRR = (1<<14)
#define D6_1 GPIOB->BSRR = (1<<14)
#define D5_0 GPIOB->BRR = (1<<4)
#define D5_1 GPIOB->BSRR = (1<<4)
#define D4_0 GPIOB->BRR = (1<<5)
#define D4_1 GPIOB->BSRR = (1<<5)

#define scl_0 GPIOC->BRR = 1 << 12
#define scl_1 GPIOC->BSRR = 1 << 12
#define sda_0 GPIOC->BRR = 1 << 10
#define sda_1 GPIOC->BSRR = 1 << 10
#define sda_in (GPIOC->IDR & (1<<10))
#define i2c_ack 0
#define i2c_nak 255

#define data_0 GPIOB->BRR = 1 << 6
#define data_1 GPIOB->BSRR = 1 << 6
#define sck_0 GPIOB->BRR = 1 << 7
#define sck_1 GPIOB->BSRR = 1 << 7
#define data_in (GPIOB->IDR & (1 << 6))

#define RW_porta_casa      ((GPIOC->IDR >> 0) & 1)
#define RW_janela_sala     ((GPIOC->IDR >> 1) & 1)
#define RW_janela_quarto   ((GPIOC->IDR >> 2) & 1)

//SPI
// SCLK -> PA10
#define clk_1      GPIOA->BSRR = (1 << 10)
#define clk_0      GPIOA->BRR  = (1 << 10)

// CS -> PC4
#define cs_1       GPIOC->BSRR = (1 << 4)
#define cs_0       GPIOC->BRR  = (1 << 4)

// MOSI (SDIN) -> PC5
#define don_1      GPIOC->BSRR = (1 << 5)
#define don_0      GPIOC->BRR  = (1 << 5)

// D/C -> PB0
#define dc_1       GPIOC->BSRR = (1 << 11)
#define dc_0       GPIOC->BRR  = (1 << 11)

// RESET -> PB1
#define rst_1      GPIOB->BSRR = (1 << 1)
#define rst_0      GPIOB->BRR  = (1 << 1)

#define SERVO_ON_PORTA GPIOB->BSRR = (1<<9)
#define SERVO_OFF_PORTA GPIOB->BRR = (1<<9)

#define SERVO_ON_JANELA_SALA GPIOB->BSRR = (1<<8)
#define SERVO_OFF_JANELA_SALA GPIOB->BRR = (1<<8)

#define SERVO_ON_JANELA_QUARTO GPIOB->BSRR = (1<<3)
#define SERVO_OFF_JANELA_QUARTO GPIOB->BRR = (1<<3)

#define buzzer_0 GPIOC->BRR = 1 << 8
#define buzzer_1 GPIOC->BSRR = 1 << 8

#define movInp (GPIOB->IDR & (1 << 12))
#define LEDMov_0 GPIOB->BRR = (1 << 11)
#define LEDMov_1 GPIOB->BSRR = (1 << 11)

#define OLED_W 96
#define OLED_H 64

#define lcd_bk      GPIOB->BSRR = (1 << 0)

static uint16_t oled_fb[OLED_W * OLED_H];

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

RTC_HandleTypeDef hrtc;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
uint32_t Tsenha = 0;
uint8_t bloqueado = 1;
uint8_t serial = 0;
uint8_t ledQuarto = 0;
uint8_t draw = 0;
int countSenha = 0;

uint8_t portaAberta = 0, portaAbertaReed = 1;
uint8_t janelaSalaAberta = 0, janelaSalaAbertaReed = 1;
uint8_t janelaQuartoAberta = 0, janelaQuartoAbertaReed = 1;

ADC_ChannelConfTypeDef configuraAD;

const uint8_t cadeadoAberto[8] = { 0b00001110,
								   0b00000001,
								   0b00000001,
								   0b00011111,
								   0b00011011,
								   0b00011111,
								   0b00001110,
								   0b00000000};

const uint8_t cadeadoFechado[8] = { 0b00001110,
									0b00010001,
									0b00010001,
									0b00011111,
									0b00011011,
									0b00011111,
									0b00001110,
									0b00000000};

const uint8_t gota[8] = 		  { 0b00000100,
									0b00000100,
									0b00001110,
									0b00011111,
									0b00011111,
									0b00011111,
									0b00001110,
									0b00000000};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_ADC1_Init(void);
static void MX_RTC_Init(void);
/* USER CODE BEGIN PFP */
void udelay(void);
void delayus(int);
void lcd_wrcom4(uint8_t);
void lcd_wrcom(uint8_t);
void lcd_wrchar(char);
void lcd_init(uint8_t);
void goTo(uint8_t,uint8_t);
void clear(void);
void lcd_progchar(void);
char getKeyPad(void);

void i2c_init(void);
void i2c_start(void);
void i2c_stop(void);
uint8_t i2c_write(uint8_t dado);
uint8_t i2c_read(uint8_t ack_state);
void i2c_wrmem(uint8_t slave_addr, uint16_t addr, uint8_t addr_size, uint8_t data);
uint8_t i2c_rdmem(uint8_t slave_addr, uint16_t addr, uint8_t addr_size);

void sht_init(void);
void sht_pulse(void);
void sht_start(void);
void sht_stop(void);
void sht_wrCmd(uint8_t data);
uint8_t sht_rdData(uint8_t confirm);
float sht_rdHumid(void);
void sht_reSynch(void);

void put_password(char *senha);
int verify_password(char *senha);
void getMov(void);
void buzzerTrava(void);
void buzzerAlarme(void);
void checkLDR(void);
float checkMQ7(void);
void getUart(uint8_t *ans);
void servos(void);

void spi_init(void);
void spi_clock(void);
void oled_write(uint8_t data);
void oled_cmd(uint8_t cmd);
void oled_data(uint8_t data);
void oled_init(void);

void oled_putpixel(uint8_t x, uint8_t y, uint16_t color);
void oled_fill_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t color);
void oled_pixel2x(uint8_t x, uint8_t y, uint16_t color);
void oled_set_window(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
void oled_update(void);
void oled_draw_char(uint8_t x, uint8_t y, char c, uint16_t color);
void oled_draw_icon(uint8_t x, uint8_t y, const uint8_t icon[8][8], uint16_t color);
void oled_draw_background(void);
void oled_clear_locked_buffer(void);
void oled_clear_opened_buffer(void);
void oled_update_locked_area(void);
void oled_update_opened_area(void);
void oled_draw_locked(void);
void oled_draw_opened(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int __io_putchar(int ch){
	if(serial) HAL_UART_Transmit(&huart2,(uint8_t*)&ch,1,10);
	else if(ch != '\n') lcd_wrchar(ch);
	//oled_draw_gas_value(10);
	return ch;
}

void udelay(void){
	int tempo = 7;
	while(tempo--);
}

void delayus(int tempo){
	while(tempo--)udelay();
}

void send_data(uint8_t data){
	if(data & (1 << 0))D4_1; else D4_0;
	if(data & (1 << 1))D5_1; else D5_0;
	if(data & (1 << 2))D6_1; else D6_0;
	if(data & (1 << 3))D7_1; else D7_0;
}

void lcd_wrcom4(uint8_t com4){
	send_data(com4);
	RS_0;
	E_1;
	delayus(5);
	E_0;
	HAL_Delay(5);
}

void lcd_wrcom(uint8_t com4){
	send_data(com4 >> 4);
	RS_0;
	E_1;
	delayus(5);
	E_0;
	delayus(5);
	send_data(com4);
	E_1;
	delayus(5);
	E_0;
	HAL_Delay(5);
}

void lcd_wrchar(char ch){
	send_data(ch >> 4);
	RS_1;
	E_1;
	delayus(5);
	E_0;
	delayus(5);
	send_data(ch);
	RS_1;
	E_1;
	delayus(5);
	E_0;
	HAL_Delay(1);
}

void lcd_init(uint8_t cursor){
	lcd_wrcom4(3);
	lcd_wrcom4(3);
	lcd_wrcom4(3);
	lcd_wrcom4(2);
	lcd_wrcom(0x28);
	lcd_wrcom(cursor);
	lcd_wrcom(0x06);
	lcd_wrcom(0x01);
}

void goTo(uint8_t x, uint8_t y){
	uint8_t com;
	if(y == 0) com = 0x80+x;
	if(y == 1) com = 0xC0+x;
	lcd_wrcom(com);
}

void clear(){
	goTo(0,0);
	printf("                \n");
	goTo(0,1);
	printf("                \n");
}

void lcd_progchar(void){
	uint8_t n;
	lcd_wrcom(0x40);
	for(n = 0; n < 8; n++) lcd_wrchar(gota[n]);
	lcd_wrcom(0x80);
}

char getKeyPad(void){
	char ch = 0;

	GPIOD->BRR = 1 << 0;
	if((GPIOD->IDR & (1 << 4)) == 0) ch = '1';
	else if((GPIOD->IDR & (1 << 5)) == 0) ch = '2';
	else if((GPIOD->IDR & (1 << 6)) == 0) ch = '3';
	GPIOD->BSRR = 1 << 0;

	if(ch != 0) return ch;

	GPIOD->BRR = 1 << 1;
	if((GPIOD->IDR & (1 << 4)) == 0) ch = '4';
	else if((GPIOD->IDR & (1 << 5)) == 0) ch = '5';
	else if((GPIOD->IDR & (1 << 6)) == 0) ch = '6';
	GPIOD->BSRR = 1 << 1;

	if(ch != 0) return ch;

	GPIOD->BRR = 1 << 2;
	if((GPIOD->IDR & (1 << 4)) == 0) ch = '7';
	else if((GPIOD->IDR & (1 << 5)) == 0) ch = '8';
	else if((GPIOD->IDR & (1 << 6)) == 0) ch = '9';
	GPIOD->BSRR = 1 << 2;

	if(ch != 0) return ch;

	GPIOD->BRR = 1 << 3;
	if((GPIOD->IDR & (1 << 4)) == 0) ch = '*';
	else if((GPIOD->IDR & (1 << 5)) == 0) ch = '0';
	else if((GPIOD->IDR & (1 << 6)) == 0) ch = '#';
	GPIOD->BSRR = 1 << 3;

	return ch;
}

void i2c_init(void){
	sda_1;
	scl_1;
	delayus(50);
}

void i2c_start(void){
	sda_0;
	delayus(5);
	scl_0;
	delayus(1);
}

void i2c_stop(void){
	sda_0;
	delayus(1);
	scl_1;
	delayus(5);
	sda_1;
	delayus(5);
}

uint8_t i2c_write(uint8_t dado){
	int8_t i;
	uint8_t resp;
	for (i = 7; i >= 0; i--){
		if ((dado & (1 << i)) != 0) sda_1; else sda_0;
		delayus(1);
		scl_1;
		delayus(5);
		scl_0;
		delayus(5);
	}
	sda_1;
	delayus(1);
	scl_1;
	if (sda_in != 0) resp = i2c_nak; else resp = i2c_ack;
	delayus(5);
	scl_0;
	delayus(5);
	sda_0;
	delayus(1);
	return resp;
}

uint8_t i2c_read(uint8_t ack_state){
	int8_t i;
	uint8_t v = 0;
	sda_1;
	delayus(1);
	for (i = 7; i >= 0; i--){
		v <<= 1;
		scl_1;
		if (sda_in != 0) v++;
		delayus(5);
		scl_0;
		delayus(5);
	}
	if (ack_state) sda_1; else sda_0;
	scl_1;
	delayus(5);
	scl_0;
	delayus(5);
	return v;
}

void i2c_wrmem(uint8_t slave_addr, uint16_t addr, uint8_t addr_size, uint8_t data){
	uint8_t hi_addr, lo_addr;
	hi_addr = addr/256;
	lo_addr = addr%256;
	i2c_start();
	i2c_write((slave_addr & 0xfe)); // Garante que op seja 0 (wr)
	if(addr_size == 2)
	i2c_write(hi_addr);
	i2c_write(lo_addr);
	i2c_write(data);
	i2c_stop();
}

uint8_t i2c_rdmem(uint8_t slave_addr, uint16_t addr, uint8_t addr_size){
	uint8_t resp;
	uint8_t hi_addr, lo_addr;
	hi_addr = addr/256;
	lo_addr = addr%256;
	i2c_start();
	i2c_write((slave_addr & 0xfe)); // Garante que op seja 0 (wr)
	if(addr_size == 2)
	i2c_write(hi_addr);
	i2c_write(lo_addr);
	i2c_stop();
	i2c_start();
	i2c_write((slave_addr & 0xfe) + 1); // Garante que op seja 1 (rd)
	resp = i2c_read(i2c_nak);
	i2c_stop();
	return resp;
}

void sht_init(void){
    data_0;
    sck_0;
    data_1;
    delayus(10);
}

void sht_pulse(void){
    delayus(1);
    sck_1;
    delayus(2);
    sck_0;
    delayus(1);
}

void sht_start(void){
    sck_1;
    delayus(1);
    data_0;
    delayus(1);
    sck_0;
    delayus(1);
    sck_1;
    delayus(1);
    data_1;
    delayus(1);
    sck_0;
    delayus(1);
}

void sht_stop(void){
    data_1;
    sht_pulse();
}

void sht_wrCmd(uint8_t data){
    uint8_t n;
    for(n=0; n<8; n++){
        if((data & (1<<(7-n))) == 0) data_0; else data_1;
        sht_pulse();
    }
    data_1;
    sht_pulse();
}

uint8_t sht_rdData(uint8_t confirm){
    uint8_t n, val=0;
    data_1;
    for(n=0; n<8; n++){
        val <<= 1;
        delayus(1);
        sck_1;
        delayus(1);
        if(data_in != 0) val++;
        delayus(1);
        sck_0;
        delayus(1);
    }
    if(confirm == 0) data_0; else data_1;
    sht_pulse();
    data_1;
    return val;
}

float sht_rdHumid(void){
    uint16_t humid = 0;
    uint8_t Hi_data, Lo_data;
    float linear_humid;
    sht_start();
    sht_wrCmd(0x05);
    while(data_in != 0) HAL_Delay(1);
    Hi_data = sht_rdData(0);
    Lo_data = sht_rdData(0xff);
    humid = (Hi_data<<8)+Lo_data;
    linear_humid = (4e-10)*humid*humid*humid - 0.000005*humid*humid + 0.0434*humid - 7.3845;
    return linear_humid;
}

void sht_reSynch(void){
	uint8_t n;
	data_1;
	for(n=0; n<10; n++) sht_pulse();
}

void put_password(char *senha){
	char ch;
	if((HAL_GetTick() - Tsenha) >= 300){
		Tsenha = HAL_GetTick();

		ch = getKeyPad();
		if(ch != 0 && ch != '*' && ch != '#'){
			serial = 0;
			goTo(countSenha, 1);
			senha[countSenha++] = ch;
			printf("%c\n", ch);
		}
	}
}

int verify_password(char *senha){
	for(int i = 0; i < 4; i++)
		if(senha[i] != i2c_rdmem(0xa0, 100 + i, 1))
			return 0;
	return 1;
};

void getMov(void){
	if(movInp != 0) LEDMov_1; else LEDMov_0;
}

void buzzerTrava(void){
	buzzer_1;
	HAL_Delay(500);
	buzzer_0;
}

void buzzerAlarme(void){
	buzzer_1;
	HAL_Delay(100);
	buzzer_0;
	HAL_Delay(100);
}

void checkLDR(void){
	int leitura;
	configuraAD.Channel = ADC_CHANNEL_1;
	HAL_ADC_ConfigChannel(&hadc1, &configuraAD);
	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 1);
	leitura = HAL_ADC_GetValue(&hadc1);
	HAL_ADC_Stop(&hadc1);

	if(leitura >= 3600){
		GPIOD->BRR = 1 << 8;
		GPIOD->BRR = 1 << 9;
	}
	else {
		GPIOD->BSRR = 1 << 8;
		GPIOD->BSRR = 1 << 9;
	}
}

float checkMQ7(void){
	int leitura;
	configuraAD.Channel = ADC_CHANNEL_4;
	HAL_ADC_ConfigChannel(&hadc1, &configuraAD);
	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 1);
	leitura = HAL_ADC_GetValue(&hadc1);
	HAL_ADC_Stop(&hadc1);

	return 500.0*leitura/4096;
}

void getUart(uint8_t *ans){
	char ch = 0;
	serial = 1;
	if(*ans){
		*ans = 0;
		printf("========================\r\n");
		printf("(1) Ligar LED quarto\r\n");
		printf("(2) Abre/Fecha Porta\r\n");
		printf("(3) Abre/Fecha Janela da Sala\r\n");
		printf("(4) Abre/Fecha Janela do Quarto\r\n");

	} else {
		HAL_UART_Receive(&huart2,(uint8_t*)&ch,1,10);
		if(ch != 0){
			printf("-> %c\r\n", ch);
			*ans = 1;
		}
	}

	switch(ch){
		case '1':
			if(ledQuarto) GPIOC->BRR = 1 << 9;
			else GPIOC->BSRR = 1 << 9;
			ledQuarto = !ledQuarto;
			break;

		case '2':
			portaAberta = !portaAberta;
			break;

		case '3':
			janelaSalaAberta = !janelaSalaAberta;
			break;

		case '4':
			janelaQuartoAberta = !janelaQuartoAberta;
			break;

		default: break;
	}
}

void servos(void){
	if(portaAberta == 1 && portaAbertaReed == 0){
		for(int i = 0; i < 20; i++){
			SERVO_ON_PORTA;
			HAL_Delay(1);
			SERVO_OFF_PORTA;
			HAL_Delay(19);
		}
		portaAbertaReed = 1;
	}else if(portaAberta == 0 && portaAbertaReed == 1){
		for(int i = 0; i < 20; i++){
			SERVO_ON_PORTA;
			HAL_Delay(2);
			SERVO_OFF_PORTA;
			HAL_Delay(18);
		}
		portaAbertaReed = 0;
	}


	if(janelaSalaAberta == 1 && janelaSalaAbertaReed == 0){
		for(int i = 0; i < 20; i++){
			SERVO_ON_JANELA_SALA;
			HAL_Delay(1);
			SERVO_OFF_JANELA_SALA;
			HAL_Delay(19);
		}
		janelaSalaAbertaReed = 1;

	} else if(janelaSalaAberta == 0 && janelaSalaAbertaReed == 1){
		for(int i = 0; i < 20; i++){
			SERVO_ON_JANELA_SALA;
			HAL_Delay(2);
			SERVO_OFF_JANELA_SALA;
			HAL_Delay(18);
		}
		janelaSalaAbertaReed = 0;
	}

	if(janelaQuartoAberta == 1 && janelaQuartoAbertaReed == 0){
		for(int i = 0; i < 20; i++){
			SERVO_ON_JANELA_QUARTO;
			HAL_Delay(2);
			SERVO_OFF_JANELA_QUARTO;
			HAL_Delay(18);
		}
		janelaQuartoAbertaReed = 1;
	} else if(janelaQuartoAberta == 0 && janelaQuartoAbertaReed == 1){
		for(int i = 0; i < 20; i++){
			SERVO_ON_JANELA_QUARTO;
			HAL_Delay(1);
			SERVO_OFF_JANELA_QUARTO;
			HAL_Delay(19);
		}
		janelaQuartoAbertaReed = 0;
	}
}

void fechaTudo(void){
	if(bloqueado == 1 && (portaAbertaReed == 1 || janelaSalaAbertaReed == 1 || janelaQuartoAbertaReed == 1)){
		for(int i = 0; i < 20; i++){
			SERVO_ON_JANELA_QUARTO;
			HAL_Delay(1);
			SERVO_OFF_JANELA_QUARTO;
			HAL_Delay(19);
		}
		janelaQuartoAbertaReed = 0;

		for(int i = 0; i < 20; i++){
			SERVO_ON_JANELA_SALA;
			HAL_Delay(2);
			SERVO_OFF_JANELA_SALA;
			HAL_Delay(18);
		}
		janelaSalaAbertaReed = 0;

		for(int i = 0; i < 20; i++){
			SERVO_ON_PORTA;
			HAL_Delay(2);
			SERVO_OFF_PORTA;
			HAL_Delay(18);
			}
		portaAbertaReed = 0;
	}
}

//Protocolo Spi
void spi_init(void)
{
    clk_0;
    cs_1;
    don_0;
    dc_0;
    rst_1;
}

void spi_clock(void)
{
    clk_1;
    delayus(10);
    clk_0;
    delayus(10);
}

//Tela LCD - Pmod Oled rgb
void oled_write(uint8_t data)
{
    for(int i=0;i<8;i++)
    {
        if(data & 0x80)
            don_1;
        else
            don_0;

        spi_clock();
        data <<= 1;
    }
}

void oled_cmd(uint8_t cmd)
{
    dc_0;
    cs_0;
    oled_write(cmd);
    cs_1;
}

void oled_data(uint8_t data)
{
    dc_1;
    cs_0;
    oled_write(data);
    cs_1;
}


void oled_init(void)
{
    //Sequência de Reset por Hardware
    rst_1;
    HAL_Delay(10);
    rst_0;          // Ativa o reset
    HAL_Delay(20);  // Mantém em nível baixo
    rst_1;          // Libera o reset
    HAL_Delay(20);  // Aguarda estabilização

    //Sequência Oficial de Comandos de Inicialização do SSD1331
    oled_cmd(0xAE); // Display OFF

    oled_cmd(0xA0); // Set Remap & Data Format
    oled_cmd(0x72); // Configuração de cor RGB (RGB565) e orientação

    oled_cmd(0xA1); // Set Display Start Line
    oled_cmd(0x00);

    oled_cmd(0xA2); // Set Display Offset
    oled_cmd(0x00);

    oled_cmd(0xA4); // Normal Display Mode

    oled_cmd(0xA8); // Set Multiplex Ratio
    oled_cmd(0x3F); // 64MUX (Tela de 64 linhas)

    oled_cmd(0xAD); // Set Master Configuration
    oled_cmd(0x8E); // Seleciona a fonte de alimentação externa

    oled_cmd(0xB0); // Power Save Mode
    oled_cmd(0x0B); // Desabilitado

    oled_cmd(0xB1); // Phase 1 and 2 Period Adjustment
    oled_cmd(0x31);

    oled_cmd(0xB3); // Display Clock Divide Ratio / Oscillator Frequency
    oled_cmd(0xF0);

    oled_cmd(0x8A); // Set Pre-charge Speed For Red
    oled_cmd(0x64);
    oled_cmd(0x8B); // Set Pre-charge Speed For Green
    oled_cmd(0x78);
    oled_cmd(0x8C); // Set Pre-charge Speed For Blue
    oled_cmd(0x64);

    oled_cmd(0xBB); // Set VCOMH Voltage
    oled_cmd(0x3A);

    oled_cmd(0xBE); // Set VREG Voltage
    oled_cmd(0x3E);

    oled_cmd(0x81); // Master Contrast Control
    oled_cmd(0x91); // Brilho em nível médio/alto

    // Ajuste de contraste individual dos canais de cores
    oled_cmd(0x82); // Contraste Vermelho
    oled_cmd(0xFF);
    oled_cmd(0x83); // Contraste Verde
    oled_cmd(0xFF);
    oled_cmd(0x87); // Contraste Azul
    oled_cmd(0xFF);

    HAL_Delay(50);  // Aguarda a estabilização das tensões internas

    oled_cmd(0xAF); // Display ON (Ativa a tela definitivamente)
    HAL_Delay(100);
}

static const uint8_t font5x7[][5] =
{
    {0x7F,0x02,0x0C,0x02,0x7F}, // M - 0
    {0x00,0x44,0x7D,0x40,0x00}, // i - 1
    {0x7C,0x08,0x04,0x04,0x78}, // n - 2
    {0x46,0x49,0x49,0x49,0x31}, // S - 3
    {0x7C,0x04,0x18,0x04,0x78}, // m - 4
    {0x20,0x54,0x54,0x54,0x78}, // a - 5
    {0x7C,0x08,0x04,0x04,0x08}, // r - 6
    {0x04,0x3F,0x44,0x40,0x20}, // t - 7
    {0x7F,0x08,0x08,0x08,0x7F}, // H - 8
    {0x38,0x44,0x44,0x44,0x38}, // o - 9
    {0x3C,0x40,0x40,0x20,0x7C}, // u - 10
    {0x48,0x54,0x54,0x54,0x20}, // s - 11
    {0x38,0x54,0x54,0x54,0x18}, // e - 12
    {0x7F,0x49,0x49,0x49,0x36}, // B - 13
    {0x1C,0x20,0x40,0x20,0x1F}, // v - 14
    {0x38,0x44,0x44,0x48,0x7F}, // d - 15
    {0x08,0x08,0x08,0x08,0x08}, // - - 16
    {0x00,0x00,0x5F,0x00,0x00}, // ! - 17
    {0x7F,0x08,0x04,0x04,0x7F}, // h - 18
    {0x00,0x00,0x00,0x00,0x00}  // (Espaço) - 19
};

static const uint8_t locked_icon[8][8] =
{
    {0,0,1,1,1,1,0,0},
    {0,1,0,0,0,0,1,0},
    {0,1,0,0,0,0,1,0},
    {1,1,1,1,1,1,1,1},
    {1,1,1,0,0,1,1,1},
    {1,1,1,0,0,1,1,1},
    {1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1}
};

static const uint8_t opened_icon[8][8] =
{
    {0,0,1,1,1,1,0,0},
    {0,1,0,0,0,0,1,0},
    {0,1,0,0,0,0,0,0},
    {1,1,1,1,1,1,1,1},
    {1,1,1,0,0,1,1,1},
    {1,1,1,0,0,1,1,1},
    {1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1}
};

// -------------------- IMPLEMENTAÇÃO DAS FUNÇÕES --------------------

void oled_putpixel(uint8_t x, uint8_t y, uint16_t color)
{
    if(x >= OLED_W || y >= OLED_H) return;
    oled_fb[y * OLED_W + x] = color;
}

void oled_fill_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t color)
{
    for(uint8_t i = 0; i < w; i++)
    {
        for(uint8_t j = 0; j < h; j++)
        {
            oled_putpixel(x + i, y + j, color);
        }
    }
}

void oled_pixel2x(uint8_t x, uint8_t y, uint16_t color)
{
    oled_putpixel(x,     y,     color);
    oled_putpixel(x + 1, y,     color);
    oled_putpixel(x,     y + 1, color);
    oled_putpixel(x + 1, y + 1, color);
}

void oled_set_window(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
    oled_cmd(0x15);
    oled_cmd(x0);
    oled_cmd(x1);

    oled_cmd(0x75);
    oled_cmd(y0);
    oled_cmd(y1);
}

void oled_update(void)
{
    oled_set_window(0, 0, OLED_W - 1, OLED_H - 1);

    dc_1;
    cs_0;

    for(uint16_t i = 0; i < OLED_W * OLED_H; i++)
    {
        oled_write(oled_fb[i] >> 8);
        oled_write(oled_fb[i] & 0xFF);
    }

    cs_1;
}

void oled_draw_char(uint8_t x, uint8_t y, char c, uint16_t color)
{
    // Desenha o H manualmente
    if(c == 'H')
    {
        // Barra esquerda
        for(uint8_t i = 0; i < 7; i++)
            oled_putpixel(x, y + i, color);

        // Barra direita
        for(uint8_t i = 0; i < 7; i++)
            oled_putpixel(x + 4, y + i, color);

        // Barra central
        for(uint8_t i = 0; i < 5; i++)
            oled_putpixel(x + i, y + 3, color);

        return;
    }

    static const char lookup[] = "MinSmarthouseBvd-!h";
    int8_t index = -1;

    if(c == ' ')
    {
        return;
    }

    for(uint8_t i = 0; lookup[i] != '\0'; i++)
    {
        if(lookup[i] == c)
        {
            index = i;
            break;
        }
    }

    if(index < 0)
        return;

    for(uint8_t col = 0; col < 5; col++)
    {
        uint8_t line = font5x7[index][col];

        for(uint8_t row = 0; row < 7; row++)
        {
            if(line & (1 << row))
            {
                oled_putpixel(x + col, y + row, color);
            }
        }
    }
}

void oled_draw_icon(uint8_t x, uint8_t y, const uint8_t icon[8][8], uint16_t color)
{
    for(uint8_t row = 0; row < 8; row++)
    {
        for(uint8_t col = 0; col < 8; col++)
        {
            if(icon[row][col])
            {
                oled_pixel2x(x + col * 2, y + row * 2, color);
            }
        }
    }
}

void oled_draw_background(void)
{
    oled_fill_rect(0, 0, OLED_W, OLED_H, 0x0000);

    // Moldura
    for(uint8_t x = 0; x < OLED_W; x++)
    {
        oled_putpixel(x, 0, 0xFFFF);
        oled_putpixel(x, OLED_H - 1, 0xFFFF);
    }

    for(uint8_t y = 0; y < OLED_H; y++)
    {
        oled_putpixel(0, y, 0xFFFF);
        oled_putpixel(OLED_W - 1, y, 0xFFFF);
    }

    char l0[] = "Bem-vindo!";
    char l1[] = "Mini Smart";
    char l2[] = "House";

    // Centralização simples
    uint8_t x0 = (OLED_W - (sizeof(l0)-1) * 6) / 2;
    uint8_t x1 = (OLED_W - (sizeof(l1)-1) * 6) / 2;
    uint8_t x2 = (OLED_W - (sizeof(l2)-1) * 6) / 2;

    for(uint8_t i = 0; l0[i] != '\0'; i++)
        oled_draw_char(x0 + i * 6, 8, l0[i], 0xFFFF);

    for(uint8_t i = 0; l1[i] != '\0'; i++)
        oled_draw_char(x1 + i * 6, 22, l1[i], 0xFFFF);

    for(uint8_t i = 0; l2[i] != '\0'; i++)
        oled_draw_char(x2 + i * 6, 36, l2[i], 0xFFFF);

    oled_update();
}
// -------------------- CONTROLE LOCAL DOS CADEADOS COM MARGEM --------------------

// Limpa a área do ícone Locked (X: 3 a 18, Y: 45 a 60)
void oled_clear_locked_buffer(void)
{
    oled_fill_rect(3, 45, 16, 16, 0x0000);
}

// Limpa a área do ícone Opened (X: 77 a 92, Y: 45 a 60)
void oled_clear_opened_buffer(void)
{
    oled_fill_rect(77, 45, 16, 16, 0x0000);
}

// Atualiza fisicamente a janela do ícone da esquerda (Locked)
void oled_update_locked_area(void)
{
    oled_set_window(3, 45, 18, 60);

    dc_1;
    cs_0;

    for(uint8_t y = 45; y <= 60; y++)
    {
        for(uint8_t x = 3; x <= 18; x++)
        {
            uint16_t color = oled_fb[y * OLED_W + x];
            oled_write(color >> 8);
            oled_write(color & 0xFF);
        }
    }

    cs_1;
}

// Atualiza fisicamente a janela do ícone da direita (Opened)
void oled_update_opened_area(void)
{
    oled_set_window(77, 45, 92, 60);

    dc_1;
    cs_0;

    for(uint8_t y = 45; y <= 60; y++)
    {
        for(uint8_t x = 77; x <= 92; x++)
        {
            uint16_t color = oled_fb[y * OLED_W + x];
            oled_write(color >> 8);
            oled_write(color & 0xFF);
        }
    }

    cs_1;
}

void oled_draw_locked(void)
{
    oled_clear_opened_buffer();
    oled_update_opened_area();

    oled_fill_rect(3, 45, 16, 16, 0x0000);
    oled_draw_icon(3, 45, locked_icon, 0xF800);

    oled_update_locked_area();
}

void oled_draw_opened(void)
{
    oled_clear_locked_buffer();
    oled_update_locked_area();

    oled_fill_rect(77, 45, 16, 16, 0x0000);
    oled_draw_icon(77, 45, opened_icon, 0x07E0);

    oled_update_opened_area();
}

void lock(void){
	if(bloqueado == 1 && draw == 0){
		oled_draw_locked();
		draw = 1;
	}
	else if(bloqueado == 0 && draw == 1){
		oled_draw_opened();
		draw = 0;
	}
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	int TSensor = 0;
	uint8_t flagAlarme = 0;
	uint8_t ans = 1;
	uint8_t h,m,s;
	int state = 0;
	char senha[5] = "    \0";
	RTC_TimeTypeDef relogio;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */

  i2c_init();
  sht_init();
  lcd_init(cursor_off);
  lcd_bk;
  lcd_progchar();

  configuraAD.Rank = ADC_REGULAR_RANK_1;
  configuraAD.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;

  HAL_RTC_Init(&hrtc);
  HAL_RTC_WaitForSynchro(&hrtc);

  relogio.Hours = 20;
  relogio.Minutes = 45;
  relogio.Seconds = 45;
  HAL_RTC_SetTime(&hrtc, &relogio, RTC_FORMAT_BIN);

  spi_init();
  oled_init();
  oled_draw_background();

  HAL_Delay(500);
  oled_draw_locked();
  fechaTudo();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while(1){
	  switch(state){
		  case 0:
			  serial = 0;
			  goTo(0,0);
			  printf("Digite a senha\n");
			  put_password(senha);
			  if(countSenha >= 4){
				  state = 1;
				  countSenha = 0;
				  senha[4] = '\0';
				  clear();
				  i2c_wrmem(0xa0, 100, 1, senha[0]);
				  HAL_Delay(20);
				  i2c_wrmem(0xa0, 101, 1, senha[1]);
				  HAL_Delay(20);
				  i2c_wrmem(0xa0, 102, 1, senha[2]);
				  HAL_Delay(20);
				  i2c_wrmem(0xa0, 103, 1, senha[3]);
				  HAL_Delay(20);
				  fechaTudo();
			  }
			  break;

		  case 1:
			  lock();
			  if(getKeyPad() == '#'){
				  	serial = 0;
			  		countSenha = 0;
			  		goTo(0,1);
			  		printf("    \n");
			  		bloqueado = 1;
			  		fechaTudo();
			  } else if(bloqueado) put_password(senha);
			  if(countSenha >= 4 && bloqueado){
				  serial = 0;
				  countSenha = 0;
				  senha[4] = 0;
				  goTo(0,1);
				  printf("    \n");

				  if(verify_password(senha)){
					  bloqueado = 0;
					  goTo(15,0);
					  flagAlarme = 0;
					  buzzerTrava();
				  } else flagAlarme = 1;
			  }

			  HAL_RTC_GetTime(&hrtc, &relogio, RTC_FORMAT_BIN);
			  h = relogio.Hours;
			  m = relogio.Minutes;
			  s = relogio.Seconds;
			  serial = 0;
			  goTo(8,1);
			  printf("%02d:%02d:%02d\n", h, m, s);

			  if(flagAlarme) buzzerAlarme();

			  if(bloqueado == 0){
				  getUart(&ans);
				  if(HAL_GetTick() - TSensor >= 1000){
				  		TSensor = HAL_GetTick();
				  		checkLDR();
				  		serial = 0;
				  	  	goTo(0,0);
				  		printf("%2.2f%%\n", sht_rdHumid());
				  		lcd_wrchar(0);
				  		goTo(8,0);
				  		printf("%3.0f ppm\n", checkMQ7());
				  }
				  servos();
				  getMov();
			  }
			  break;
		  default:
			  break;
	  }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.LowPowerAutoPowerOff = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc1.Init.SamplingTimeCommon1 = ADC_SAMPLETIME_1CYCLE_5;
  hadc1.Init.SamplingTimeCommon2 = ADC_SAMPLETIME_1CYCLE_5;
  hadc1.Init.OversamplingMode = DISABLE;
  hadc1.Init.TriggerFrequencyMode = ADC_TRIGGER_FREQ_HIGH;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLINGTIME_COMMON_1;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  hrtc.Init.OutPutPullUp = RTC_OUTPUT_PULLUP_NONE;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart2, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart2, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_4|GPIO_PIN_5
                          |GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_10
                          |GPIO_PIN_11|GPIO_PIN_14|GPIO_PIN_3|GPIO_PIN_4
                          |GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8
                          |GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_0|GPIO_PIN_1
                          |GPIO_PIN_2|GPIO_PIN_3, GPIO_PIN_RESET);

  /*Configure GPIO pins : PC11 PC12 PC4 PC5
                           PC7 PC8 PC9 */
  GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_4|GPIO_PIN_5
                          |GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PC0 PC1 PC2 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 PB2 PB10
                           PB11 PB14 PB3 PB4
                           PB5 PB7 PB8 PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_10
                          |GPIO_PIN_11|GPIO_PIN_14|GPIO_PIN_3|GPIO_PIN_4
                          |GPIO_PIN_5|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PB12 */
  GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PA8 PA9 PA10 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PD8 PD9 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : PD0 PD1 PD2 PD3 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : PD4 PD5 PD6 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : PB6 */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PC10 */
  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
