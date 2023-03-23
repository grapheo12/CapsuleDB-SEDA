from sys import argv
from uuid import uuid4
import random
import numpy as np
import string

key_size=16
max_val_len=1000
dist = "zipf"
zipf_const = 2
warmup = True
try:
    wr_ratio = float(argv[1])
    unique_keys = int(argv[2])
    min_reqs = int(argv[3])
except Exception:
    print(f"Usage: {argv[0]} <write-to-read-ratio> <#-unique-keys> <min-requests>")
    exit(0)

keys = set()
while len(keys) < unique_keys:
    k = str(uuid4())[:key_size]
    k = k.replace("-", "x")
    keys.add(k)

key_list = list(keys)
req_cnt = 0

vals = ["READ", "WRITE"]
probs = [1-wr_ratio, wr_ratio]
arr = np.random.choice(vals, 10 * min_reqs, p=probs)

finp = open("inp.txt", "w")
fout = open("ans.txt", "w")

sim = dict()

key_idx = []
if dist == "zipf":
    key_idx = np.random.default_rng().zipf(zipf_const, size=min_reqs)

if warmup:
    letters = string.ascii_lowercase
    v = ''.join(random.choice(letters) for i in range(max_val_len-1))
    for k in key_list:
        #k = key_list[i]
        finp.write(f"WRITE {k} {v}\n")    
        fout.write(f"WRITE_PASS {k} {v}\n")
        sim[k] = v

random.shuffle(key_list)

while len(keys) != 0 and req_cnt < min_reqs:
    if dist == "uniform": 
        k = random.choice(key_list)
    elif dist == "zipf":
        k = key_list[(key_idx[req_cnt] - 1)%len(key_list)]
    else:
        exit() 
    #if k in keys:
    #    keys.remove(k)

    req = arr[req_cnt % len(arr)]
    if req == "WRITE":
        v = ''.join(random.choices(string.ascii_lowercase +
                             string.digits, k=max_val_len))
		#v = str(uuid4()) + "-" + str(uuid4())
        lenrand = random.randint(1, max_val_len-1)
       
        v = v[:lenrand]
        finp.write(f"WRITE {k} {v}\n")
        
        fout.write(f"WRITE_PASS {k} {v}\n")
        sim[k] = v
    else:
        finp.write(f"READ {k}\n")
        if k in sim and len(sim[k]) > 0:
            fout.write(f"READ_PASS {sim[k]}\n")
        else:
            fout.write("READ_FAIL\n")

    req_cnt += 1

finp.write("END\n")

for (k, v) in sorted(sim.items(), key=lambda x: x[0]):
    print(k, v)

finp.close()
fout.close()
    






