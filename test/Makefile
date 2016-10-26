TEST_DIRS := functionality performance crash_handler experiments
DEMO_DIRS := $(wildcard *_demo/)
ENGINE_DIRS := $(wildcard *_engine/)
MARINE_DIR := ../marine
MARINE_SRC := $(shell find $(MARINE_DIR) -name "*.hh")

CPPCHECK := cppcheck -D__GNUC__ -D__cplusplus -f --enable=all --inline-suppr --suppress=unusedFunction
#CPPCHECK := cppcheck -f --enable=all --inline-suppr --suppress=ConfigurationNotChecked --suppress=missingInclude
#--xml

all: debug

debug:
	@for dir in $(TEST_DIRS) ; do if ! $(MAKE) debug -C $${dir} ; then exit 1 ; fi ; done
	@for dir in $(DEMO_DIRS) ; do if ! $(MAKE) debug -C $${dir} ; then exit 1 ; fi ; done

release:
	@for dir in $(TEST_DIRS) ; do if ! $(MAKE) release -C $${dir} ; then exit 1 ; fi ; done
	@for dir in $(DEMO_DIRS) ; do if ! $(MAKE) release -C $${dir} ; then exit 1 ; fi ; done

cppcheck:
#	@echo | cpp -x c++ -Wp,-v 2>&1 | grep "#include <" -A 50 | grep "/usr" | sed "s/ //g" > src.inc
	@echo $(MARINE_SRC) | sed "s/ /\n/g" | grep -vw deprecated > src.list
	@$(CPPCHECK) --file-list=src.list #--includes-file=src.inc
	@$(RM) src.list src.inc

clean:
	@for dir in $(TEST_DIRS) ; do if ! $(MAKE) clean -C $${dir} ; then exit 1 ; fi ; done
	@for dir in $(DEMO_DIRS) ; do if ! $(MAKE) clean -C $${dir} ; then exit 1 ; fi ; done

cleanall:
	@for dir in $(TEST_DIRS) ; do if ! $(MAKE) cleanall -C $${dir} ; then exit 1 ; fi ; done
	@for dir in $(DEMO_DIRS) ; do if ! $(MAKE) cleanall -C $${dir} ; then exit 1 ; fi ; done
	$(RM) tags $(MARINE_DIR)/tags

line:
	@wc -l $(MARINE_SRC)

.PHONY: all debug release clean cleanall cppcheck line
