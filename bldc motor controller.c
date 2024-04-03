
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// CON 포트는 포트 C와 연결됨을 정의
#define LCD_CON      PORTC
// DATA 포트는 포트 A와 연결됨을 정의
#define LCD_DATA     PORTA
// DATA 포트의 출력 방향 설정 매크로를 정의
#define LCD_DATA_DIR DDRA
// DATA 포트의 입력 방향 설정 매크로를 정의
#define LCD_DATA_IN  PINA
// RS 신호의 비트 번호 정의
#define LCD_RS   7
// RW 신호의 비트 번호 정의
#define LCD_RW   5
// E 신호의 비트 번호 정의
#define LCD_E    6

void MOTOR_START_CW(uint16_t Hall_data);
void MOTOR_START_CCW(uint16_t Hall_data);
void MOTOR_OCR(uint16_t count);
uint8_t LCD_rCommand(void);
uint8_t LCD_BusyCheck(uint8_t temp);
void LCD_wBCommand(uint8_t cmd);
void LCD_wBCommand(uint8_t cmd);
void LCD_Init(void);
void LCD_wData(uint8_t dat);
void LCD_Clear(void);
void LCD_wString(uint8_t *str);
void pwm1_control(uint16_t x);

uint8_t direction;
uint8_t Hall_data;
uint16_t sum;
uint16_t value;
ISR(INT4_vect)
{
	if (direction == '+')
	{
		if (Hall_data == 0x30)
		{
			PORTD = 0x02;
			TCCR1A = 0X0A;
		}
		else if (Hall_data == 0x40)
		{
			PORTD = 0x08;
			TCCR1A = 0x82;
		}
	}
	else if (direction == '-')
	{
		if (Hall_data == 0x50)
		{
			PORTD = 0x02;
			TCCR1A = 0x22;
		}
		else if (Hall_data == 0x20)
		{
			PORTD = 0x04;
			TCCR1A = 0x82;
		}
	}

}

ISR(INT5_vect)
{
	if (direction == '+')
	{
		if (Hall_data == 0x10)
		{
			PORTD = 0x02;
			TCCR1A = 0x22;
		}
		else if (Hall_data == 0x60)
		{
			PORTD = 0x04;
			TCCR1A = 0x82;
		}
		
	}
	else if (direction == '-')
	{
		if (Hall_data == 0x40)
		{
			PORTD = 0x08;
			TCCR1A = 0x22;
		}
		else if (Hall_data == 0x30)
		{
			PORTD = 0x04;
			TCCR1A = 0x0A;
		}
	}

}
ISR(INT6_vect)
{
	if (direction == '+')
	{
		if (Hall_data == 0x20)
		{
			PORTD = 0x04;
			TCCR1A = 0x0A;
		}
		else if (Hall_data == 0x50)
		{
			PORTD = 0x08;
			TCCR1A = 0x22;
		}
	}
	else if (direction == '-')
	{
		if (Hall_data == 0x60)
		{
			PORTD = 0x08;
			TCCR1A = 0x82;
		}
		else if (Hall_data == 0x10)
		{
			PORTD = 0x02;
			TCCR1A = 0X0A;
		}
	}

}
void MOTOR_OCR(uint16_t count)
{//OCR 설정 함수
	
	OCR1A = count;
	OCR1B = count;
	OCR1CH = count >> 8;
	OCR1CL = count & 0xFF;
}

void MOTOR_START_CW(uint16_t Hall_data)
{
	if (Hall_data == 0x20)
	{
		PORTD = 0x04;
		TCCR1A = 0x0A;
	}
	else if (Hall_data == 0x30)
	{
		PORTD = 0x02;
		TCCR1A = 0X0A;
	}
	else if (Hall_data == 0x10)
	{
		PORTD = 0x02;
		TCCR1A = 0x22;
	}
	else if (Hall_data == 0x50)
	{
		PORTD = 0x08;
		TCCR1A = 0x22;
	}
	else if (Hall_data == 0x40)
	{
		PORTD = 0x08;
		TCCR1A = 0x82;
	}
	else if (Hall_data == 0x60)
	{
		PORTD = 0x04;
		TCCR1A = 0x82;
	}

}
void MOTOR_START_CCW(uint16_t Hall_data)
{
	if (Hall_data == 0x60)
	{
		PORTD = 0x08;
		TCCR1A = 0x82;
	}
	else if (Hall_data == 0x10)
	{
		PORTD = 0x02;
		TCCR1A = 0X0A;
	}
	
	else if (Hall_data == 0x50)
	{
		PORTD = 0x02;
		TCCR1A = 0x22;
	}
	else if (Hall_data == 0x20)
	{
		PORTD = 0x04;
		TCCR1A = 0x82;
	}
	
	else if (Hall_data == 0x40)
	{
		PORTD = 0x08;
		TCCR1A = 0x22;
	}
	else if (Hall_data == 0x30)
	{
		PORTD = 0x04;
		TCCR1A = 0x0A;
	}

}



// 텍스트 LCD로 부터 상태(명령)를 읽는 함수
uint8_t LCD_rCommand(void){
	unsigned char temp=1;
	
	LCD_DATA_DIR = 0X00;
	
	LCD_CON &= ~(1 << LCD_RS);
	LCD_CON |= (1 << LCD_RW);
	LCD_CON |= (1 << LCD_E);
	_delay_us(1);
	
	temp = LCD_DATA_IN;      // 명령 읽기
	_delay_us(1);
	
	LCD_CON &= ~(1 << LCD_E);  // 명령 읽기 동작 끝
	
	LCD_DATA_DIR = 0XFF;
	_delay_us(1);
	
	return temp;
}

// 텍스트 LCD의 비지 플래그 상태를 확인하는 함수
uint8_t LCD_BusyCheck(uint8_t temp){
	if(temp & 0x80)          return 1;
	else            return 0;
}

// 텍스트 LCD에 명령을 출력하는 함수 - 단, 비지플래그 체크하지 않음
void LCD_wCommand(uint8_t cmd){

	LCD_CON &= ~(1 << LCD_RS);
	LCD_CON &= ~(1 << LCD_RW);
	LCD_CON |= (1 << LCD_E);
	
	LCD_DATA = cmd;          // 명령 출력
	_delay_us(1);
	LCD_CON &= ~(1 << LCD_E);  // 명령 쓰기 동작 끝
	
	_delay_us(1);
}

// 텍스트 LCD에 명령을 출력하는 함수 - 단, 비지플래그 체크함
void LCD_wBCommand(uint8_t cmd){
	while(LCD_BusyCheck(LCD_rCommand()))
	_delay_us(1);
	LCD_CON &= ~(1 << LCD_RS);
	LCD_CON &= ~(1 << LCD_RW);
	LCD_CON |= (1 << LCD_E);
	
	LCD_DATA = cmd;          // 명령 출력
	_delay_us(1);
	LCD_CON &= ~(1 << LCD_E);  // 명령 쓰기 동작 끝
	
	_delay_us(1);
}

// 텍스트 LCD를 초기화하는 함수
void LCD_Init(void){
	_delay_ms(100);
	// 비지 플래그를 체크하지 않는 Function Set
	LCD_wCommand(0x38);
	_delay_ms(10);
	// 비지 플래그를 체크하지 않는 Function Set
	LCD_wCommand(0x38);
	_delay_us(200);
	// 비지 플래그를 체크하지 않는 Function Set
	LCD_wCommand(0x38);
	_delay_us(200);
	
	// 비지 플래그를 체크하는 Function Set
	LCD_wBCommand(0x38);
	// 비지 플래그를 체크하는 Display On/Off Control
	LCD_wBCommand(0x0c);
	// 비지 플래그를 체크하는 Clear Display
	LCD_wBCommand(0x01);
}

// 텍스트 LCD에 1바이트 데이터를 출력하는 함수
void LCD_wData(uint8_t dat){
	while(LCD_BusyCheck(LCD_rCommand()))
	_delay_us(1);
	

	LCD_CON |= (1 << LCD_RS);
	LCD_CON &= ~(1 << LCD_RW);
	LCD_CON |= (1 << LCD_E);    //
	LCD_DATA = dat;       // 데이터 출력
	_delay_us(1);
	LCD_CON &= ~(1 << LCD_E);  // 데이터 쓰기 동작 끝
	
	_delay_us(1);
}

void LCD_Clear(void)
{
	LCD_wBCommand(0x01);
	_delay_ms(2);
	//커서나 클리어는 2ms정도가 필요함
}

// 텍스트 LCD에 문자열을 출력하는 함수
void LCD_wString(uint8_t *str){
	while(*str)
	LCD_wData(*str++);
}

void pwm1_control(uint16_t x)
{
	uint8_t l, m, n, o, p;
	uint16_t duty_ratio;

	duty_ratio = x * 0.125;
	o = 0x30 + duty_ratio / 10;
	p = 0x30 + duty_ratio % 10;
	LCD_wBCommand(0x84);
	LCD_wData(o);
	LCD_wBCommand(0x85);
	LCD_wData(p);
	LCD_wBCommand(0x86);
	LCD_wString("%");
}

uint16_t read_adc()
{
	unsigned char adc_low, adc_high;
	ADMUX = 0x00;
	ADCSRA |= 0x40;
	//AD 변환 실행하는 거임. |=이렇게 한 이유는 기존꺼 0x87에서 합치려고
	while ((ADCSRA & 0x10) != 0x10);

	adc_low = ADCL;
	//우측이 값 차있으니까 L쪽 부터 쭈우욱될거임.
	adc_high = ADCH;
	value = (adc_high << 8) | adc_low;
	//쉬프트 시켜서 하나의 값으로 만들어줌.
	return value;
}

int main(void)
{
	
	uint8_t Flag;
	uint8_t str_first1[] = "MOTOR CO";
	uint8_t str_first2[] = "NTROLLER";
	uint8_t str_welcome[] = "WELCOME";
	uint8_t str_title1[] = "DESIGNED";
	uint8_t str_title2[] = " BY HJH ";
	uint8_t str_start1[] = "  PRESS ";
	uint8_t str_start2[] = "START";
	uint8_t str_overcurrent1[] = " OVER CU";
	uint8_t str_overcurrent2[] = "RRENT!!!";
	uint8_t str_disconnect1[] = "DISCONNE";
	uint8_t str_disconnect2[] = "CTED !!!";
	uint8_t str_wait1[] = "--PLEASE";
	uint8_t str_wait2[] = " WAIT-- ";

	long i;
	uint16_t j, k, count = 0; // i,j,k는 for문에서
	uint8_t a, b, c = 0;        // a,b,c는 전류계산할때
	uint8_t old_key, new_key;   //버튼

	DDRA = 0xFF;        // LCD 데이터 출력으로설정
	PORTA = 0x00;       //LCD DATA PORT

	DDRB = 0xF0;        //PB.0~PB.3 -> SW1~4,(PDB.4:X),   PB.5:UH, PB.6:VH, PB.7:WH
	PORTB = 0x0F;       //SW PULL UP ON

	DDRC = 0xFF;        //LED 및 LCD 제어를 위한 출력
	PORTC = 0x00;        //PC.0~PC.3->LED1~4, LCD CONTROL PORT(PC.5~PC.7)  5:RW 6:E 7:RS

	DDRD = 0xFF;         // PD.1:UL, PD.2:VL, PD.3:WL PD.4~7 LED DEBUG
	PORTD = 0x00;

	DDRE = 0x00;        //PE.4:HU, PE.5:HV, PE.6:HW    ,PE.0:RXD, PE.1:TXD
	PORTE = 0xF0;        //PULL UP ON

	DDRF = 0x00;
	PORTF = 0xF0;        //PF.0:I_SENSE_INPUT

	DDRG = 0xFF;
	PORTG = 0x00;       //PG.3:BUZZER

	//    TCCR1A=(1<<COM1A1) | (0<<COM1A0) | (1<<COM1B1) | (0<<COM1B0) | (1<<COM1C1) | (0<<COM1C0) | (1<<WGM11) | (0<<WGM10);
	TCCR1A = 0b00000010;     //COM은 OCR값과 같아질때랑 오버플로우때 하이,로우 정하는거
	//    TCCR1B=(0<<ICNC1) | (0<<ICES1) | (0<<WGM13) | (1<<WGM12) | (1<<CS12) | (0<<CS11) | (1<<CS10);
	TCCR1B = 0b00011001; // CS가 분주비1, WGM이 fast pwm 모드

	TCNT1H = 0x00;
	TCNT1L = 0x00;
	ICR1 = 800;               //TOP

	OCR1AH = 0x00;            //PWM1 OUTPUT    15PIN OC1A
	OCR1AL = 0x01;

	OCR1BH = 0x00;            //PWM2 OUTPUT    16PIN OC1B
	OCR1BL = 0x01;

	OCR1CH = 0x00;            //PWM3 OUTPUT    17PIN OC1C
	OCR1CL = 0x01;

	EIMSK = 0x70;       //외부인터럽트 4,5,6 활성화
	EICRB = 0x14;       //외부인터럽트 트리거를 논리변화시에 즉 라이징,폴링 둘다 인터럽트 발생

	ACSR = 0x80; //아날로그 비교기를 비활성화
	//아날로그 비교기는 두 아날로그 입력 사이의 전압 차이를 비교하는 기능을 수행합니다.
	//이 비교기는 ADC와 동일한 아날로그 입력 핀을 공유하므로, ADC를 사용하려면 비교기를 비활성화해야 합니다.
	SFIOR = 0x00; //특별한 기능을 비활성화
	//이렇게 하면 ADC의 자동 트리거 기능이나 아날로그 비교기의 입력 캡처 기능 등과 같은 특수 기능이 비활성화됩니다.
	//ADC를 정상적으로 사용하기 위해 이러한 특수 기능들을 비활성화하는 것이 좋습니다.
	ADMUX = 0x00; //AREF 단자에 공급되는 전압 이용, 데이터 우측부터 저장, adc0번 핀을 사용
	ADCSRA = 0x87; //adc활성화 이게 8임, 뒤에7는 adc 분주비 128로 설정임.
	// 기준전압과 adc채널설정
	// 나는 16MHZ/128 이라 125KHZ임 주기론 8us임


	LCD_Init();
	LCD_Clear();
	LCD_wBCommand(0x80);
	LCD_wString(str_first1);
	_delay_ms(50);
	LCD_wBCommand(0xC0);
	LCD_wString(str_first2);
	_delay_ms(1000);
	_delay_ms(1000);

	LCD_Clear();

	LCD_wBCommand(0x80);
	LCD_wString(str_title1);
	_delay_ms(50);
	LCD_wBCommand(0xC0);
	LCD_wString(str_title2);
	_delay_ms(2000);
	LCD_Clear();

	LCD_wBCommand(0x80);
	LCD_wString(str_start1);
	LCD_wBCommand(0xC0);
	LCD_wString(str_start2);

	direction = '+';
	old_key = 0;
	Flag = 0;
	
	while (1)
	{
		Hall_data = PINE & 0x70;
		//START 0X0E , CW 0X0D ,UP 0X0B ,DOWN 0X07
		new_key = PINB & 0x0F;    // 풀업저항이라 버튼 눌리면 0임 안눌렸을 때 1임
		if (new_key != old_key)        // if문없으면 예를들어 sw3누르는데 조금 길게 눌렀을 경우 여러번 눌리는 효과가 일어날수도 있음
		{                               // 채터링 방지하려고 했던거임.

			switch (new_key)
			{
				case 0x0E:              //스타트, 스탑 PB0이 눌렸으니 PB0은 0이 되서 0x0F-0x01=0x0E
				if (Flag == 0)   //초기에는 모터 안돌고 때는 인터럽트 꺼놓음
				{
					
					sei(); //타이머인터럽트든 외부인터럽트든 인터럽트허용
					//모터 시작하면 인터럽트 시작
					Flag = 1;
					k = 120;
					count = 0;
					for (j = 0; j < k; j++)
					{
						count = count + 1;
						MOTOR_OCR(count);
						_delay_us(10);
					}
					//위에 부분이 듀티비 설정하고 그값을 순차적으로 상승시킴
					if (direction == '+')
					{
						MOTOR_START_CW(Hall_data);
					}
					else if (direction == '-')
					{
						MOTOR_START_CCW(Hall_data);
					}
					
					//홀센서가 한번이라도 바뀌어야 인터럽트 실행되니 현재 홀센서 값을 기준으로
					//다음 스탭으로 설정 바꿔서 인터럽트 시작하게 함
					//이 이후에는 순차적으로 인터럽트 실행됨.

					LCD_Clear();
					pwm1_control(count);

					if (direction == '+')
					{
						LCD_wBCommand(0x80);
						LCD_wString("CW");
					}
					else
					{
						LCD_wBCommand(0x80);
						LCD_wString("CCW");
					}
					//여기 쪽은 LCD에 현재상태 표시하려고 이렇게 함.
				}

				else // 모터 정지시키려고 현재 인터럽트 켜져있으면 모터정지시키고 인터럽트 비활성화함.
				{
					Flag = 0;
					while (count > 0)
					{
						count = count - 1;
						MOTOR_OCR(count);
						_delay_us(1);
					}
					cli();                // 인터럽트 꺼버림
					TCCR1A = 0x02;                // ocr1a,b,c 연결 off 즉 연결 다 끊음
					PORTB = 0x0F;                // 스위치 아무것도 안눌린 상태
					LCD_Clear();
					LCD_wBCommand(0x80);
					LCD_wString(str_start1);
					LCD_wBCommand(0xC0);
					LCD_wString(str_start2);
				}
				break;

				case 0x0D:      //방향전환  PB1이 눌렸으니 두번째 비트 0됨
				//일단 모터 실행되서 인터럽트 실행되야 회전을하든 하니까
				
				if (direction == '+')
				{

					k = count;
					for (j = 0; j < k; j++)
					{
						count = count - 1;
						MOTOR_OCR(count);
						_delay_us(1);
					}
					count = 0;
					MOTOR_OCR(count);

					direction = '-';

					for (j = 0; j < k; j++)
					{
						count = count + 1;
						MOTOR_OCR(count);
						_delay_us(1);
					}
					LCD_Clear();
					pwm1_control(count);
					LCD_wBCommand(0x80);
					LCD_wString("CCW");


				}

				else
				{

					k = count;
					for (j = 0; j < k; j++)
					{
						count = count - 1;
						MOTOR_OCR(count);
						_delay_us(1);
					}
					count = 0;
					MOTOR_OCR(count);

					direction = '+';

					count = 0;
					for (j = 0; j < k; j++)
					{
						count = count + 1;
						MOTOR_OCR(count);
						_delay_us(1);
					}
					LCD_Clear();
					pwm1_control(count);
					LCD_wBCommand(0x80);
					LCD_wString("CW");
				}
				
				break;

				case 0x0B:  //up
				if (Flag == 1)
				{
					k = 40;
					if (count < 759)
					{
						for (j = 0; j < k; j++)
						{
							count = count + 1;
							MOTOR_OCR(count);
							_delay_us(1);
						}
						pwm1_control(count);
					}
				}
				break;

				case 0x07:  //down
				if (Flag == 1)
				{
					k = 40;
					if (count > 159)
					{
						for (j = 0; j < k; j++)
						{
							count = count - 1;
							MOTOR_OCR(count);
							_delay_us(1);
						}
						pwm1_control(count);
					}
				}
				break;
			}                // SWITCH문 끝
		}                    // if(new_key!=old_key)끝

		old_key = new_key;   // sw1누르면 new키가 0x0E되고 if(new_key!=old_key)조건만족하고 스위치문 start조건 만족해 실행후 끝나면new=old키가되어 old도 0x0E가 됨
		// 그리고나서 sw1누른거 떼면 new가 0x0F가되는데 new키가 바뀌어도 if문안의 switch문이 진행중이므로 상관없고 다끝나면 old도 0x0F가됨


		//모터에 흐르는 전류를 LCD에 표시하기 위해
		i++;

		if (Flag == 1)        //전류 표시, 과전류 때 처리루틴 작성됨.
		{
			if (i >= 200000)      //바로 LCD에 출력하지 않으려고
			{
				value = read_adc(); //adc0이용해서 값 입력받음
				sum = value * 4.8;
				i = 0;
				c = sum / 100;                     //100
				b = (sum % 100) / 10;                //10
				a = sum % 10;                      //1
				LCD_wBCommand(0xC3);
				LCD_wData(0x30 + c);
				LCD_wBCommand(0xC4);
				LCD_wData(0x30 + b);
				LCD_wBCommand(0xC5);
				LCD_wData(0x30 + a);
				_delay_us(20);
				LCD_wBCommand(0xC0);
				LCD_wString("DC");
				LCD_wBCommand(0xC6);
				LCD_wString("mA");
			}
			
			//여기는 측정되는 전류 LCD로 출력하려고

			if (c >= 9)     //900mA 이상일 때 과전류로 보고 부저실행하고 모터 정지시킴
			{
				//PORTG = 0x08;                // 부저 on
				//_delay_ms(200);
				//PORTG = 0x00;
				//_delay_ms(200);
				//PORTG = 0x08;
				//_delay_ms(200);
				//PORTG = 0x00;                // 부저 off

				cli();                // 인터럽트 off
				TCCR1A = 0x02;                // ocr1a,b,c 연결 off
				PORTB = 0x0F;                // 스위치 아무것도 안눌린 상태
				LCD_Clear();
				LCD_wBCommand(0x80);
				LCD_wString(str_overcurrent1);
				LCD_wBCommand(0xC0);
				LCD_wString(str_overcurrent2);
				_delay_ms(2000);

				LCD_Clear();
				LCD_wBCommand(0x80);
				LCD_wString(str_wait1);
				LCD_wBCommand(0xC0);
				LCD_wString(str_wait2);
				_delay_ms(2000);

				LCD_Clear();
				LCD_wBCommand(0x80);
				LCD_wString(str_start1);
				LCD_wBCommand(0xC0);
				LCD_wString(str_start2);

			}



		}

		if ((PINE & 0x70) == 0x70)               //커넥터 빠졌을때 처리루틴 홀센서가 111임
		{
			cli();
			count = 0;
			MOTOR_OCR(count);
			//
			//PORTG = 0x08;
			//_delay_ms(200);
			//PORTG = 0x00;
			//_delay_ms(200);
			//PORTG = 0x08;
			//_delay_ms(200);
			//PORTG = 0x00;

			while ((PINE & 0x70) == 0x70)
			{
				LCD_Clear();
				LCD_wBCommand(0x80);
				LCD_wString(str_disconnect1);
				LCD_wBCommand(0xC0);
				LCD_wString(str_disconnect2);
				_delay_ms(2000);
			}
			LCD_wBCommand(0x80);
			LCD_wString(str_wait1);
			LCD_wBCommand(0xC0);
			LCD_wString(str_wait2);
			_delay_ms(2000);
			LCD_Clear();
			LCD_wBCommand(0x80);
			LCD_wString(str_start1);
			LCD_wBCommand(0xC0);
			LCD_wString(str_start2);
		}

	}                     // while(1)끝

}                    // void main()끝
