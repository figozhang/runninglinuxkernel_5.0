cc = aarch64-linux-gnu-gcc
prom = test
obj = test.o
CFLAGS = -static
 
$(prom): $(obj)
	    $(cc) -o $(prom) $(obj) $(CFLAGS)

%.o: %.c 
	    $(cc) -c $< -o $@

clean:
	    rm -rf $(obj) $(prom)

