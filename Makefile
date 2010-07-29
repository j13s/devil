makeall: d.exe 

# include wins/makefile
# plotsys.o is not linked. Instead psysfast.o is used (see file "link" and
# plotsys.c).
OBJ=devil.o userio.o tools.o tag.o insert.o calctxt.o initio.o config.o \
 askcfg.o plot.o plottxt.o plotsys.o click.o savetool.o readlvl.o \
 readtxt.o do_event.o do_stat.o do_ins.o do_mod.o do_light.o do_move.o \
 do_tag.o do_side.o grfx.o do_opts.o opt_txt.o options.o macros.o title.o\
 lac_cfg.o linux_config.o

GRX_INCLUDES=-I$(HOME)/include

ifdef GER
CFLAGS=$(GRX_INCLUDES) -std=c99 -Werror -Wall -g3 -DGNU_C -DGO32 -DGER -ansi -pedantic -Wwrite-strings -I wins -I language -c
else
CFLAGS=$(GRX_INCLUDES) -std=c99 -Werror -Wall -g3 -DGNU_C -DGO32 -pedantic -Wwrite-strings -I wins -I language -c
#CFLAGS=-Wall -O3 -g -DLIST_DEBUG -DGNU_C -DGO32 -ansi -pedantic -ffast-math -Wwrite-strings -I wins -I language -c
#CFLAGS=-Wall -O3 -DGNU_C -DGO32 -ansi -pedantic -ffast-math -Wwrite-strings -I wins -I language -c
endif

# %.exe: %.out
#	rem strip $<
#	coff2exe $<
d.exe: $(OBJ) 
	gcc -L$(HOME)/lib -z muldefs @linkx

compile: $(OBJ)
%.o: %.c
	gcc -MMD $(CFLAGS) -g3 -o $@ $<
plotsys.o: plotsys.c plotsys.h plotdata.h structs.h
	gcc $(CFLAGS) plotsys.c -o plotsys.o
plottxt.o: plottxt.c plottxt.h plotsys.h plotdata.h structs.h
	gcc $(CFLAGS) -lm -I wins -I language -Wall -g3 -DGNU_C -c plottxt.c -o plottxt.o
title.o: title.c title.h structs.h
	gcc $(CFLAGS) title.c -o title.o
lac_cfg.o: lac_cfg.c lac_cfg.h
	gcc $(CFLAGS) lac_cfg.c -o lac_cfg.o
linux_config.o: linux_config.c linux_config.h
	gcc $(CFLAGS) linux_config.c -o linux_config.o
    
