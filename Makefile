main: main.c http_message.c request.c
	gcc -o main main.c http_message.c request.c

clean:
	rm main