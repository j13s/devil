TESTS=test_linux_config.o

%.o: %.c
	gcc $(CFLAGS) -o $(basename $@) -ltap $<
	
test: $(TESTS)
