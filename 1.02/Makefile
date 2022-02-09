CC = gcc
ECHO = echo
RM = rm -f

CFLAGS = -Wall -Werror -ggdb -funroll-loops

BIN = pokemon
OBJS = pokemon.o terrain.o navigation.o test.o

all: $(BIN)

$(BIN): $(OBJS)
	@$(ECHO) Linking $@
	@$(CC) $^ -o $@

-include $(OBJS:.o=.d)

%.o: %.c
	@$(ECHO) Compiling $<
	@$(CC) $(CFLAGS) -MMD -MF $*.d -c $<
	
.PHONY: all clean clobber

clean:
	@$(ECHO) Removing all generated files
	@$(RM) *.o $(BIN) *.d TAGS core vgcore.* gmon.out

clobber: clean
	@$(ECHO) Removing backup files
	@$(RM) *~ \#* *pgm