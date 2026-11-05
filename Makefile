CC = gcc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -O2
CPPFLAGS ?=
LDLIBS ?= -lm

ifeq ($(OS),Windows_NT)
EXEEXT := .exe
PYTHON := python
MKDIR = powershell -NoProfile -Command "New-Item -ItemType Directory -Force -Path '$1' | Out-Null"
REMOVE = powershell -NoProfile -Command "if (Test-Path -LiteralPath '$1') { Remove-Item -LiteralPath '$1' -Recurse -Force }"
else
EXEEXT :=
PYTHON := python3
MKDIR = mkdir -p $1
REMOVE = rm -rf $1
endif

APP := qmlc$(EXEEXT)
TEST_APP := build/test/test_runner$(EXEEXT)
PROD_OBJECTS := build/prod/a.o build/prod/common.o build/prod/circuit.o \
	build/prod/graph.o build/prod/input.o build/prod/analysis.o \
	build/prod/sim.o build/prod/qml.o
TEST_SOURCES := tests/test_main.c tests/test_common.c tests/test_circuit.c \
	tests/test_graph.c tests/test_input.c tests/test_analysis.c \
	tests/test_sim.c tests/test_qml.c
TEST_OBJECTS := $(patsubst %.c,build/test/%.o,$(TEST_SOURCES)) \
	build/test/common.o build/test/circuit.o build/test/graph.o \
	build/test/input.o build/test/analysis.o build/test/sim.o build/test/qml.o
ANALYSIS_ONLY_APP := build/analysis-only/analysis_only_test$(EXEEXT)
ANALYSIS_ONLY_SOURCES := tests/analysis_only_test.c common.c circuit.c \
	graph.c input.c analysis.c
ANALYSIS_ONLY_OBJECTS := $(patsubst %.c,build/analysis-only/%.o,\
	$(ANALYSIS_ONLY_SOURCES))
REFERENCE_APP := build/reference/reference_probe$(EXEEXT)
REFERENCE_SOURCES := tests/reference_probe.c common.c circuit.c sim.c qml.c
REFERENCE_OBJECTS := $(patsubst %.c,build/reference/%.o,$(REFERENCE_SOURCES))
SAN_CC ?= clang
SAN_FLAGS := -std=c11 -Wall -Wextra -Wpedantic -O1 -g \
	-fsanitize=address,undefined -fno-omit-frame-pointer

.PHONY: all test-unit test-analysis-only test-reference test-demos test sanitize clean

all: $(APP)

$(APP): $(PROD_OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDLIBS)

build/prod/%.o: %.c
	@$(call MKDIR,build/prod)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

build/test/%.o: %.c
	@$(call MKDIR,$(dir $@))
	$(CC) $(CPPFLAGS) $(CFLAGS) -DPQC_TESTING -I. -c $< -o $@

build/analysis-only/%.o: %.c
	@$(call MKDIR,$(dir $@))
	$(CC) $(CPPFLAGS) $(CFLAGS) -DPQC_TESTING -I. -c $< -o $@

build/reference/%.o: %.c
	@$(call MKDIR,$(dir $@))
	$(CC) $(CPPFLAGS) $(CFLAGS) -I. -c $< -o $@

$(TEST_APP): $(TEST_OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDLIBS)

test-unit: $(TEST_APP)
	$(TEST_APP)

$(ANALYSIS_ONLY_APP): $(ANALYSIS_ONLY_OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDLIBS)

test-analysis-only: $(ANALYSIS_ONLY_APP)
	$(ANALYSIS_ONLY_APP)

$(REFERENCE_APP): $(REFERENCE_OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDLIBS)

test-reference: $(REFERENCE_APP)
	$(PYTHON) tests/test_reference.py

test-demos: all
	$(PYTHON) tests/test_demos.py

test: all test-unit test-analysis-only test-reference test-demos
	$(PYTHON) tests/test_cli.py --stage all

sanitize:
	@$(call MKDIR,build/sanitize)
	$(SAN_CC) $(SAN_FLAGS) -DPQC_TESTING -I. $(TEST_SOURCES) \
		common.c circuit.c graph.c input.c analysis.c sim.c qml.c \
		-o build/sanitize/test_runner$(EXEEXT) $(LDLIBS)
	build/sanitize/test_runner$(EXEEXT)
	$(SAN_CC) $(SAN_FLAGS) -I. a.c common.c circuit.c graph.c input.c \
		analysis.c sim.c qml.c -o build/sanitize/qmlc$(EXEEXT) $(LDLIBS)
	$(SAN_CC) $(SAN_FLAGS) -I. $(REFERENCE_SOURCES) \
		-o build/sanitize/reference_probe$(EXEEXT) $(LDLIBS)
	PQC_TEST_APP=build/sanitize/qmlc$(EXEEXT) \
	PQC_TEST_PROBE=build/sanitize/reference_probe$(EXEEXT) \
		$(PYTHON) tests/test_cli.py --stage all

clean:
	@$(call REMOVE,build)
	@$(call REMOVE,qmlc)
	@$(call REMOVE,qmlc.exe)
