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
BLD_SUBDIR = $(BLD_DIR)/host
PROTO_DIR = ../proto

$(HEADERS): ../cdb.edl
	oeedger8r ../cdb.edl --untrusted \
		--search-path $(INCDIR) \
		--search-path $(INCDIR)/openenclave/edl/sgx

build: $(HEADERS)
	@ echo "Compilers used: $(CC), $(CXX)"
	@ mkdir -p $(BLD_SUBDIR)
	$(PROTOC) -I$(PROTO_DIR) --cpp_out=. $(PROTO_DIR)/capsule.proto
	$(PROTOC) -I$(PROTO_DIR) --cpp_out=. $(PROTO_DIR)/request.proto
	$(PROTOC) -I$(PROTO_DIR) --cpp_out=. $(PROTO_DIR)/wal.proto
	$(PROTOC) -I$(PROTO_DIR) --cpp_out=. $(PROTO_DIR)/net_request.proto
	# $(CXX) -g -c $(CXXFLAGS) $(GRPC_INCDIR) globals.cpp -o $(BLD_SUBDIR)/globals.o
	# $(CXX) -g -c $(CXXFLAGS) $(GRPC_INCDIR) ocall.cpp -o $(BLD_SUBDIR)/ocall.o
	# $(CXX) -g -c $(CXXFLAGS) $(GRPC_INCDIR) flush_worker.cpp -o $(BLD_SUBDIR)/flush_worker.o
	# $(CXX) -g -c $(CXXFLAGS) $(GRPC_INCDIR) rw_worker.cpp -o $(BLD_SUBDIR)/rw_worker.o
	$(CXX) -g -c $(CXXFLAGS) $(GRPC_INCDIR) host.cpp -o $(BLD_SUBDIR)/host.o
	$(CXX) -g -c $(CXXFLAGS) $(GRPC_INCDIR) connection_handler.cpp -o $(BLD_SUBDIR)/connection_handler.o
	$(CXX) -g -c $(CXXFLAGS) $(GRPC_INCDIR) ../util/queue_generator.cpp -o $(BLD_SUBDIR)/queue_generator.o
	# $(CC) -g -c $(CFLAGS)  cdb_u.c -o $(BLD_SUBDIR)/cdb_u.o
	# $(CXX) -g -c $(CXXFLAGS) $(GRPC_INCDIR) capsule.pb.cc -o $(BLD_SUBDIR)/capsule.pb.o
	# $(CXX) -g -c $(CXXFLAGS) $(GRPC_INCDIR) request.pb.cc -o $(BLD_SUBDIR)/request.pb.o
	# $(CXX) -g -c $(CXXFLAGS) $(GRPC_INCDIR) dc_client.cc -o $(BLD_SUBDIR)/dc_client.o
	# $(CXX) -g -c $(CXXFLAGS) $(GRPC_INCDIR) client_comm.cc -o $(BLD_SUBDIR)/client_comm.o
	# $(CXX) -g -c $(CXXFLAGS) $(GRPC_INCDIR) ../util/logging.cc -o $(BLD_SUBDIR)/logging.o
	# $(CXX) -g -c $(CXXFLAGS) $(GRPC_INCDIR) ../config.cpp -o $(BLD_SUBDIR)/config.o
	# $(CXX) -g -c $(CXXFLAGS) $(GRPC_INCDIR) towncrier/network.pb.cc -o $(BLD_SUBDIR)/network.pb.o
	# $(CXX) -g -c $(CXXFLAGS) $(GRPC_INCDIR) towncrier/network.grpc.pb.cc -o $(BLD_SUBDIR)/network.grpc.pb.o
	$(CXX) -g -c $(CXXFLAGS) $(GRPC_INCDIR) net_request.pb.cc -o $(BLD_SUBDIR)/net_request.pb.o


	# $(CXX) -o $(BLD_DIR)/cdbhost \
	# $(BLD_SUBDIR)/cdb_u.o $(BLD_SUBDIR)/ocall.o \
	# $(BLD_SUBDIR)/globals.o $(BLD_SUBDIR)/flush_worker.o $(BLD_SUBDIR)/rw_worker.o \
	# $(BLD_SUBDIR)/host.o \
	# $(BLD_SUBDIR)/capsule.pb.o $(BLD_SUBDIR)/request.pb.o $(PROTO_LIB) $(GRPC_LIB) \
	# $(BLD_SUBDIR)/dc_client.o $(BLD_SUBDIR)/client_comm.o $(BLD_SUBDIR)/logging.o \
	# $(BLD_SUBDIR)/network.pb.o $(BLD_SUBDIR)/network.grpc.pb.o \
	# $(BLD_SUBDIR)/config.o $(LDFLAGS) -lstdc++ -lc -lzmq

	$(CXX) -o $(BLD_DIR)/cdbhost \
	$(BLD_SUBDIR)/host.o $(BLD_SUBDIR)/queue_generator.o $(BLD_SUBDIR)/net_request.pb.o $(PROTO_LIB) $(GRPC_LIB) \
	$(BLD_SUBDIR)/connection_handler.o $(LDFLAGS) -lstdc++ -lc -lzmq


clean:
	rm -f $(BLD_SUBDIR)/* $(HEADERS) *.pb.cc *.pb.h

.PHONY: hmk
hmk: $(HEADERS)
