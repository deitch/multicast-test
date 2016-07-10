DIR=./dist

default: build run

clean:
	rm -rf ${DIR}

run: build
	./run.sh
	
dir:
	mkdir -p ${DIR}

build: dir
	gcc -o ${DIR}/client client.c
	gcc -o ${DIR}/server server.c

