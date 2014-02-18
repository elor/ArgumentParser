OBJ+= obj/convert.o
OBJ+= obj/Argument.o
OBJ+= obj/ArgumentParser.o
OBJ+= obj/ArgumentParserInternals.o

INC+= ArgumentParser.h

OUT= argumentparser
ARL=lib$(OUT).a

BIN_DIR= /home/elor/usr/bin/
LIB_DIR= /home/elor/usr/lib/
INC_DIR= /home/elor/usr/include/

.PHONY: clean install uninstall

#DECL+= -DANNOY
#DECL+= -DDEBUG
DECL+= -DPRINT

#DECL+= -DLMPSTUB

DECL+= -DTEST

DECL+= -DRELEASE

obj/%.o: src/%.cpp
	c++ -g -Iinclude $(DECL) -c -Wall $(<:obj/=src/) -o $@

remake: 
	make -j 8 $(OUT)

new: clean remake
	echo done

$(OUT): dirs $(OBJ)
	ar rcs $(ARL) $(OBJ)

install:
	install $(ARL) $(LIB_DIR)
	install include/$(INC) $(INC_DIR)
	
uninstall:
	rm $(LIB_DIR)/$(ARL) $(INC_DIR)/$(INC)

open:
	bash -c "vim $(shell echo -n $(shell tree -fi | grep ./*.*pp))"

list:
	@tree -fi | grep ./*.*pp

find:
	-Scripts/findbycontent.sh

count:
	bash -c "cat $(shell echo -n $(shell tree -fi | grep ./*.*pp)) | wc -l"

dirs:
	mkdir -p $(shell find src -type d | awk '{sub(/src/,"obj");print}')
	
clean:
	-rm -rfv ./obj
	-rm -fv $(OUT)

all:
	echo "stub"
