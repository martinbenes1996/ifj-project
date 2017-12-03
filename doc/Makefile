
# Makefile
# Project manager file
# IFJ project
# FIT VUT
# 2017/2018


# compile settings
cc = gcc
flags = -O2 -g -std=c99 -pedantic -Wall -Wextra
links = -lm

# source settings
src = $(wildcard *.c)
head = $(wildcard *.h)

dep = $(src:.c=.dep)
obj = $(src:.c=.o)


#output settings
output = ifj

.PHONY: run
all: $(output)

# compiling
$(output) : $(obj)
	@echo "Linking project into $@.";\
	$(cc) $(flags) $(obj) -o $@ $(links)


# dependencies generating
ifneq ($(MAKECMDGOALS),clean)
-include $(dep)
endif

%.dep: %.c
	@echo "Generating dependencies $@.";\
	$(cc) $(flags) -MM $< -MF $@ && \
	sed -i $@ -e 's_$*.o[ ]*:_$*.o $@: _' 2> /dev/null




# object files generating
%.o : %.c
	@echo "Compiling $@.";\
	$(cc) $(flags) -c $< -o $@



# clean
.PHONY: clean
clean:
	@echo "Cleaning project files.";\
	rm -rf *~ *.o *.gch *.dep $(output)
