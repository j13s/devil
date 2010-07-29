W_OBJ=w_init.o w_event.o wi_buts.o wi_keys.o wi_winma.o wi_menu.o w_draw.o \
 w_tools.o w_system.o w_list.o
GRX_INCLUDES=-I$(HOME)/include

compile: $(W_OBJ)

%.o: %.c
	gcc $(CFLAGS) -o $@ $<
makeall: w_test.exe
CFLAGS=$(GRX_INCLUDES) -std=c99 -MMD -Wall -g3 -DGNU_C -DGO32 -pedantic -Wwrite-strings -c
%.exe: %.out
	rem strip $<
	coff2exe $<
w_test.out: $(W_OBJ) w_test.o $(HOME)/lib/libgrx20X.a
	gcc $(CFLAGS)-g3 -L$(HOME)/lib -o w_test.out w_test.o ../linux_config.o $(W_OBJ) -lgrx20X -lX11 -lm
w_system.o: w_system.c wins.h wins_int.h w_system.h w_list.h
	gcc $(CFLAGS) -c w_system.c -o w_system.o
