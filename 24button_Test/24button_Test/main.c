/*
 * 24button_Test.c
 *
 * Created: 2020-08-29 오후 5:49:18
 * Author : CHOI
 */ 


/*
*	버튼 24개 입력을 동시에 받는 코드
*
*/

/*
*
*
*	배경
*		PCM을 통해 효과음을 만들어냈으며, 이를 함수로 만들었다.
*		즉, 예를들어 PCM_C_ON(); 뭐 이런 함수 하나가 호출되면 스피커를 통해 피아노 '도' 소리가 발생한다.
*		->이 함수를 한번만 호출되어야 한다. 
*	문제점
*		-버튼을 누르고 봤더니, 누르는 동안에 계속 PCM 출력 함수가 호출되니 원하는 소리가 안나고 이상한 소리가 난다.
*	
*	해결방안
*		-버튼입력을 저장한 배열을 활용하여, rising edge 신호를 검출할 수 있는 코드를 구현해줘야 한다.
*	
*	테스트 방법
		-각 버튼을 누를때 특정 변수가 1씩 업 카운팅 되는 코드를 구현해보자.
*	===========================================================================================================
*	
*	배경
		위 문제점을 해결하고자 rising edge 신호를 검출하는 코드를 모두 작성하여 테스트해보니, 버튼을 누르면 1씩 카운팅이 되는 것을 확인했다.
		그런데, 간헐적으로 2~3씩 카운팅되는 것이 확인이 된다.
	문제점
		->버튼을 눌렀을 때 아주 짧은 순간 동안 기계적으로 진동하는 현상(채터링 현상)이 발생하여 순간적으로 rising edge가 연속적으로 인식이 된다.
	해결방안
		-rising edge가 처음 검출되면 tick 타이머를 통해 최소 10ms동안은 edge신호를 검출하지 않는 방식의 코드를 구현해줘야 한다. 
*			 
*
*/

#include <avr/io.h>

volatile uint32_t ticks=0;
//타이머0에 대한 비교매치 인터럽트
ISR(TIMER0_COMP_vect)
{
	ticks++;
}


int main(void)
{
	//
	DDRA=0x00;
	DDRC=0x00;
	DDRF=0x00;
		
	unsigned char A=0;	
	unsigned char C=0;
	unsigned char F=0;	
	unsigned char arr[3] = {0,0,0};
	unsigned char buffer =0;
	unsigned char edge_flag[3][8]={0,};
    /* Replace with your application code */
	uart_init(0,BAUD_9600);
	
    while (1) 
    {
		
		//A = buffer^0xff;
		//우리가 원하는 값 0x00
		//실제로 들어오는 값 0xff
		// >> 비트 반전시켜줘야 함
		
		// tick timer (혹은 system timer) 1ms 1씩 증가되는 변수 
		//Timer/counter : 
		if(ticks%10==0) 
		{//10ms 마다 갱신이 된다.
			buffer = PINA; 
			A = buffer ^ (0xff); 
			buffer = PINC;
			C = buffer ^ (0xff);
			buffer = PINF; 
			F = buffer ^ (0xff);
		}
		
		//data^=0x01; ==>> data = data ^ (0b00000001) 
// 		A = PINA;  
// 		C = PINC;
// 		F = PINF;
		//해당 값들은 10ms마다 외부 핀 상태로부터 갱신된다.
		arr[0] = A;
		arr[1] = C;
		arr[2] = F;
		// arr = { 0b(pa7...pa0),0b(pc7...pc0),0b(pf7...pf0)
		for(int i=0; i<3; i++){//각각 A,C,F 그룹
			for(int j=0; j<8; j++){//그룹 내에서 0~7비트 데이터를 가져오기 위함.
					buffer = ((arr[i]>>j)&0x01);
					if(buffer==1){
						if(edge_flag[i][j]==0){
							edge_flag[i][j]=1;
							//edge triggered.
								//해당 구문으로 정상적으로 들어오는지 체크 
								uart0_tx_string(IntToString(i));
								uart0_tx_char(',');
								uart0_tx_string(IntToString(j));
								uart0_tx_char('\n');
						}
					}
					else{
						edge_flag[i][j]=0;
					}
			}
		}
    }
}

