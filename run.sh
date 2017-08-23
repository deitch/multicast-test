#!/bin/bash

MADDR=224.16.17.8
PORT=$(($RANDOM%8000+2001))

# Find an unused multicast address /32 beginning with
route add ${MADDR}/32 via 127.0.0.1 dev lo

./dist/server ${MADDR} ${PORT} &
SERVER_PID=$!
# need to sleep long enough for server to be listening
sleep 5
./dist/client ${MADDR} ${PORT}

kill $SERVER_PID

route del ${MADDR}/32 via 127.0.0.1 dev lo
