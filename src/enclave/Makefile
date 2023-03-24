# Copyright (c) Open Enclave SDK contributors.
# Licensed under the MIT License.

include ../config.mk

PROTOC=../../third-party/protobufs-bin/bin/protoc
PROTO_INCL=-I../../third-party/protobufs-bin/include
PROTO_LIB=-L../../third-party/protobufs-bin/lib -lprotobuf -loeenclave -loelibcxx -loelibc -loesyscall -loecore


CRYPTO_LDFLAGS := $(shell pkg-config oeenclave-$(CXX_COMPILER) --variable=${OE_CRYPTO_LIB}libs)

ifeq ($(LVI_MITIGATION), ControlFlow)
    ifeq ($(LVI_MITIGATION_BINDIR),)
        $(error LVI_MITIGATION_BINDIR is not set)
    endif
    # Only run once.
    ifeq (,$(findstring $(LVI_MITIGATION_BINDIR),$(CC)))
        CC := $(LVI_MITIGATION_BINDIR)/$(CC)
    endif
    COMPILER := $(COMPILER)-lvi-cfg
    CRYPTO_LDFLAGS := $(shell pkg-config oeenclave-$(COMPILER) --variable=${OE_CRYPTO_LIB}libslvicfg)
endif

CXXFLAGS=--std=c++17 $(shell pkg-config oeenclave-$(CXX_COMPILER) --cflags) -O2 #-DDEBUG -g
CFLAGS=$(shell pkg-config oeenclave-$(COMPILER) --cflags) -O2 #-DDEBUG -g 

LDFLAGS=$(shell pkg-config oeenclave-$(CXX_COMPILER) --libs)
INCDIR=$(shell pkg-config oeenclave-$(CXX_COMPILER) --variable=includedir)

HEADERS = cdb_t.h cdb_t.c cdb_args.h

all:
	$(MAKE) build
	$(MAKE) keys
	$(MAKE) sign

$(HEADERS): ../cdb.edl
	oeedger8r ../cdb.edl --trusted \
		--search-path $(INCDIR) \
		--search-path $(INCDIR)/openenclave/edl/sgx

BLD_DIR = ../../build
BLD_SUBDIR = $(BLD_DIR)/enclave
PROTO_DIR = ../proto

build: $(HEADERS) *.cpp *.h ../../build
	@ echo "Compilers used: $(CC), $(CXX)"
	@ echo "Crypto libraries: $(CRYPTO_LDFLAGS)"
	@ mkdir -p $(BLD_SUBDIR)
	$(PROTOC) -I$(PROTO_DIR) --cpp_out=. $(PROTO_DIR)/capsule.proto
	$(PROTOC) -I$(PROTO_DIR) --cpp_out=. $(PROTO_DIR)/request.proto
	$(PROTOC) -I$(PROTO_DIR) --cpp_out=. $(PROTO_DIR)/dc_record.proto
	$(PROTOC) -I$(PROTO_DIR) --cpp_out=. $(PROTO_DIR)/wal.proto
	# $(CXX) -g -c $(CXXFLAGS) $(PROTO_INCL)  -DOE_API_VERSION=2 cache.cpp -o $(BLD_SUBDIR)/cache.o
	# $(CXX) -g -c $(CXXFLAGS) $(PROTO_INCL)  -DOE_API_VERSION=2 crypto.cpp -o $(BLD_SUBDIR)/crypto.o
	# $(CXX) -g -c $(CXXFLAGS) $(PROTO_INCL)  -DOE_API_VERSION=2 local.cpp -o $(BLD_SUBDIR)/local.o
	# $(CXX) -g -c $(CXXFLAGS) $(PROTO_INCL)  -DOE_API_VERSION=2 dc.cpp -o $(BLD_SUBDIR)/dc.o
	# $(CXX) -g -c $(CXXFLAGS) $(PROTO_INCL)  -DOE_API_VERSION=2 ecall.cpp -o $(BLD_SUBDIR)/ecall.o	
	$(CXX) -g -c $(CXXFLAGS) $(PROTO_INCL)  -DOE_API_VERSION=2 memtable_stage.cpp -o $(BLD_SUBDIR)/memtable_stage.o
	$(CXX) -g -c $(CXXFLAGS) $(PROTO_INCL)  -DOE_API_VERSION=2 init_queues.cpp -o $(BLD_SUBDIR)/init_queues.o
	$(CXX) -g -c $(CXXFLAGS) $(PROTO_INCL)  -DOE_API_VERSION=2 memtable.cpp -o $(BLD_SUBDIR)/memtable.o
	$(CXX) -g -c $(CXXFLAGS) $(PROTO_INCL)  -DOE_API_VERSION=2 kvs.cpp -o $(BLD_SUBDIR)/kvs.o
	# $(CXX) -g -c $(CXXFLAGS) $(PROTO_INCL)  -DOE_API_VERSION=2 capsule.pb.cc -o $(BLD_SUBDIR)/capsule.pb.o
	# $(CXX) -g -c $(CXXFLAGS) $(PROTO_INCL)  -DOE_API_VERSION=2 request.pb.cc -o $(BLD_SUBDIR)/request.pb.o
	# $(CXX) -g -c $(CXXFLAGS) $(PROTO_INCL)  -DOE_API_VERSION=2 dc_record.pb.cc -o $(BLD_SUBDIR)/dc_record.pb.o
	# $(CXX) -g -c $(CXXFLAGS) $(PROTO_INCL)  -DOE_API_VERSION=2 ../config.cpp -o $(BLD_SUBDIR)/config.o
	$(CXX) -g -c $(CXXFLAGS) $(PROTO_INCL)  -DOE_API_VERSION=2 ../util/MurMurHash3.cpp -o $(BLD_SUBDIR)/MurMurHash3.o
	$(CXX) -g -c $(CXXFLAGS) $(PROTO_INCL)  -DOE_API_VERSION=2 ../util/bloom_filter.cpp -o $(BLD_SUBDIR)/bloom_filter.o
	$(CXX) -g -c $(CXXFLAGS) $(PROTO_INCL)  -DOPENENCLAVE_SEMAPHORE=1 -DOE_API_VERSION=2 queue_generator.cpp -o $(BLD_SUBDIR)/queue_generator.o

	$(CC) -g -c $(CFLAGS) -DOE_API_VERSION=2 cdb_t.c -o $(BLD_SUBDIR)/cdb_t.o
	# $(CXX) -o $(BLD_DIR)/cdbenc \
	# $(BLD_SUBDIR)/cdb_t.o $(BLD_SUBDIR)/cache.o $(BLD_SUBDIR)/crypto.o $(BLD_SUBDIR)/local.o $(BLD_SUBDIR)/dc.o \
	# $(BLD_SUBDIR)/ecall.o $(BLD_SUBDIR)/memtable.o $(BLD_SUBDIR)/kvs.o \
	# $(BLD_SUBDIR)/capsule.pb.o $(BLD_SUBDIR)/request.pb.o $(BLD_SUBDIR)/dc_record.pb.o $(PROTO_LIB) \
	# $(BLD_SUBDIR)/config.o $(BLD_SUBDIR)/MurMurHash3.o $(BLD_SUBDIR)/bloom_filter.o $(LDFLAGS) $(CRYPTO_LDFLAGS)

	$(CXX) -o $(BLD_DIR)/cdbenc \
	$(BLD_SUBDIR)/cdb_t.o \
	$(BLD_SUBDIR)/init_queues.o $(BLD_SUBDIR)/memtable_stage.o $(BLD_SUBDIR)/queue_generator.o \
	$(BLD_SUBDIR)/memtable.o $(BLD_SUBDIR)/kvs.o \
	$(PROTO_LIB) \
	$(BLD_SUBDIR)/MurMurHash3.o $(BLD_SUBDIR)/bloom_filter.o $(LDFLAGS) $(CRYPTO_LDFLAGS)

sign:
	# oesign sign -e $(BLD_DIR)/cdbenc -c cdb.conf -k $(BLD_SUBDIR)/private.pem

clean:
	rm -f $(BLD_SUBDIR)/* $(HEADERS) *.pb.cc *.pb.h

keys:
	openssl genrsa -out $(BLD_SUBDIR)/private.pem -3 3072
	openssl rsa -in $(BLD_SUBDIR)/private.pem -pubout -out $(BLD_SUBDIR)/public.pem


.PHONY: hmk
hmk: $(HEADERS)

