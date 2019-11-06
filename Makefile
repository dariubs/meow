default: main.c repl.c
	cc main.c -o meow -lreadline 

clean:
	-rm meow
