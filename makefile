TARGET = calper
PREFIX = /usr/local
SRC = $(wildcard src/*.cc)
OBJ = $(SRC:.cc=.o)

$(TARGET): $(OBJ)
	$(CXX) -o $@ $^

clean:
	rm -f $(OBJ) $(TARGET)

install:
	install -d $(DESTDIR)$(PREFIX)/bin
	install -m 755 $(TARGET) $(DESTDIR)$(PREFIX)/bin

.PHONY: clean install
