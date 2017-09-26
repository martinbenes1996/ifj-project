
# Makefile
# Project manager file
# IFJ project
# FIT VUT
# 2017/2018

all:
	@printf "";\
	$(MAKE) -C src/


# doc
.PHONY: doc
doc:
	@echo "Generating documentation.";\
	doxygen Doxyfile 2> /dev/null > /dev/null

# test
.PHONY: test
test:
	@echo "Running verification.";\
	./test/verify

# help
.PHONY: help
help:
	@echo "Makefile Help.";\
	cat dev/make_howto

# zip
.PHONY: zip
zip:
	@printf "";\
	cd ./src && make zip

# clean
.PHONY: clean
clean:
	@printf "";\
	cd ./src && make clean
	@echo "Cleaning project files.";\
	rm -rf doc/* test/*.err test/*.out test/*.stderr test/*.stdout
