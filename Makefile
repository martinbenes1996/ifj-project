
# Makefile
# Project manager file
# IFJ project
# FIT VUT
# 2017/2018

all:
	@printf "";\
	$(MAKE) -C src/ -s


# doc
.PHONY: doc
doc:
	@echo "Generating documentation.";\
	#doxygen Doxyfile 2> /dev/null > /dev/null
	cd doc && pdflatex ifj.tex

# test
.PHONY: test
test:
	@echo "Running verification.";\
	cd test && ./verify

# help
.PHONY: help
help:
	@echo "Makefile Help.";\
	cat dev/make_howto

# zip
.PHONY: zip
zip:
	@echo "Compressing and zipping.";\
	mkdir tmp
	@printf "";\
	cp src/*.c tmp/
	@printf "";\
	cp src/*.h tmp/
	@printf "";\
	cp doc/Makefile tmp/
	@printf "";\
	cp doc/rozsireni tmp/
	@printf "";\
	cp doc/rozdeleni tmp/
	@printf "";\
	make doc 2> /dev/null > /dev/null
	@printf "";\
	cp doc/dokumentace.pdf tmp/
	@printf "";\
	cd tmp && tar -zcvf ~/Desktop/xbenes49.tgz * > /dev/null 2>/dev/null && cd ..
	@printf "";\
	rm -rf tmp

# clean
.PHONY: clean
clean:
	@printf "";\
	cd ./src && make clean -s
	@echo "Cleaning project files.";\
	rm -rf doc/html doc/*.toc doc/*.aux doc/*.log tmp/
	@printf "";\
	$(MAKE) -C test/ -s
