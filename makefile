CC = gcc

CFLAGS = -Wall -g -pthread

RM = rm -f

OBJS = main.o monitor.o

MAIN = tp2

all: $(MAIN)

$(MAIN): $(OBJS)
  clear
	@echo ""
	@echo " --- COMPILANDO PROGRAMA: tp2 ---"
	@$(CC) $(CFLAGS) $(OBJS) -lm -o $(MAIN)
	@echo ""

%.o: %.c %.h
	@echo " --- COMPILANDO OBJETO \"$@\""
	@$(CC) $(CFLAGS) $< -c 

clean:
	$(RM) $(MAIN) *.o output
	clear

