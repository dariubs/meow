UNAME_S := $(shell uname -s 2>/dev/null)
# On macOS use system clang to avoid Homebrew LLVM's z3 dependency
ifeq ($(UNAME_S),Darwin)
  CC = /usr/bin/clang
else
  CC = clang
endif

ifeq ($(OS),Windows_NT)
  CFLAGS_REPL = -DMEOW_NO_READLINE
  LIBS =
else
  CFLAGS_REPL =
  LIBS = -lreadline
endif

# On macOS, use SDK so clang finds system headers (stdio.h, etc.)
ifeq ($(UNAME_S),Darwin)
  SDKROOT := $(shell xcrun --show-sdk-path 2>/dev/null)
  ifneq ($(SDKROOT),)
    CFLAGS_SDK = -isysroot $(SDKROOT)
  endif
endif

meow: main.c repl.c run.c lexer.c parser.c interp.c
	$(CC) $(CFLAGS_SDK) $(CFLAGS_REPL) main.c repl.c run.c lexer.c parser.c interp.c -o meow $(LIBS)

run: meow
	./meow

clean:
	-rm -f meow meow.exe
