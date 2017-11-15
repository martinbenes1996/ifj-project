
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
	doxygen Doxyfile 2> /dev/null > /dev/null
#pdflatex doc/main.tex

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
	tar -zcvf ifj.tar.gz src/*.c src/*.h Makefile README install.sh dev/* test/* > /dev/null

# clean
.PHONY: clean
clean:
	@printf "";\
	cd ./src && make clean -s
	@echo "Cleaning project files.";\
	rm -rf doc/html test/*.err test/*.out test/*.stderr test/*.stdout
