CC = gcc
CFLAGS = -Wall -ansi -pedantic -g -Iinclude
TARGET = assembler


SRCS = src/main.c \
       src/assembler.c \
       src/preprocessor.c \
       src/parser.c \
       src/first_pass.c \
       src/second_pass.c \
       src/symbol_table.c \
       src/macro_table.c \
       src/instruction_table.c \
       src/code_image.c \
       src/data_image.c \
       src/encoder.c \
       src/output_files.c \
       src/errors.c \
       src/utils.c

OBJS = $(SRCS:.c=.o)


HEADERS = include/main.h \
          include/assembler.h \
          include/preprocessor.h \
          include/parser.h \
          include/first_pass.h \
          include/second_pass.h \
          include/symbol_table.h \
          include/macro_table.h \
          include/instruction_table.h \
          include/code_image.h \
          include/data_image.h \
          include/encoder.h \
          include/output_files.h \
          include/errors.h \
          include/utils.h

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)


src/%.o: src/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
