COVERAGE=N

VALGRIND=valgrind
VALGRIND_OPTS=--tool=memcheck --track-origins=yes --show-reachable=yes --num-callers=20 --leak-check=full --error-exitcode=1

CPPCHECK=cppcheck
CPPCHECK_OPTS=--enable=performance,missingInclude,portability,style --error-exitcode=1

RM=rm -f

CXXFLAGS = -I../inc -Wall -Wextra -Wold-style-cast -Wswitch-default -pedantic -pedantic-errors -Weffc++ -Wcast-align -Wcast-qual
CXXFLAGS += -Wctor-dtor-privacy -Wmissing-declarations -Wmissing-include-dirs -Woverloaded-virtual
CXXFLAGS += -Wunused-parameter -Wunused -Wshadow -Wfloat-equal
CXXFLAGS += -Wsign-conversion -Wsign-promo -Wredundant-decls -Wuninitialized -Winit-self -Werror
CXXFLAGS += -Wpointer-arith -Wtype-limits -Wwrite-strings -Wnon-virtual-dtor

ifeq ($(CXX),g++)
	CXXFLAGS += -Wnoexcept -Wlogical-op
endif

ifeq ($(COVERAGE),Y)
	CXXFLAGS += --coverage -g3 -fno-inline -fno-inline-small-functions -fno-default-inline
else
	CXXFLAGS += -O3
endif

tests:
	$(MAKE) util_test.o
	$(MAKE) primitive_test.o
	$(MAKE) headers_test.o
	$(MAKE) metadata_test.o
	$(MAKE) produce_test.o
	$(MAKE) main_test.o

valgrind: tests
	$(VALGRIND) $(VALGRIND_OPTS) ./util_test.o
	$(VALGRIND) $(VALGRIND_OPTS) ./primitive_test.o
	$(VALGRIND) $(VALGRIND_OPTS) ./headers_test.o
	$(VALGRIND) $(VALGRIND_OPTS) ./metadata_test.o
	$(VALGRIND) $(VALGRIND_OPTS) ./produce_test.o
	$(VALGRIND) $(VALGRIND_OPTS) ./main_test.o

coverage:
	$(MAKE) util_test.o COVERAGE=Y
	$(MAKE) primitive_test.o COVERAGE=Y
	$(MAKE) headers_test.o COVERAGE=Y
	$(MAKE) metadata_test.o COVERAGE=Y
	$(MAKE) produce_test.o COVERAGE=Y
	$(MAKE) main_test.o COVERAGE=Y
	(cd .. && python test/upload_coverage_to_coveralls.py -i inc)

cppcheck:
	$(CPPCHECK) $(CPPCHECK_OPTS) ../inc/kafka_broker_stub/*.hpp

clean:
	$(RM) *.o
	$(RM) *.gcno
	$(RM) *.gcda

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@
	./$@


