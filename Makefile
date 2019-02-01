cc = gcc
prom = crc
source = crc.c
 
$(prom): $(source)
	$(cc) -o $(prom) $(source)

clean:
	rm ${prom}
