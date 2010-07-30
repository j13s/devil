W_OBJ=w_init.o w_event.o wi_buts.o wi_keys.o wi_winma.o wi_menu.o w_draw.o \
 w_tools.o w_system.o w_list.o
GRX_INCLUDES=-I$(HOME)/include

compile: $(W_OBJ)

%.o: %.c
	gcc $(CFLAGS) -c -o $@ $<
makeall: w_test.exe
CFLAGS=$(GRX_INCLUDES) -Werror -std=c99 -MMD -Wall -g3 -DGNU_C -DGO32 -pedantic -Wwrite-strings
%.exe: %.out
	rem strip $<
	coff2exe $<
test: $(W_OBJ)
	gcc $(CFLAGS) -g3 -L$(HOME)/lib -o w_test w_test.c ../linux_config.o $(W_OBJ) -lgrx20X -lX11 -lm
w_system.o: w_system.c wins.h wins_int.h w_system.h w_list.h
	gcc $(CFLAGS) -c w_system.c -o w_system.o
