default: main.c repl.c
	cc -o meow main.c

clean:
	-rm meow
