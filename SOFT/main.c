#include "stm8s.h"
#include <iostm8s103.h>
//#include "main.h"

#define TIME_CONST	100

@near bool b1000Hz=0,b100Hz=0,b10Hz=0,b5Hz=0,b2Hz=0,b1Hz=0;
@near char t0_cnt0=0,t0_cnt1=0,t0_cnt2=0,t0_cnt3=0,t0_cnt4=0;
bool bFL5,bFL2,bFL1,bFL_;

@near bool bTIME_WRK=0;
@near signed short time_wrk_cnt,time_wrk_cnt_max=TIME_CONST;

@near enum {p1=0,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11} program_stat=p9;
@near char port_temp;

const char program_steps_max[]={6/*p1*/,6/*p2*/,12/*p3*/,12/*p4*/,12/*p5*/,12/*p6*/,12/*p7*/,6/*p8*/,6/*p9*/,28/*p10*/,28/*p11*/}; 
const char program_repeat_max[]={10/*p1*/,10/*p2*/,5/*p3*/,5/*p4*/,5/*p5*/,5/*p6*/,5/*p7*/,10/*p8*/,10/*p9*/,3/*p10*/,3/*p11*/};

@near char program_steps;
@near char program_repeat;

@near short adc_buff[16];
@near short adc_buff_;
@near char adc_cnt;

#define MAX_PROGRAM_STAT	11

const char p1_const[]={	0b00000001,	//Бегущий огонь вперед
						0b00000010,
						0b00000100,
						0b00001000,
						0b00010000,
						0b00100000};
						
const char p2_const[]={	0b00100000,	//Бегущий огонь назад
						0b00010000,
						0b00001000,
						0b00000100,
						0b00000010,
						0b00000001};
						
const char p3_const[]={	0b00000001,	//Накапливающееся включение и гашение вперед
						0b00000011,
						0b00000111,
						0b00001111,
						0b00011111,
						0b00111111,
						0b00111110,
						0b00111100,
						0b00111000,
						0b00110000,
						0b00100000,
						0b00000000};
						
const char p4_const[]={	0b00100000,	//Накапливающееся включение и гашение назад
						0b00110000,
						0b00111000,
						0b00111100,
						0b00111110,
						0b00111111,
						0b00011111,
						0b00001111,
						0b00000111,
						0b00000011,
						0b00000001,
						0b00000000};

const char p5_const[]={	0b00000001,	//Гусеница вперед
						0b00000011,
						0b00000010,
						0b00000110,
						0b00000100,
						0b00001100,
						0b00001000,
						0b00011000,
						0b00010000,
						0b00110000,
						0b00100000,
						0b00100001};

const char p6_const[]={	0b00100000,	//Гусеница назад
						0b00110000,
						0b00010000,
						0b00011000,
						0b00001000,
						0b00001100,
						0b00000100,
						0b00000110,
						0b00000010,
						0b00000011,
						0b00000001,
						0b00100001};
						
const char p7_const[]={	0b00100000,	//встречное накапливающееся включение-гашение
						0b00100001,
						0b00110001,
						0b00110011,
						0b00111011,
						0b00111111,
						0b00111011,
						0b00110011,
						0b00110001,
						0b00100001,
						0b00100000,
						0b00000000};
						
const char p8_const[]={	0b00111110,	//Бегущая тень вперед
						0b00111101,
						0b00111011,
						0b00110111,
						0b00101111,
						0b00011111};
						
const char p9_const[]={	0b00011111,	//Бегущая тень назад
						0b00101111,
						0b00110111,
						0b00111011,
						0b00111101,
						0b00111110};

const char p10_const[]={0b00000000,	//набегающие огни вперед и накапливающееся гашение вперед 
						0b00000001,
						0b00000010,
						0b00000100,
						0b00001000,
						0b00010000,
						0b00100000,
						0b00100001,
						0b00100010,
						0b00100100,
						0b00101000,
						0b00110000,
						0b00110001,
						0b00110010,
						0b00110100,
						0b00111000,
						0b00111001,
						0b00111010,
						0b00111100,
						0b00111101,
						0b00111110,
						0b00111111,
						0b00111110,
						0b00111100,
						0b00111000,
						0b00110000,
						0b00100000,
						0b00000000};
						
const char p11_const[]={0b00000000,	//набегающие огни назад и накапливающееся гашение назад 
						0b00100000,
						0b00010000,
						0b00001000,
						0b00000100,
						0b00000010,
						0b00000001,
						0b00100001,
						0b00010001,
						0b00001001,
						0b00000101,
						0b00000011,
						0b00100011,
						0b00010011,
						0b00001011,
						0b00000111,
						0b00100111,
						0b00010111,
						0b00001111,
						0b00101111,
						0b00011111,
						0b00111111,
						0b00011111,
						0b00001111,
						0b00000111,
						0b00000011,
						0b00000001,
						0b00000000};
						

//

//-----------------------------------------------
void time_wrk(void)
{
time_wrk_cnt_max=adc_buff_*2;

time_wrk_cnt++;
if(time_wrk_cnt>=time_wrk_cnt_max)
	{
	time_wrk_cnt=0;
	bTIME_WRK=1;
	}

}

//-----------------------------------------------
void wrk_hndl(void)
{
if(bTIME_WRK)
	{
	program_steps++;
	if(program_steps>=program_steps_max[program_stat])
		{
		program_steps=0;
		program_repeat++;
		if(program_repeat>=program_repeat_max[program_stat])
			{
			program_repeat=0;
			program_stat++;
			if(program_stat>=MAX_PROGRAM_STAT)program_stat=0;
			}
		}
	
	if(program_stat==p1)
		{
		port_temp=p1_const[program_steps];	
		}
	else if(program_stat==p2)
		{
		port_temp=p2_const[program_steps];	
		}
	else if(program_stat==p3)
		{
		port_temp=p3_const[program_steps];	
		}		
	else if(program_stat==p4)
		{
		port_temp=p4_const[program_steps];	
		}	
	else if(program_stat==p5)
		{
		port_temp=p5_const[program_steps];	
		}	
	else if(program_stat==p6)
		{
		port_temp=p6_const[program_steps];	
		}	
	else if(program_stat==p7)
		{
		port_temp=p7_const[program_steps];	
		}
	else if(program_stat==p8)
		{
		port_temp=p8_const[program_steps];	
		}	
	else if(program_stat==p9)
		{
		port_temp=p9_const[program_steps];	
		}	
	else if(program_stat==p10)
		{
		port_temp=p10_const[program_steps];	
		}	
	else if(program_stat==p11)
		{
		port_temp=p11_const[program_steps];	
		}



	GPIOD->ODR=port_temp;
	}
bTIME_WRK=0;
}

//-----------------------------------------------
void gpio_init(void)
{


GPIOD->DDR|=0xff;
GPIOD->CR1|=0xff;
GPIOD->CR2&=~0xff;


}

//-----------------------------------------------
void t4_init(void)
{
TIM4->PSCR = 7;
TIM4->ARR= 123;
TIM4->IER|= TIM4_IER_UIE;					// enable break interrupt

TIM4->CR1=(TIM4_CR1_URS | TIM4_CR1_CEN | TIM4_CR1_ARPE);	
}
//-----------------------------------------------
void adc_init(void){
	GPIOB->DDR&=~(1<<2);
	GPIOB->CR1&=~(1<<2);
	GPIOB->CR2&=~(1<<2);
	ADC1->TDRL|=(1<<2);

	
	ADC1->CR2=0x08;
	ADC1->CR1=0x40;
	ADC1->CSR=0x20+2;
	
	ADC1->CR1|=1;
ADC1->CR1|=1;
}


//***********************************************
//***********************************************
//***********************************************
//***********************************************
@far @interrupt void TIM4_UPD_Interrupt (void) 
{
b1000Hz=1;
if(++t0_cnt0>=10)
	{		
	t0_cnt0=0;
	b100Hz=1;

	if(++t0_cnt1>=10)
		{
		t0_cnt1=0;
		b10Hz=1;
		}
		
	if(++t0_cnt2>=20)
		{
		t0_cnt2=0;
		b5Hz=1;
		bFL5=!bFL5;
		}

	if(++t0_cnt3>=50)
		{
		t0_cnt3=0;
		b2Hz=1;
		bFL2=!bFL2;		
		}

	if(++t0_cnt4>=100)
		{
		t0_cnt4=0;
		b1Hz=1;
		bFL1=!bFL1;
		}
	}

TIM4->SR1&=~TIM4_SR1_UIF;			// disable break interrupt
return;
}
//***********************************************
@far @interrupt void ADC_EOC_Interrupt (void) {

signed long temp_adc;


/*		GPIOA->DDR|=(1<<1);
		GPIOA->CR1|=(1<<1);
		GPIOA->CR2&=~(1<<1);	
		GPIOA->ODR|=(1<<1);*/

ADC1->CSR&=~(1<<7);

temp_adc=(((signed long)(ADC1->DRH))*256)+((signed long)(ADC1->DRL));

//temp_adc=4000;
//temp_adc=720;


adc_buff[adc_cnt]=temp_adc;

//adc_plazma=ADC1->DR;
//if(adc_ch==0)adc_plazma_short=temp_adc;

adc_cnt++;
if(adc_cnt>=16)
	{
	adc_cnt=0;
	}


if((adc_cnt&0x03)==0)
	{
	signed long tempSS;
	char i;
	tempSS=0;
		

	
	for(i=0;i<16;i++)
		{
		tempSS+=(signed long)adc_buff[i];
		}
	adc_buff_=(signed short)(tempSS>>4);
	}

//adc_buff_[adc_ch]=adc_ch*10;

//GPIOD->ODR&=~(1<<0);

//ADC1->CR1&=~(1<<0);

/*
adcw[0]=(ADC1->DB0RL)+((ADC1->DB0RH)*256);
adcw[1]=(ADC1->DB1RL)+((ADC1->DB1RH)*256);
adcw[2]=(ADC1->DB2RL)+((ADC1->DB2RH)*256);*/




//GPIOD->ODR|=(1<<0);


	
		//GPIOA->ODR&=~(1<<1);
}
//***********************************************
@far @interrupt void TIM1_Ovf_Interrupt (void) 
{
//GPIOD->ODR^=(1<<3)|(1<<4);
TIM1->SR1&=~TIM1_SR1_UIF;			
}
//===============================================
//===============================================
//===============================================
//===============================================
main()
{
CLK->CKDIVR=0;
	
gpio_init();
	//_init();
	//t2_init();
	
FLASH_DUKR=0xae;
FLASH_DUKR=0x56;	


	
t4_init();
enableInterrupts();	

adc_init();



while (1)
	{
//GPIOD->ODR^=0xff;
	if(b1000Hz)
		{
		b1000Hz=0;
		time_wrk();
      	}  	
		
	if(b100Hz)
		{
		b100Hz=0;
		wrk_hndl();	
      	}  
      	
	if(b10Hz)
		{
		b10Hz=0;
		adc_init();	
      	}
      	 
	if(b5Hz)
		{
		b5Hz=0;
		
	//port_temp^=0xff;

	//GPIOD->ODR=port_temp;		
		}
		
	if(b2Hz)
		{
		b2Hz=0;
			
      	}      

	if(b1Hz)
		{
		b1Hz=0;
		//GPIOD->ODR^=0xff;	
		}      	     	      
	}
}