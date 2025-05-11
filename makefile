# Compiler and flags
CC = gcc
CFLAGS = -ansi -Wall -pedantic -IheaderFiles -g

# Directories
SRC_DIR = srcFiles
HEADER_DIR = headerFiles

# Object files
OBJS = main.o preas.o myTools.o guardian.o data.o asScan.o creator.o

# Target
assembler: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o assembler

# Object file rules
main.o: $(SRC_DIR)/main.c $(HEADER_DIR)/*.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/main.c

preas.o: $(SRC_DIR)/preas.c $(HEADER_DIR)/*.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/preas.c

myTools.o: $(SRC_DIR)/myTools.c $(HEADER_DIR)/*.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/myTools.c

guardian.o: $(SRC_DIR)/guardian.c $(HEADER_DIR)/*.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/guardian.c

data.o: $(SRC_DIR)/data.c $(HEADER_DIR)/*.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/data.c

asScan.o: $(SRC_DIR)/asScan.c $(HEADER_DIR)/*.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/asScan.c

creator.o: $(SRC_DIR)/creator.c $(HEADER_DIR)/*.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/creator.c


# Clean rule
clean:
	rm -f $(OBJS) assembler
