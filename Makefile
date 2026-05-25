CC = gcc
CFLAGS = -Wall -Wextra -ansi -pedantic -Iheaders

SRC = src/assembler_table.c \
      src/first_pass.c \
      src/macro_helper.c \
      src/mem_img.c \
      src/opcode_list.c \
      src/output_files.c \
      src/parser.c \
      src/pre_assembly.c \
      src/second_pass.c \
      src/util.c

OBJ = $(SRC:.c=.o)

TARGET = assembler

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(TARGET)