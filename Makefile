OPENSSL := openssl

KEYS = build/keys/enclave_sign_priv.pem build/keys/enclave_sign_pub.pem build/keys/enclave_enc.key

build: $(shell find src -type f) $(KEYS)
	mkdir -p build
	$(MAKE) -C src

$(KEYS):
	mkdir -p build/keys
	openssl ecparam -name secp256k1 -genkey -noout -out build/keys/enclave_sign_priv.pem
	openssl ec -in build/keys/enclave_sign_priv.pem -pubout > build/keys/enclave_sign_pub.pem
	openssl enc -aes-256-cbc -iter 10 -k secret -P > build/keys/enclave_enc.key


.PHONY: clean
clean:
	$(MAKE) -C src/host clean
	$(MAKE) -C src/enclave clean
	rm -rf build

.PHONY: tests
tests: build
	$(MAKE) -C tests
	$(MAKE) -C tests clean
	