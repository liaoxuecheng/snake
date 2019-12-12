all:tetris snake

$(BUILD_OUTPUT)/%.d: $(build-dir)/%.cpp
	$(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,$(BUILD_OUTPUT)/\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

local_src=$(wildcard $(build-dir)/*.cpp)

sources = $(notdir $(local_src))
sinclude $(local_src:$(build-dir)/%.cpp=$(BUILD_OUTPUT)/%.d)

$(BUILD_OUTPUT)/%.o: $(build-dir)/%.cpp
	$(CC) -c -Wall $< -o $@ $(CPPFLAGS)

$(BUILD_OUTPUT)/snake: $(BUILD_OUTPUT)/node.o
	$(CC) -Wall $< -o $@ $(CPPFLAGS) $(LIBS)

$(BUILD_OUTPUT)/tetris: $(BUILD_OUTPUT)/tetris.o
	$(CC) -Wall $< -o $@ $(CPPFLAGS) $(LIBS)

.PHONY:tetris snake

tetris:$(BUILD_OUTPUT)/tetris

snake:$(BUILD_OUTPUT)/snake tetris

install:$(BUILD_OUTPUT)/tetris $(BUILD_OUTPUT)/snake
	cp $^ $(BINDIR)


.PHONY:clean

clean:
	@echo $(local_src) $(BUILD_OUTPUT) 
	rm -f $(BUILD_OUTPUT)/*

