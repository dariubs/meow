default: main.c repl.c
	cc main.c -o meow -lreadline 

run: meow
	./meow

clean:
	-rm meow
