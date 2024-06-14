cc := gcc
flags :=  -g
libs := -lz
obj := main.o emerald.o utills.o

emerald: $(obj)
	$(cc) $(obj) $(libs) $(flags) -o $@

%.o: %.c
	$(cc) -c $< $(flags) $(flags) -o $@

%.o: %.c %.h
	$(cc) -c $< $(flags) $(flags) -o $@

clean:
	rm emerald $(obj)

