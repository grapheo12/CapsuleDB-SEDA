# Copyright (c) Open Enclave SDK contributors.
# Licensed under the MIT License.

include ../config.mk

PROTOC=../../third-party/grpc-bin/bin/protoc
PROTO_LIB=-lprotobuf
GRPC_LIB=

CXXFLAGS=$(shell pkg-config oehost-$(CXX_COMPILER) --cflags) -O2 #-g
CFLAGS=$(shell pkg-config oehost-$(COMPILER) --cflags) -O2 #-g

GRPC_LIB_PATH = $(shell echo "$$(pwd)/../../third-party/grpc-bin")
OE_LDFLAGS=-L/lib64/ $(shell pkg-config oehost-$(CXX_COMPILER) --libs)
GRPC_LDFLAGS=$(shell pkg-config --define-variable=prefix=$(GRPC_LIB_PATH) grpc++ --libs)
LDFLAGS=$(OE_LDFLAGS) $(GRPC_LDFLAGS)
INCDIR=$(shell pkg-config oehost-$(CXX_COMPILER) --variable=includedir)
GRPC_INCDIR=-I../../third-party/grpc-bin/include/
HEADERS = cdb_u.c cdb_u.h cdb_args.h

all: build

BLD_DIR = ../../build
BLD_SUBDIR = $(BLD_DIR)/client
PROTO_DIR = ../proto

build:
	@ echo "Compilers used: $(CC), $(CXX)"
	@ mkdir -p $(BLD_SUBDIR)
	$(PROTOC) -I$(PROTO_DIR) --cpp_out=. $(PROTO_DIR)/net_request.proto
	$(CXX) -g -c $(CXXFLAGS) $(GRPC_INCDIR) client.cpp -o $(BLD_SUBDIR)/client.o
	$(CXX) -g -c $(CXXFLAGS) $(GRPC_INCDIR) net_request.pb.cc -o $(BLD_SUBDIR)/net_request.pb.o

	$(CXX) -o $(BLD_DIR)/cdbclient \
	$(BLD_SUBDIR)/client.o $(BLD_SUBDIR)/net_request.pb.o $(PROTO_LIB) $(GRPC_LIB)\
	$(LDFLAGS) -lstdc++ -lc -lzmq


clean:
	rm -f $(BLD_SUBDIR)/* $(HEADERS) *.pb.cc *.pb.h

.PHONY: hmk
hmk: $(HEADERS)
