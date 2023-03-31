KERN_DIR = /home/book/buildroot-100ask_t113-pro/buildroot/output/build/linux-origin_master


CC=$(CROSS_COMPILE)gcc
  
objs=Uart.o gps_analyse.o main.o  
srcs=Uart.c gps_analyse.c main.c  
  
gps: $(objs)  
	$(CC) -o gps $(objs)  

main.o: $(srcs) gps_analyse.h Uart.h
	$(CC) -c  $(srcs)  

Uart.o: Uart.c  
	$(CC) -c Uart.c  

analyse_gps.o: gps_analyse.c gps_analyse.h  
	$(CC) -c gps_analyse.c  

clean:  
	rm -f gps *.o 
