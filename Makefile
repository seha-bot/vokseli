CFLAGS=-lglfw3 -lGL -lm

src := $(wildcard src/*.c)
obj := $(src:src/%.c=build/%.o)

all: build shaders main

build:
	@mkdir -p build

shaders: FORCE
	@cp -r shaders build

main: $(obj)
	@gcc $^ -Llib $(CFLAGS) -o build/$@

build/%.o: src/%.c
	@gcc -c -Iinc $^ -o $@

clean:
	rm -rf build

FORCE:

