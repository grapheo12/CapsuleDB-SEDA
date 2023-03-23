# Add all tests here.
SHELL=/bin/bash
BINARIES = ../build/cdbhost ../build/cdbenc.signed
CONFIG_FILE = test_config.ini
.PHONY: all db

all: correctness

db:
	mkdir -p db
	rm -rf db/*

.PHONY: clean
clean:
	rm -rf db


# Correctness Test

CORRECTNESS_MAX_KEY := 50000
CORRECTNESS_MAX_REQ := 500000
CORRECTNESS_WR_RATIO := 0.2
correctness: db test_gen.py $(BINARIES)
	python3 test_gen.py $(CORRECTNESS_WR_RATIO) $(CORRECTNESS_MAX_KEY) $(CORRECTNESS_MAX_REQ) > sim.log
	$(BINARIES) $(CONFIG_FILE) < inp.txt > out.txt 2> debug.log
	-sdiff -s out.txt ans.txt > diff.log
	@echo "Correctness Pass if next line is blank:"
	tail diff.log


clean-recovery: db test_gen.py $(BINARIES)
	#python3 test_gen.py $(CORRECTNESS_WR_RATIO) $(CORRECTNESS_MAX_KEY) $(CORRECTNESS_MAX_REQ) > sim.log
	$(eval QUERY_NUM := $(shell grep -c ^ inp.txt))
	head -n $$(( $(QUERY_NUM) - ($(QUERY_NUM) / 2) )) inp.txt > inp1.txt
	echo "END" >> inp1.txt	
	tail -n $$(( $(QUERY_NUM) / 2 )) inp.txt  > inp2.txt
	tail -n $$(( $(QUERY_NUM) / 2 )) ans.txt  > ans2.txt
	$(BINARIES) $(CONFIG_FILE) < inp1.txt > out1.txt 2> debug.log
	@echo "Starting from the result of previous execution ..."
	$(BINARIES) $(CONFIG_FILE) --load < inp2.txt > out2.txt 2> debug.log
	-sdiff -s out2.txt ans2.txt > diff.log
	@echo "Correctness Pass if next line is blank:"
	tail diff.log
