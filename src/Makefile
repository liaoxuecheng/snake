all:tetris snake install

$(BUILD_OUTPUT)/%.d: %.cpp
	$(CC) -MM $(CPPFLAGS) $(build-dir)/$< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,$(BUILD_OUTPUT)/\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

local_src=$(wildcard $(build-dir)/*.cpp)

sources = $(notdir $(local_src))
sinclude $(sources:%.cpp=$(BUILD_OUTPUT)/%.d)

$(BUILD_OUTPUT)/%.o: %.cpp
	$(CC) -c -Wall $< -o $@ $(CPPFLAGS)

$(BUILD_OUTPUT)/snake: $(BUILD_OUTPUT)/node.o
	$(CC) -Wall $< -o $@ $(CPPFLAGS) $(LIBS)

$(BUILD_OUTPUT)/tetris: $(BUILD_OUTPUT)/tetris.o
	$(CC) -Wall $< -o $@ $(CPPFLAGS) $(LIBS)

.PHONY:tetris snake install

tetris:$(BUILD_OUTPUT)/tetris

snake:$(BUILD_OUTPUT)/snake

install:tetris snake
	cp $(BUILD_OUTPUT)/snake $(BUILD_OUTPUT)/tetris $(BINDIR)


.PHONY:clean

clean:
	@echo $(local_src) $(BUILD_OUTPUT) 
	rm -f $(BUILD_OUTPUT)/*

