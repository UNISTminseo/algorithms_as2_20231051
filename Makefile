# Makefile

CC = gcc
CFLAGS = -std=c11 -O2 -Wall -Wextra

SRCDIR = src
OBJDIR = obj
BINDIR = bin
TARGET = tsp_solver

SOURCES = $(SRCDIR)/graph.c \
          $(SRCDIR)/parser.c \
          $(SRCDIR)/mst_approx.c \
          $(SRCDIR)/held_karp.c \
          $(SRCDIR)/my_algo.c \
          $(SRCDIR)/main.c

OBJECTS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SOURCES))

all: $(BINDIR)/$(TARGET)

$(BINDIR):
	mkdir -p $(BINDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BINDIR)/$(TARGET): $(OBJECTS) | $(BINDIR)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -rf $(OBJDIR) $(BINDIR)

.PHONY: all clean
