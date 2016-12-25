all: DB-Server.o

DB-Server.o: TaPFX-DB_Server.c UDPServer.c ledString.c ws2811/ws2811.c ws2811/dma.c ws2811/rpihw.c ws2811/mailbox.c ws2811/pwm.c
	gcc TaPFX-DB_Server.c UDPServer.c ledString.c  ws2811/ws2811.c ws2811/dma.c ws2811/rpihw.c ws2811/mailbox.c ws2811/pwm.c -lpthread -o DB-Server
	
