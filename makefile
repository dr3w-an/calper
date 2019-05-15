TARGET = calper
SRC = $(wildcard src/*.cc)
OBJ = $(SRC:.cc=.o)

$(TARGET): $(OBJ)
	$(CXX) -o $@ $^

.PHONY: clean
clean:
	rm -f $(OBJ) $(TARGET)
