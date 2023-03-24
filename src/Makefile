# Copyright (c) Open Enclave SDK contributors.
# Licensed under the MIT License.

.PHONY: all build clean run simulate headers

OE_CRYPTO_LIB := openssl
export OE_CRYPTO_LIB

all: headers build

build:
	mkdir -p build
	$(MAKE) -C enclave
	$(MAKE) -C host
	$(MAKE) -C client
headers:
	$(MAKE) -C enclave hmk
	$(MAKE) -C host hmk

clean:
	$(MAKE) -C enclave clean
	$(MAKE) -C host clean
