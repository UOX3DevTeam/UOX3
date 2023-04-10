BASE      := ..
SPIDER    := $(BASE)/support/spidermonkey
ZLIB      := $(BASE)/support/zlib
SUPPORT      := $(BASE)/support
PLATFORM  := $(shell uname)


ifeq ($(PLATFORM),FreeBSD)
CXX     	:= clang++
else
CXX     	:= g++
GCCVER8		:= $(shell expr `gcc -dumpversion | cut -f1 -d.` \<= 8)
endif

CXXFLAGS	:= -std=c++17 -O2 -pthread -Wno-shift-negative-value -Wall -Wextra -Wno-attributes
LDFLAGS		:= -L$(SPIDER)  -ljs32 -L$(ZLIB) -lz
TARGET		:= uox3
INCLUDE		:=  -I$(SPIDER) -I. -I$(ZLIB) -I$(SUPPORT)

DEPDIR := ./.deps
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d
POSTCOMPILE = mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d && touch $@

BUILD    	:= ./build
SRC      	:= $(wildcard *.cpp)
OBJECTS  	:= $(SRC:%.cpp=$(BUILD)/%.o)

ifeq "$(GCCVER8)" "1"
	LDFLAGS += -lstdc++fs
endif

#COMPILE 	:= $(CXX) $(DEPFLAGS) $(CXXFLAGS) $(INCLUDE) -c

all: build $(TARGET)

$(BUILD)/%.o: %.cpp
$(BUILD)/%.o: %.cpp $(DEPDIR)/%.d | $(DEPDIR)
	@mkdir -p $(@D)
	$(CXX)  $(DEPFLAGS) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

$(DEPDIR): ; @mkdir -p $@

DEPFILES := $(SRC:%.cpp=$(DEPDIR)/%.d)
$(DEPFILES):

include $(wildcard $(DEPFILES))

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $^ $(LDFLAGS)

.PHONY: all build clean

build:
	@mkdir -p $(BUILD)


.PHONY clean:
	-@rm -rf $(BUILD)
	-@rm -rf $(DEPDIR)
	-@rm $(TARGET)

