
# Makefile
# Project manager file
# IFJ project
# FIT VUT
# 2017/2018


# compile settings
cc = gcc
defines = -DDEBUG_MODE
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


# linking
$(output) : $(obj)
	@echo "Linking project into $@.";\
	$(cc) $(flags) $(obj) -o $@ $(linkings)

# dependencies generating
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),zip)
ifneq ($(MAKECMDGOALS),doc)
-include $(dep)
endif
endif
endif
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
	tar -zcvf $(output).tar.gz $(src) $(head) Makefile > /dev/null

# doc
.PHONY: doc
doc:
	@echo "Generating documentation.";\
	doxygen Doxyfile 2> /dev/null > /dev/null

# clean
.PHONY: clean
clean:
	@echo "Cleaning project files.";\
	rm -rf *~ *.o *.gch *.dep $(output) $(output).tar.gz doc/*
