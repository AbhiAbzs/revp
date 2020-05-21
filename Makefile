TARGET:=revp

SOURCES:=./sources
HEADERS:=./headers

ifeq ($(MAKECMDGOALS),build)
	BUILD:=./build
else ifeq ($(MAKECMDGOALS),debug)
	BUILD:=./build/debug
else
	BUILD:=./build
endif

CC:=g++
CFLAGS:=-I$(HEADERS) -lboost_program_options -lboost_regex -lpthread

CFILES:=$(shell find $(SOURCES) -printf '%P ' -name '*.cpp')
OFILES:=$(patsubst %.cpp,$(BUILD)/%.o,$(CFILES))

.PHONY: build debug clean mkdir
.DEFAULT_GOAL:=build

build: CFLAGS+=-O3
build: mkdir $(OFILES)
	$(CC) $(OFILES) -o $(TARGET).exe $(CFLAGS)

debug: CFLAGS+=-O0 -ggdb -D DEBUG
debug: mkdir $(OFILES)
	$(CC) $(OFILES) -o $(TARGET).debug.exe $(CFLAGS)

mkdir:
	mkdir -p $(BUILD)

clean:
	rm -rf $(BUILD)
	rm -f $(TARGET).exe
	rm -f $(TARGET).debug.exe

$(OFILES): $(BUILD)/%.o: $(SOURCES)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@
