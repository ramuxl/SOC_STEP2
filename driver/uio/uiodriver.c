/*
* Simple app to read/write into custom IP in PL via /dev/uoi0 interface
* To compile for arm: make ARCH=arm CROSS_COMPILE=arm-xilinx-linux-gnueabi-
* Author: Tsotnep, Kjans
*/
  
//C
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdint.h>
#include "udpclient.h"
#include "ZedboardOLED.h"
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#include <string.h>
#include <termios.h>

#define BYTES_TO_RECEIVE 256
#define BYTES_TO_READ 4
#define TEMP_FIFO "/temp.fifo"
#define BROADCAST_ADDRESS "10.255.255.255"
#define BROADCAST_PORT 7891
#define BUFFER_SIZE 256
#define VOL_LEVELS 14

//control keys
#define UP_KEY 1
#define DOWN_KEY 2
#define RIGHT_KEY 3
#define LEFT_KEY 4
#define F1_KEY 5
#define F2_KEY 6
#define F3_KEY 7
#define ZERO_KEY 8
#define QUIT 9
  
#define GPIO_FILE "/sys/class/gpio/gpio224/value"
#define REG_AXI_0   ((unsigned *)(ptr0 + 0))

#define REG_FIL_NET_0   *((unsigned *)(ptr3 + 0))
#define REG_FIL_NET_1   *((unsigned *)(ptr3 + 4))
#define REG_FIL_NET_2   *((unsigned *)(ptr3 + 8))
#define REG_FIL_NET_3   *((unsigned *)(ptr3 + 12)) 
#define REG_FIL_NET_4   *((unsigned *)(ptr3 + 16))
#define REG_FIL_NET_5   *((unsigned *)(ptr3 + 20))
#define REG_FIL_NET_6   *((unsigned *)(ptr3 + 24)) 
#define REG_FIL_NET_7   *((unsigned *)(ptr3 + 28))
#define REG_FIL_NET_8   *((unsigned *)(ptr3 + 32))
#define REG_FIL_NET_9   *((unsigned *)(ptr3 + 36)) 
#define REG_FIL_NET_10  *((unsigned *)(ptr3 + 40))
#define REG_FIL_NET_11  *((unsigned *)(ptr3 + 44))
#define REG_FIL_NET_12  *((unsigned *)(ptr3 + 48)) 
#define REG_FIL_NET_13  *((unsigned *)(ptr3 + 52))
#define REG_FIL_NET_14  *((unsigned *)(ptr3 + 56))
#define REG_FIL_NET_15  *((unsigned *)(ptr3 + 60)) 
#define REG_FIL_NET_16  *((unsigned *)(ptr3 + 64))


#define REG_FIL_NET_17  ((unsigned *)(ptr3 + 68))
#define REG_FIL_NET_18  ((unsigned *)(ptr3+ 72)) 
#define REG_FIL_NET_19  ((unsigned *)(ptr3 + 76))


#define REG_VOL_NET_0  ((unsigned *)(ptr4 + 0))
#define REG_VOL_NET_1  ((unsigned *)(ptr4 + 4)) 


#define REG_FIL_LINE_0   *((unsigned *)(ptr1 + 0))
#define REG_FIL_LINE_1   *((unsigned *)(ptr1 + 4))
#define REG_FIL_LINE_2   *((unsigned *)(ptr1 + 8))
#define REG_FIL_LINE_3   *((unsigned *)(ptr1 + 12)) 
#define REG_FIL_LINE_4   *((unsigned *)(ptr1 + 16))
#define REG_FIL_LINE_5   *((unsigned *)(ptr1 + 20))
#define REG_FIL_LINE_6   *((unsigned *)(ptr1 + 24)) 
#define REG_FIL_LINE_7   *((unsigned *)(ptr1 + 28))
#define REG_FIL_LINE_8   *((unsigned *)(ptr1 + 32))
#define REG_FIL_LINE_9   *((unsigned *)(ptr1 + 36)) 
#define REG_FIL_LINE_10  *((unsigned *)(ptr1 + 40))
#define REG_FIL_LINE_11  *((unsigned *)(ptr1 + 44))
#define REG_FIL_LINE_12  *((unsigned *)(ptr1 + 48)) 
#define REG_FIL_LINE_13  *((unsigned *)(ptr1 + 52))
#define REG_FIL_LINE_14  *((unsigned *)(ptr1 + 56))
#define REG_FIL_LINE_15  *((unsigned *)(ptr1 + 60)) 
#define REG_FIL_LINE_16  *((unsigned *)(ptr1 + 64))

#define REG_FIL_LINE_17  ((unsigned *)(ptr1 + 68))
#define REG_FIL_LINE_18  ((unsigned *)(ptr1+ 72)) 
#define REG_FIL_LINE_19  ((unsigned *)(ptr1 + 76))

#define REG_VOL_LINE_0  ((unsigned *)(ptr2 + 0))
#define REG_VOL_LINE_1  ((unsigned *)(ptr2 + 4)) 


#define REG_OLED_0 *((unsigned *)(ptr1 + 0))

//function prototyeps
void *ReceiveAudio(void *);
void *ReadAudio(void *);
void *CLIThread_Func(void *);
void *RdSwitchesFunc(void *);
void Set_Filter(unsigned *, unsigned * , unsigned *);
void Set_Volume (unsigned *, unsigned *);
int getch(void);
int Get_Ctrl_Key(void);

typedef struct thread_arg{
	const char * fifo_file;
	unsigned*  mem_pointer;
} my_thread_arg;

typedef struct p_params{
	unsigned * VolumenRNet;
	unsigned * VolumenLNet;
	unsigned * Filter0Net;
	unsigned * Filter1Net;
	unsigned * Filter2Net;
	unsigned * VolumenRline;
	unsigned * VolumenLline;
	unsigned * Filter0line;
	unsigned * Filter1line;
	unsigned * Filter2line;
	unsigned * OLED;
} p_user_thread_arg;


//global varible to exit program
bool call_to_exit = false;

int main(int argc, char *argv[])
{
        //open dev/uio0 AXI BUS
        int fd0 = open ("/dev/uio0", O_RDWR);
        if (fd0 < 1) { perror(argv[0]); return -1; }
        
        //open FILTER LINE IN  -> uio1
		int fd1 = open ("/dev/uio1", O_RDWR);
		if (fd1 < 1) { perror(argv[0]); return -1; }
		
		//open VOLUME LINE IN  -> uio2
		int fd2 = open ("/dev/uio2", O_RDWR);
		if (fd2 < 1) { perror(argv[0]); return -1; }
		
		//open dev/uio3 FILTER NETWORK
        int fd3 = open ("/dev/uio3", O_RDWR);
        if (fd3 < 1) { perror(argv[0]); return -1; }
		//open dev/uio4 VOLUME NETWORK
        int fd4 = open ("/dev/uio4", O_RDWR);
        if (fd4 < 1) { perror(argv[0]); return -1;}
 
	//OPEN /dev/ui05 OLED	
	int fd5 = open ("/dev/uio5", O_RDWR);
        if (fd5 < 1) { perror(argv[0]); return -1;}
	
	
	//if (fd6 < 1) {perror (argv[0]); return -1;}	

        //Redirect stdout/printf into /dev/kmsg file (so it will be printed using printk)
        //freopen ("/dev/kmsg","w",stdout);
  
        //get architecture specific page size
        unsigned pageSize = sysconf(_SC_PAGESIZE);
  
        //Map virtual memory to physical memory         
        void *ptr0;
        ptr0 = mmap(NULL, pageSize, (PROT_READ |PROT_WRITE), MAP_SHARED, fd0, pageSize*0);
        
                
        void *ptr1;
        ptr1 = mmap(NULL, pageSize, (PROT_READ |PROT_WRITE), MAP_SHARED, fd1, pageSize*0);
        
             
        void *ptr2;
        ptr2 = mmap(NULL, pageSize, (PROT_READ |PROT_WRITE), MAP_SHARED, fd2, pageSize*0);
        
         
        void *ptr3;
        ptr3 = mmap(NULL, pageSize, (PROT_READ |PROT_WRITE), MAP_SHARED, fd3, pageSize*0);
                
        void *ptr4;
        ptr4 = mmap(NULL, pageSize, (PROT_READ |PROT_WRITE), MAP_SHARED, fd4, pageSize*0);
        
	void *ptr5;
        ptr5 = mmap(NULL, pageSize, (PROT_READ |PROT_WRITE), MAP_SHARED, fd5, pageSize*0);

	/*write something in OLED*/
	oled_clear(ptr5);
	char str[] = "Welcome SOC TTU";
	if (!oled_print_message(str, 0, ptr5)){
		perror("Error: Failed to write in the OLED\n");
		exit(-1);
	}
	char str2[] = "**AUDIO MIXER**";
	if (!oled_print_message(str2, 2, ptr5)){
		perror("Error: Failed to write in the OLED\n");
		exit(-1);
	}
        
        
         /* write coeffients in both filters */
        REG_FIL_NET_0 = 0x00002CB6;
        REG_FIL_NET_1 = 0x0000596C;
        REG_FIL_NET_2 = 0x00002CB6;
        REG_FIL_NET_3 = 0x8097A63A;
        REG_FIL_NET_4 = 0x3F690C9D;
        REG_FIL_NET_5 = 0x074D9236;
        REG_FIL_NET_6 = 0x00000000;
        REG_FIL_NET_7 = 0xF8B26DCA;
        REG_FIL_NET_8 = 0x9464B81B;
        REG_FIL_NET_9 = 0x3164DB93;
        REG_FIL_NET_10 = 0x12BEC333;
        REG_FIL_NET_11 = 0xDA82799A;
        REG_FIL_NET_12 = 0x12BEC333;
        REG_FIL_NET_13 = 0x00000000;
        REG_FIL_NET_14 = 0x0AFB0CCC;
        
        REG_FIL_LINE_0 = 0x00002CB6;
        REG_FIL_LINE_1 = 0x0000596C;
        REG_FIL_LINE_2 = 0x00002CB6;
        REG_FIL_LINE_3 = 0x8097A63A;
        REG_FIL_LINE_4 = 0x3F690C9D;
        REG_FIL_LINE_5 = 0x074D9236;
        REG_FIL_LINE_6 = 0x00000000;
        REG_FIL_LINE_7 = 0xF8B26DCA;
        REG_FIL_LINE_8 = 0x9464B81B;
        REG_FIL_LINE_9 = 0x3164DB93;
        REG_FIL_LINE_10 = 0x12BEC333;
        REG_FIL_LINE_11 = 0xDA82799A;
        REG_FIL_LINE_12 = 0x12BEC333;
        REG_FIL_LINE_13 = 0x00000000;
        REG_FIL_LINE_14 = 0x0AFB0CCC;
        
        //set reset for NET filter device to zero
        REG_FIL_NET_15 = 0;
        //set REG16 for LINE to constant 1
        REG_FIL_NET_16 = 1;
        
        
        
        
        //set reset for LINE filter device to zero
        REG_FIL_LINE_15 = 0;
        //set REG16 for LINE to constant 1
        REG_FIL_LINE_16 = 1;
        
      //Initialize volume and filters for NET and LINE
        *REG_VOL_NET_0 = 256;
        *REG_VOL_NET_1 = 256;
 
		*REG_FIL_NET_17 = 0;
		*REG_FIL_NET_18 = 0;
		*REG_FIL_NET_19 = 0;
		
        *REG_VOL_LINE_0 = 256;
        *REG_VOL_LINE_1 = 256;
        
        *REG_FIL_LINE_17 = 0;
        *REG_FIL_LINE_18 = 0;
        *REG_FIL_LINE_19 = 0;
        
	    //Initilize CLI thread structure values
        p_user_thread_arg prmtr;
        prmtr.VolumenRNet = REG_VOL_NET_0;
		prmtr.VolumenLNet = REG_VOL_NET_1;
        
        prmtr.Filter0Net  = REG_FIL_NET_17;
        prmtr.Filter1Net  = REG_FIL_NET_18;
        prmtr.Filter2Net  = REG_FIL_NET_19;
        
        prmtr.VolumenRline = REG_VOL_LINE_0;
        prmtr.VolumenLline = REG_VOL_LINE_1;
        
        prmtr.Filter0line = REG_FIL_LINE_17;
        prmtr.Filter1line = REG_FIL_LINE_18;
        prmtr.Filter2line = REG_FIL_LINE_19;

	//prmtr.OLED =REG_OLED_0;

	//prmtr.SwitchF = fd6;
        
        
        //setup socket
        if (udp_client_setup (BROADCAST_ADDRESS, BROADCAST_PORT) == 1){
			perror("Error with UDP setup");
			exit(-1);
		}
          
        //Create FIFO
        const char * my_fifo;
		my_fifo = strcat (getenv("HOME"), TEMP_FIFO);
		if (access(my_fifo, F_OK) != -1){
			remove(my_fifo);
		}
        if(mkfifo(my_fifo, S_IRUSR| S_IWUSR) != 0){
			 perror("mkfifo() error\n");
			 exit (-1);
		}
		
		//Initialize Read thread structure values
		my_thread_arg read_thread_arg;   
		read_thread_arg.mem_pointer = REG_AXI_0;
        read_thread_arg.fifo_file = my_fifo;
        
        //create threads
		 pthread_t ReceiveThread;
		 pthread_t ReadThread;
		 pthread_t ReadSwitches;
		 pthread_t CLIThread;
		 int rc;
		 rc = pthread_create(&ReceiveThread, NULL, ReceiveAudio,(void *)my_fifo);
		 if (rc){
			 perror("Error: Failed to create the ReceiveThread with retrun code\n");
			 exit(-1);
		 }
		 rc = pthread_create(&ReadThread, NULL, ReadAudio,(void *)&read_thread_arg);
		 if (rc){
			 perror("Error: Failed to create the ReadThread with retrun code\n");
			 exit(-1);
		 }
         rc = pthread_create(&CLIThread, NULL, CLIThread_Func,(void *)&prmtr);
		 if (rc){
			 perror("Error: Failed to create the CLIThread with retrun code\n");
			 exit(-1);
		 }   
		rc = pthread_create(&ReadSwitches, NULL, RdSwitchesFunc, (void *)&prmtr);
		  if (rc){
			 perror("Error: Failed to create the ReadSwitches with retrun code\n");
			 exit(-1);
		 } 
		pthread_join(ReceiveThread, NULL);
		pthread_join(ReadThread, NULL);
		pthread_join(CLIThread,NULL);
        //unmap
        munmap(ptr0, pageSize);
        munmap(ptr1, pageSize);
        munmap(ptr2, pageSize);
        munmap(ptr3, pageSize);
        munmap(ptr4, pageSize);
        
  
        //close
        fclose(stdout);
        unlink(my_fifo);
        pthread_exit(NULL);
        return 0;
}

void *ReceiveAudio (void *tempfifo)
{
	const char *fifo;
	fifo = (char *) tempfifo;
	int wfd, bytes_written;
	unsigned buffer[BUFFER_SIZE];
	wfd = open (fifo, O_WRONLY);
	if (wfd < 0){
			perror("open() error for write end");
			return;
	}
	while (!call_to_exit){
		
		if (udp_client_recv (buffer, BYTES_TO_RECEIVE) == 1){;//receive 256 bytes from UDP to buffer
			perror("Error receiving from UDP");
			close(wfd);
			return;
		}
		if ((bytes_written = write (wfd, buffer, BYTES_TO_RECEIVE) )== -1){ //Write 256 bytes from buffer to FIFO
			perror("write () error");
			close(wfd);
			return;
		}
		
	}
	close(wfd);
	pthread_exit(NULL);
	return;
}

void *ReadAudio(void *thread_arg)
{
	my_thread_arg *func_data;
	func_data = (my_thread_arg *) thread_arg;
	const char *fifo = func_data -> fifo_file;
	int rfd, bytes_read;
	rfd = open(fifo, O_RDONLY);
	if(rfd < 0){
			perror("open() error for read end");
			return;			
	} 
	while (!call_to_exit){
		if((bytes_read =read(rfd, func_data -> mem_pointer, BYTES_TO_READ)) == -1){//Write 4  bytes to REG_AXI_0
			perror("read() error");
			close(rfd);
			return;
		}
	}
	
	close(rfd);
	pthread_exit(NULL);
	return;
}

void *CLIThread_Func(void * p_register){
	p_user_thread_arg * prmt;
	prmt = (p_user_thread_arg *) p_register;
	int Control, ch;
    while (true){
		printf("*****************************************\n");
		printf("*\t\tMAIN MENU\t\t*\n");
		printf("*****************************************\n");
		printf("/*Enter*/\n");
		printf("0. Exit program\n");
		printf("1. Control volume for network\n");
		printf("2. Control volume for Line in\n");
		printf("3. Control filter for network\n");
		printf("4. Control filter for Line in\n");
		printf("\nUser_Input/>");
		fflush(stdin);
		scanf(" %d",&Control);
		switch (Control){
			case 0:
				getchar();
				while (true){
					printf("Are you sure you want to exit? y or n: ");
					ch = getchar();
					if(getchar() != '\n'){
						printf("Invalid input!!\n");
						while(getchar() != '\n');
						continue;
						
					}
					else if( ch == 'n' || ch == 'N' || ch == 'y' || ch == 'Y')
						break;
					printf("Invalid input!!\n");
				}
				if(ch == 'n' || ch == 'N')
					break;
				call_to_exit = true;
				pthread_exit(NULL);
				return;
			case 1:
				Set_Volume(prmt->VolumenRNet, prmt->VolumenLNet);
				break;
			case 2:
				Set_Volume(prmt->VolumenRline, prmt->VolumenLline);
				break;
			case 3:
				Set_Filter(prmt->Filter0Net, prmt->Filter1Net, prmt->Filter2Net);
				break;
			case 4:
				Set_Filter(prmt->Filter0line, prmt->Filter1line, prmt->Filter2line);
				break;
			default: //Notify unsupported control
				printf("Unsupported control specified\n");
				break;
		}
    }
     pthread_exit(NULL);
}
void Set_Volume(unsigned *Vol_R, unsigned *Vol_L){
	
	
	int ch, i, r_vol, l_vol;
	unsigned vol_levels[VOL_LEVELS]  = {0, 16, 32, 64, 128, 256, 512, 1024, 1536, 2048, 2560, 3072, 3584, 4096};
	//Determine current volume level for right and left
	for (i = 0; i < VOL_LEVELS; i++){
		if (*Vol_R == vol_levels[i])
			r_vol = i;
		if(*Vol_L == vol_levels[i])
			l_vol = i;
	}
	//print usage
	printf("/*Use the arrow keys as follow*/ \n");
	printf("*UP KEY - R Volume increase\n");
	printf("*DOWN KEY - R Volume decrease\n");
	printf("*RIGHT KEY - L Volume increase\n");
	printf("*LEFT KEY - L Voulme decrease\n");
	printf("*0 mute both L & R\n");
	printf("Q to go back to main menu\n");
	getchar();
	while((ch = Get_Ctrl_Key()) != QUIT){
		switch (ch){
			case UP_KEY:
				if(r_vol != VOL_LEVELS -1){//if maximum volume is not reached.
					*Vol_R = vol_levels[++r_vol];
				}
				break;
			case DOWN_KEY:
				if(r_vol != 0){//if minimum volume is not reached
					*Vol_R = vol_levels[--r_vol];
				}
				break;
			case RIGHT_KEY:
				if(l_vol != VOL_LEVELS -1){//if maximum volume is not reached.
					*Vol_L = vol_levels [++l_vol];
				}
				break;
			case LEFT_KEY:
				if (l_vol != 0){//if minimum volume is not reached
					*Vol_L = vol_levels[--l_vol];
				}
				break;
			case ZERO_KEY:
				l_vol = 0;
				r_vol = 0;
				*Vol_L = vol_levels[l_vol];
				*Vol_R = vol_levels[r_vol];
				break;
			default:
				printf("Wrong key pressed!. Use Q to return to main menu.\n");
		}
	}
	return;
}
void Set_Filter (unsigned *high_pass, unsigned *band_pass, unsigned *low_pass){
	
	int ch;
	//print usage
	printf("/*Use the function keys as follow*/ \n");
	printf("F1 to enable or disable high pass\n");
	printf("F2 to enable or disable band pass\n");
	printf("F3 to enable or disable low pass\n");
	printf("0 to display filter status\n");
	printf("Q to go back to main menu\n");
	getchar();
	while ((ch = Get_Ctrl_Key()) != QUIT){
		switch(ch){
			case F1_KEY:
				*high_pass = !(*high_pass);
				printf("High pass %s\n", (*high_pass == 0)? "enabled" : "disabled");
				break;
			case F2_KEY:
				*band_pass = !(*band_pass);
				printf("Band pass %s\n", (*band_pass == 0)? "enabled" : "disabled");
				break;
			case F3_KEY:
				*low_pass = !(*low_pass);
				printf("Low pass %s\n", (*low_pass == 0)? "enabled" : "disabled");
				break;
			case ZERO_KEY:
				printf("\n************Status*****************\n");
				printf("High pass filter is %s\n", (*high_pass == 0)? "enabled" : "disabled");
				printf("Band pass filter is %s\n", (*band_pass == 0)? "enabled" : "disabled");
				printf("Low pass filter is %s\n",  (*low_pass == 0)?  "enabled" : "disabled");
				break;
			default:
				printf("Wrong key pressed. Use Q to return to main menu!\n");				
		}
	}
	return;
}
int getch(void)
{
	 int ch;
	 struct termios oldt;
	 struct termios newt;
	 tcgetattr(STDIN_FILENO, &oldt); /*store old settings */
	 newt = oldt; /* copy old settings to new settings */
	 newt.c_lflag &= ~(ICANON | ECHO); /* make one change to old settings in new settings */
	 tcsetattr(STDIN_FILENO, TCSANOW, &newt); /*apply the new settings immediatly */
	 ch = getchar(); /* standard getchar call */
	 tcsetattr(STDIN_FILENO, TCSANOW, &oldt); /*reapply the old settings */
	 return ch; /*return received char */
}
int Get_Ctrl_Key(void){

    int x, y, z;
    x = getch();
    if (x == 'q'|| x == 'Q')
        return QUIT;
	if (x == '0')
		return ZERO_KEY;
	if (x == '1')
		return F1_KEY;
	if (x == '2')
		return F2_KEY;
	if (x == '3')
		return F3_KEY;

    if (x == 27){
        y = getch();
        z = getch();
    }

    if (x == 27 && (y == 91 || y == 79))
    {
        switch(z){
            case 65:
                return UP_KEY;
            case 66:
                return DOWN_KEY;
            case 67:
                return RIGHT_KEY;
            case 68:
                return LEFT_KEY;
            
        }
    }
    return -1;
}
void * RdSwitchesFunc(void * p_register){
	p_user_thread_arg * prmt;		
	prmt = (p_user_thread_arg *) p_register;
	

	/*FILES*/
	FILE* fd6;
	int readvalue, oldvalue;

	/*SOUND IN NETWORK*/
	FILE* fsl;
	int Vsnc=-1,NBool=0;
	int TemVL,TemVR;
	
	/*SOUND IN LINE*/
	FILE* fsn;
	int Vslc=-1,LBool=0;
	int TemVLL,TemVRL;

			
	while (true){

		fd6 = fopen ("/sys/class/gpio/gpio224/value", "r");
		fsl = fopen ("/sys/class/gpio/gpio230/value", "r");
		fsn = fopen ("/sys/class/gpio/gpio231/value", "r");
		
		//prmt->SwitchF = fopen(GPIO_FILE, "r");
		
		/* HIGH PASS */
		fscanf(fd6, "%d", &readvalue);
		fscanf(fsl, "%d", &Vslc);
		fscanf(fsn, "%d", &Vsnc);
		//read(*(prmt->SwitchF), &readvalue, sizeof(readvalue));
		//fgets(&readvalue, 4, prmt->SwitchF);
		//printf("Value %d\n", readvalue);
		if(readvalue != oldvalue){
			//printf("Value changed to %d\n", readvalue);
			*prmt->Filter2line = readvalue;
			
		}
		/* TURN OFF SOUND NETWORK*/
		if(Vsnc == 0 ){
			//printf("Value SOUND NETWORK to %d\n", Vsnc);
			TemVL=*prmt->VolumenLNet;
			TemVR=*prmt->VolumenRNet;

			*prmt->VolumenRNet = Vsnc;
			*prmt->VolumenLNet = Vsnc;
			LBool=0;
			
		}
		if(Vsnc == 1 ){
			//printf("Value SOUND NETWORK to %d\n", Vsnc);
			//TemVL=*prmt->VolumenLNet;
			//TemVR=*prmt->VolumenRNet;
			if (LBool==0){
				*prmt->VolumenRNet = TemVR;
				*prmt->VolumenLNet = TemVL;
			}
		}
	
		/* TURN OFF SOUND LINE*/
		if(Vslc == 0 ){
			//printf("Value SOUND LINE to %d\n", Vslc);
			TemVLL=*prmt->VolumenLline;
			TemVRL=*prmt->VolumenRline;

			*prmt->VolumenLline = Vsnc;
			*prmt->VolumenRline = Vsnc;
			NBool=0;
			
		}
		if(Vslc == 1 ){
			//printf("Value SOUND LINE to %d\n", Vslc);
			//TemVL=*prmt->VolumenLNet;
			//TemVR=*prmt->VolumenRNet;
			if (NBool== 0){
				*prmt->VolumenRline = TemVRL;
				*prmt->VolumenLline = TemVLL;
				NBool++;
			}
		}


		oldvalue = readvalue;
		

		fclose(fd6);
		fclose(fsl);
		fclose(fsn);
	}
	

	
}
