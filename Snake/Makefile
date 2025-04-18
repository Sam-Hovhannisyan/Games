progname=snake_game
CXX=g++
CXXFLAGS=-Wall -Wextra -Werror -std=c++17 -I. -I../resources/headers
LDFLAGS=-lncurses
BUILDS=builds

ifeq ($(MAKECMDGOALS),)
	BUILD_DIR=$(BUILDS)/debug
else
	BUILD_DIR=$(BUILDS)/$(MAKECMDGOALS)
endif

debug:   CXXFLAGS+=-g3
release: CXXFLAGS+=-g0 -DNDEBUG

SOURCES=main.cpp sources/Game.cpp ../resources/templates/Board.cpp
DEPENDS=$(patsubst %.cpp,$(BUILD_DIR)/%.d,$(SOURCES))
OBJS=$(patsubst %.cpp,$(BUILD_DIR)/%.o,$(SOURCES))

debug:   $(BUILD_DIR) $(BUILD_DIR)/$(progname)
release: $(BUILD_DIR) $(BUILD_DIR)/$(progname)

$(BUILD_DIR)/$(progname): $(OBJS) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: %.cpp | $(BUILD_DIR)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@
	$(CXX) $(CXXFLAGS) -MM $< -MT $@ > $(patsubst %.o,%.d, $@)

$(BUILD_DIR):
	mkdir -p $@
	mkdir -p $(BUILD_DIR)/sources
	mkdir -p $(BUILD_DIR)/../resources/sources

clean:
	rm -rf $(BUILDS)

run: $(BUILD_DIR)/$(progname)
	./$(BUILD_DIR)/$(progname)

.PHONY: clean run debug release

-include $(DEPENDS)