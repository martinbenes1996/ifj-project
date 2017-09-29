
# Makefile
# Compile manager file
# IFJ project
# FIT VUT
# 2017/2018

# compile settings
cc = gcc
defines = -DDEBUG_MODE #-DUSE_SYSLOG
linkings = -lpthread -lm
flags = $(defines) -O2 -g -std=c99 -pedantic -Wall -Wextra


# source settings
src = $(wildcard *.c)
head = $(wildcard *.h)

dep = $(src:.c=.dep)
obj = $(src:.c=.o)


#output settings
output = ifj
all: $(output)
	
ifneq ($(MAKECMDGOALS),clean)#$(filter $(MAKECMDGOALS), clean zip )
ifneq ($(MAKECMDGOALS),zip)
-include $(dep)
endif
endif



# linking
$(output) : $(obj)
	@echo "Linking project into $@.";\
	$(cc) $(flags) $(obj) -o ../$@ $(linkings)

# dependencies generating
%.dep: %.c
	@echo "Generating dependencies $@.";\
	$(cc) $(flags) -MM $< -MF $@ && \
	sed -i $@ -e 's_$*.o[ ]*:_$*.o $@: _'


# compiling
%.o : %.c
	@echo "Compiling $@.";\
	$(cc) $(flags) $(defines) -c $< -o $@


# zip
.PHONY: zip
zip:
	@echo "Compressing and zipping.";\
	tar -zcvf ../$(output).tar.gz $(src) $(head) Makefile > /dev/null

# clean
.PHONY: clean
clean:
	@echo "Cleaning generated files.";\
	rm -rf *~ *.o *.gch *.dep ../$(output) ../$(output).tar.gz