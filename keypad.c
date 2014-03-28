#include "keypad.h"

#ifdef _NO_KEYPAD_
void get_key(unsigned int *port, unsigned int *key) {
	*key = 1;
}
#else
void get_key(volatile unsigned int *port, unsigned int *key) {
  int keyval = 0;
  int temp_key = 0;
  int flag = 0;
  volatile pwm_Speed = 0x36;
  int delayVal = 70000;

    while (1) {
		keyval = 0x0;
		*port = 0x0000000E<<5;

		usleep(1000);

		temp_key = (*port & 0x000001eF);
		if ((keyval != temp_key) && ((temp_key &0x0000000F) < 0xF)){
			keyval = temp_key;
			flag = 1;
		}
			*port = 0x0000000D<<5;

		usleep(1000);

		temp_key = (*port & 0x000001eF);
		if ((keyval != temp_key) && ((temp_key &0x0000000F) < 0xF)){
			keyval = temp_key;
			flag = 1;
		}

		*port = 0x0000000B<<5;

		usleep(1000);

		temp_key = (*port & 0x000001eF);
		if ((keyval != temp_key) && ((temp_key &0x0000000F) < 0xF)){
			keyval = temp_key;
			flag = 1;
		}
		
		*port = 0x00000007<<5;
		
		usleep(1);
		
		temp_key = (*port & 0x000001eF);
		if ((keyval != temp_key) && ((temp_key &0x0000000F) < 0xF)){
			keyval = temp_key;
			flag = 1;
		}

		if(flag) {
			*key = key;
			/*switch (keyval) {
				case 0x1CE:
					printf("-> %d\n",keyval);
					command(0x1); // clear display
					writechars("RESETTING SERVOS!");
					*pwm_A = 0x6F;
					*pwm_B = 0x6F;
					*pwm_C = 0x6F;
					*pwm_D = 0x6F;
					*pwm_A_enable = 0x1;
					*pwm_B_enable = 0x1;
					*pwm_C_enable = 0x1;
					*pwm_D_enable = 0x1;
					usleep(300000);
					*pwm_A_enable = 0x0;
					*pwm_B_enable = 0x0;
					*pwm_C_enable = 0x0;
					*pwm_D_enable = 0x0;
					break;
					
				case 0x1CD: 
					printf("<- %d\n",keyval);
					break;
					
				case 0x1CB: 
					printf("blank %d\n",keyval);
					command(0x1); // clear display
					writechars("BASE LEFT!");
					printf("Writing again lol\n");
					input_Key = BASE_LEFT_KEY;
					servo_Control();
					*pwm_A_enable = 0x1;
					usleep(delayVal);
					break;
					
				case 0x1C7: 
					printf(". %d\n",keyval);
					input_Key = BASE_RIGHT_KEY;
					servo_Control();
					*pwm_A_enable = 0x1;
					command(0x1); // clear display
					writechars("BASE RIGHT!");
					printf("Writing PWM Value 0x7F\n");
					usleep(delayVal);
					break;
					
				case 0x1AE: 
					printf("9 %d\n",keyval);
					break;
					
				case 0x1AD:
					printf("6 %d\n",keyval);
					break;
					
				case 0x1AB: *//** 8 on the keypad? *//*
					printf("3 %d\n",keyval);
					input_Key = ELBOW_DOWN_KEY;
					servo_Control();
					*pwm_B_enable = 0x1;
					command(0x1); // clear display
					writechars("ELBOW DOWN!");
					printf("Writing 2nd PWM Value 0x7F\n");
					usleep(delayVal);
					break;
					
				case 0x1A7: *//** 7 on the keypad? *//*
					printf("ent %d\n",keyval);
					input_Key = ELBOW_UP_KEY;
					servo_Control();
					*pwm_B_enable = 0x1;
					*pwm_B = duty_Cycles[i++];
					command(0x1); // clear display
					writechars("ELBOW UP!");
					//usleep(2500);
					usleep(delayVal);
					printf("Writing 2nd again lol\n");
					break;
					
				case 0x16E: 
					printf("8 %d\n",keyval);
					break;
					
				case 0x16D: 
					printf("5 %d\n",keyval);
					break;
					
				case 0x16B: *//** 5 on the keypad? *//* 
					printf("2 %d\n",keyval);
					input_Key = SHOULDER_UP_KEY;
					servo_Control();
					*pwm_C_enable = 0x1;
					command(0x1); // clear display
					writechars("SHOULDER UP!");
					usleep(delayVal);
					printf("Writing 3rd PWM Value 0x7F\n");
					break;
					
				case 0x167: *//** 4 on the keypad? *//*
					printf("0 %d\n",keyval);
					input_Key = SHOULDER_DOWN_KEY;
					servo_Control();
					*pwm_C_enable = 0x1;
					command(0x1); // clear display
					writechars("SHOULDER DOWN!");
					usleep(delayVal);
					printf("Writing 3rd again lol\n");
					break;
					
				case 0xEE: *//** A on the keypad? *//*
					printf("7 %d\n",keyval);
					command(0x1);
					writechars("SPEED DOWN!");
					delayVal -= 5000;
					printf("Decrementing pwm_Speed and sleeps %d %d\n",pwm_Speed,delayVal);
					break;
					
				case 0xED: *//** 3 on the keypad? *//*
					printf("4 %d\n",keyval);
					//3 on crappy keypad
					command(0x1); // clear display
					writechars("SPEED UP!");
					delayVal += 5000;
					break;
					
				case 0xEB: *//** 2 on the keypad? *//*
					printf("1 %d\n",keyval);
					input_Key = GRIPPER_CLOSE_KEY;
					servo_Control();
					*pwm_D_enable = 0x1;
					command(0x1); // clear display
					writechars("GRIPPER CLOSE!");
					usleep(delayVal);
					printf("Writing 4th PWM Value 0x7F\n");
					break;
					
				case 0xE7: *//** 1 on the keypad? *//*
					printf("clr %d\n",keyval);
					input_Key = GRIPPER_OPEN_KEY;
					servo_Control();
					*pwm_D_enable = 0x1;
					command(0x1); // clear display
					writechars("GRIPPER OPEN!");
					usleep(delayVal);
					printf("Writing 4th again lol\n");
					break;
			} */
			
			//flag = 0;
		}
	}
}
#endif