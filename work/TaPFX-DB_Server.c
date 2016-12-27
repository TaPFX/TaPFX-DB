#include "TaPFX-DB_Server.h"

//WolfON -> WolfRun=1; Wölfe blinzeln
//WolfOFF -> WolfRun=0; Wölfe aus

void *wolfThread(){
	int i;
	int color[3] = {0xFF1F05,0xAF2A00,0xDF3800};
	int setcolor;
	int lednum = 0;
	int ledcolor = 0;
	
	int sleeptime = 2000000;
	int sleepmin = SLEEPMIN;
	int sleepmax = SLEEPMAX;

	printf("Start Wolf effect\n");

	for(i=0;i<(LEDANZ-1);i=i+2){
		setcolor = color[rand()%3];
		setLED(i,setcolor);
		setLED(i+1,setcolor);
		if(debug == 1)
			printf("led:%d,%d color:%d\n",i,i+1,setcolor);
		}
			
	while(1){	
		while(sleeptime < sleepmin)
			sleeptime = rand()%sleepmax;

		lednum=rand() % (LEDANZ-1);
		if(lednum%2 != 0)
			lednum++;
		
		ledcolor = getLEDcolor(lednum);
		setLED(lednum,0);
		setLED(lednum+1,0);
		if(debug == 1)
			printf("lednum:%d,%d\n",lednum,lednum+1);
		usleep(BLINKTIME);
		setLED(lednum,ledcolor);
		setLED(lednum+1,ledcolor);
		
		for(i=0;i<1000;i++){
			pthread_mutex_lock(&mutexThreadRun);
			if(!WolfThreadRun) {
				pthread_mutex_unlock(&mutexThreadRun);
				printf("Wolf End\n");
				goto wolfend;}
			pthread_mutex_unlock(&mutexThreadRun);

			usleep(sleeptime/1000);
		}
	}
	wolfend:setLEDsOFF();
	printf("End Wolf effect\n");

	return;
}

void *starThread(){
	int i;
	int color[3] = {0xFFFFA5,0xCFFAFF,0x9FFFFF};
	int setcolor;
	
	printf("Start Star effect\n");

	for(i=0;i<(LEDANZ-1);i=i+2){
		setcolor = color[rand()%3];
		setLED(i,setcolor);
		if(debug == 1)
			printf("led:%d,%d color:%d\n",i,i+1,setcolor);
		}
	
	//Ausschalten spezieller LEDS
	setLED(22,0);
	setLED(8,0);
	
	while(1){
		pthread_mutex_lock(&mutexThreadRun);
		if(!StarThreadRun) {
			pthread_mutex_unlock(&mutexThreadRun);
			break;}
		pthread_mutex_unlock(&mutexThreadRun);
		
		usleep(100000);
	}
	setLEDsOFF();
	printf("End Star effect\n");
	return;
}

int main(int argc, char*argv[]){
	int i,rc;
	int brt=50;
	int opt;
	char recvmsg[BUFSIZE];
	int port = DEFAULTPORT;	
	int thrun = 0;

	pthread_t wolfPThread,starPThread;
	pthread_mutex_init(&mutexThreadRun, NULL);
	WolfThreadRun = 0;
	StarThreadRun = 0;

	if(initLEDs(LEDANZ)){
		printf("ERROR: initLED\n");
		return -1;}
		
	while ((opt = getopt(argc, argv, "p:dhb:")) != -1) {
		switch (opt) {
		case 'p':
			if(optarg != NULL)
				port = atoi(optarg);
			break;
		case 'd':
			debug = 1;
			break;		
		case 'b':
			if(optarg != NULL)
				brt = atoi(optarg);
			break;		
	
		case 'h':
			printf("Help for TaPWolfServer\n");
			printf("Usage: %s [-p portnumber] [-b brightness] [-d] debug mode [-i] invert pwm output [-h] show Help\n", argv[0]);
			return 0;
			break;
		default: /* '?' */
			printf("Usage: %s [-p portnumber] [-b brightness] [-d] debug mode [-i] invert pwm output [-h] show Help\n", argv[0]);
			return -2;
		}
	}
	
	if(setBrightness(brt)){
		printf("ERROR setBrightnes\n");
		return -1;}

	if(setLEDsOFF()){
		printf("ERROR setLEDsOFF\n");
		return -1;}


	if(initUDPServer(port) != 0){
		printf("ERROR while init UDP-Server\n");
		return -1;
	}	
	
	while(1){
		
		bzero(recvmsg, BUFSIZE);
		printf("Wait for command\n");
		waitForClient(recvmsg);
		
		if(parseCommand(recvmsg) != 0){
			printf("ERROR wrong Syntax\n");
			continue;
		}

		if(strcmp(Mode, "WolfON") == 0){
			pthread_mutex_lock(&mutexThreadRun);
				if(WolfThreadRun == 0 && StarThreadRun == 0)
					WolfThreadRun = 1;
				else{
					printf("An effect is already running\n");
					pthread_mutex_unlock(&mutexThreadRun);
					goto mainloop;}
			pthread_mutex_unlock(&mutexThreadRun);

			rc = pthread_create( &wolfPThread, NULL, &wolfThread, NULL );
			if( rc != 0 ) {
				printf("Cannot create WolfThread\n");
				
				pthread_mutex_lock(&mutexThreadRun);
				WolfThreadRun = 0;
				pthread_mutex_unlock(&mutexThreadRun);
				
				return -1;
			}
		}
			
		if(strcmp(Mode, "WolfOFF") == 0){
			pthread_mutex_lock(&mutexThreadRun);
			if(WolfThreadRun == 1)
				WolfThreadRun = 0;
			else{
				printf("WolfThread isn't running\n");
				pthread_mutex_unlock(&mutexThreadRun);
				goto mainloop;
		  	}
			pthread_mutex_unlock(&mutexThreadRun);
		}
			
		if(strcmp(Mode, "StarON") == 0){
			pthread_mutex_lock(&mutexThreadRun);
				if(WolfThreadRun == 0 && StarThreadRun == 0)
					StarThreadRun = 1;
				else{
					printf("An effect is alreay running\n");
					pthread_mutex_unlock(&mutexThreadRun);
					goto mainloop;}
			pthread_mutex_unlock(&mutexThreadRun);


			rc = pthread_create( &starPThread, NULL, &starThread, NULL );
			if( rc != 0 ) {
				printf("Cannot create StarThread \n");
				
				pthread_mutex_lock(&mutexThreadRun);
				StarThreadRun = 0;
				pthread_mutex_unlock(&mutexThreadRun);

				return -1;}
		}
			
		if(strcmp(Mode, "StarOFF") == 0){
			pthread_mutex_lock(&mutexThreadRun);
			if(StarThreadRun == 1)
				StarThreadRun = 0;
			else{
				printf("Stareffect isn't running\n");
				pthread_mutex_unlock(&mutexThreadRun);
				goto mainloop;
		  	}
			pthread_mutex_unlock(&mutexThreadRun);
		}
		
		if(strcmp(Mode, "exit") == 0){
			pthread_mutex_lock(&mutexThreadRun);
			StarThreadRun = 0;
			WolfThreadRun = 0;
			pthread_mutex_unlock(&mutexThreadRun);
			goto exitloop;
		}

		mainloop:continue;
	}
	exitloop:
	if(setLEDsOFF()){
		printf("ERROR setLEDsOFF\n");
		return -1;}

//	pthread_exit( wolfPThread);
//	pthread_exit( starPThread);

	ws2811_fini(&myledstring);
	return 0;
}

int parseCommand(char command[BUFSIZE]){
	int i;
	char copycommand[BUFSIZE];
	char *splitCommand;

	for(i=0;i<BUFSIZE;i++)
		copycommand[i] = command[i];
	
	splitCommand=strtok(copycommand,";");
	if(strcmp(splitCommand, "TaPFX-DB") != 0)
		return -1;
	
	
	splitCommand=strtok(NULL,";");
	if(strcmp(splitCommand, "WolfON") == 0 || strcmp(splitCommand, "WolfOFF") == 0 || strcmp(splitCommand, "StarON") == 0 || strcmp(splitCommand, "StarOFF") == 0 || strcmp(splitCommand, "exit") == 0)
		Mode = splitCommand;
	else
		return -1;
	
	return 0;
}

