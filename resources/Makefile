progname=board
utest=utest_$(progname)
CXX=g++
CXXFLAGS=-Wall -Wextra -Werror -std=c++11 -I.
BUILDS=builds

ifeq ($(MAKECMDGOALS),)
	BUILD_DIR=$(BUILDS)/debug
else
	BUILD_DIR=$(BUILDS)/$(MAKECMDGOALS)
endif

debug:   CXXFLAGS+=-g3
release: CXXFLAGS+=-g0 -DNDEBUG

SOURCES=main.cpp $(wildcard sources/*.cpp)
DEPENDS=$(patsubst %.cpp,$(BUILD_DIR)/%.d,$(SOURCES))
PREPROCS=$(patsubst %.cpp,$(BUILD_DIR)/%.ii,$(SOURCES))
ASSEMBLES=$(patsubst %.cpp,$(BUILD_DIR)/%.s,$(SOURCES))
OBJS=$(patsubst %.cpp,$(BUILD_DIR)/%.o,$(SOURCES))

UTEST_SOURCES=main_utest.cpp $(wildcard sources/*.cpp)
UTEST_DEPENDS=$(patsubst %.cpp,$(BUILD_DIR)/%.d,$(UTEST_SOURCES))
UTEST_PREPROCS=$(patsubst %.cpp,$(BUILD_DIR)/%.ii,$(UTEST_SOURCES))
UTEST_ASSEMBLES=$(patsubst %.cpp,$(BUILD_DIR)/%.s,$(UTEST_SOURCES))
UTEST_OBJS=$(patsubst %.cpp,$(BUILD_DIR)/%.o,$(UTEST_SOURCES))

TEST_INPUTS=$(wildcard tests/test*.input) 
TESTS=$(patsubst %.input,%,$(TEST_INPUTS))

debug:   $(BUILD_DIR) utest qa
release: $(BUILD_DIR) qa

utest: $(BUILD_DIR)/$(utest)
	./$<

qa: $(TESTS)

test%: $(BUILD_DIR)/$(progname)
	./$(BUILD_DIR)/$(progname) < $@.input > $(BUILD_DIR)/$@.output
	diff $(BUILD_DIR)/$@.output $@.expected > /dev/null && echo "$@ PASSED" || echo "$@ FAILED"

$(BUILD_DIR)/$(utest): $(UTEST_OBJS) | $(BUILD_DIR)/sources
	$(CXX) $(CXXFLAGS) $^ -lgtest -o $@

$(BUILD_DIR)/$(progname): $(OBJS) | $(BUILD_DIR)/sources
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BUILD_DIR)/%.ii: %.cpp | $(BUILD_DIR)/sources
	$(CXX) $(CXXFLAGS) -E $< -o $@
	$(CXX) $(CXXFLAGS) -MM $< -MT $@ > $(patsubst %.ii,%.d, $@)

$(BUILD_DIR)/%.s: $(BUILD_DIR)/%.ii
	$(CXX) $(CXXFLAGS) -S $< -o $@

$(BUILD_DIR)/%.o: $(BUILD_DIR)/%.s
	$(CXX) $(CXXFLAGS) -c $< -o $@

.gitignore:
	echo $(BUILD_DIR)/$(utest) >> .gitignore
	echo $(BUILD_DIR)/$(progname) >> .gitignore

$(BUILD_DIR):
	mkdir -p $@
	mkdir -p $(BUILD_DIR)/sources
	mkdir -p $(BUILD_DIR)/tests

clean:
	rm -rf $(BUILDS) tests/*.output .gitignore

.PRECIOUS:  $(PREPROCS) $(ASSEMBLES)
.SECONDARY: $(PREPROCS) $(ASSEMBLES)

sinclude $(DEPENDS)
