CFLAGS = -g -Wall -Wextra -ansi -pedantic

.PHONY: standard
standard: bin/ktui

.PHONY: debug
debug: CFLAGS += -DKLONTUIKE_DEBUG
debug: standard

bin/ktui: build/main.o build/visual.o build/logical.o build/lv_inter.o
	mkdir -p bin
	gcc -o bin/ktui build/main.o build/visual.o build/logical.o build/lv_inter.o -lncurses $(CFLAGS)

build/main.o: src/main.c src/visual.h src/lv_inter.h
	mkdir -p build
	gcc -o build/main.o -c src/main.c $(CFLAGS)

build/visual.o: src/visual.c src/visual.h src/lv_inter.h
	mkdir -p build
	gcc -o build/visual.o -c src/visual.c $(CFLAGS)

build/logical.o: src/logical.c src/logical.h
	mkdir -p build
	gcc -o build/logical.o -c src/logical.c $(CFLAGS)
	
build/lv_inter.o: src/lv_inter.c src/lv_inter.h src/logical.h
	mkdir -p build
	gcc -o build/lv_inter.o -c src/lv_inter.c $(CFLAGS)

.PHONY: unicode
unicode: bin/ktui_uni

bin/ktui_uni: build/main_uni.o build/visual_uni.o build/logical.o build/lv_inter.o
	mkdir -p bin
	gcc -o bin/ktui_uni build/main_uni.o build/visual_uni.o build/logical.o build/lv_inter.o -lncursesw $(CFLAGS)

build/main_uni.o: src/main.c src/visual.h src/lv_inter.h
	mkdir -p build
	gcc -o build/main_uni.o -c src/main.c $(CFLAGS) -DKLONTUIKE_UNICODE

build/visual_uni.o: src/visual.c src/visual.h src/lv_inter.h
	mkdir -p build
	gcc -o build/visual_uni.o -c src/visual.c $(CFLAGS) -DKLONTUIKE_UNICODE

.PHONY: clean
clean:
	rm -rf build
	rm -rf bin