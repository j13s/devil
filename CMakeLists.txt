cmake_minimum_required (VERSION 2.6)
project (Devil)

set(CMAKE_C_FLAGS
"${CMAKE_C_FLAGS} -std=c99 -Werror -Wall -g3 -DGNU_C -DGO32 -pedantic -Wwrite-strings"
)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -z muldefs")

include_directories (
    "${PROJECT_SOURCE_DIR}/wins"
    "${PROJECT_SOURCE_DIR}/language"
)

add_subdirectory ("${PROJECT_SOURCE_DIR}/wins")

add_executable(
    devil   askcfg.c    do_event.c  do_opts.c   initio.c
            opt_txt.c   readlvl.c   tools.c     calctxt.c
            do_ins.c    do_side.c   insert.c    plot.c
            readtxt.c   userio.c    click.c     do_light.c
            do_stat.c   lac_cfg.c   savetool.c  title.c
            config.c    do_mod.c    do_tag.c    macros.c
            plotsys.c   tag.c       devil.c     do_move.c
            grfx.c      options.c   plottxt.c
)

target_link_libraries(
    devil   alleg       grx20X  X11 
            pthread     Xxf86vm Xpm
            Xcursor     gif     m
            devilwins	 
)
