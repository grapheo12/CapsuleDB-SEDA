#include <cassert>
#include "memtable.h"
#include <iostream>
#include <stdio.h>
#include <cassert>

MemTable::MemTable(uint64_t max_size_in_bytes)
    :size_in_bytes_(0),
    mutable_(true),
    on_flush_(false),
    max_size_in_bytes_(max_size_in_bytes)
{
    // store_.reserve(MT_TH);
}


KV_Status MemTable::Put(const std::string &key, const std::string &value)
{
    assert(mutable_);
    //if (!mutable_)
    //    return WRITE_FAIL;

    bool new_key = false;
    if (store_.find(key) == store_.end())
        new_key = true;
    
    uint64_t old_sz = 0;
    if (!new_key){
        old_sz = store_[key].size();
    }

    store_[key] = value;

    if (new_key) { 
        size_in_bytes_ += key.size() + value.size();
		if (isFull()) {
			mutable_ = false;
            return JUST_FULL;
        }
	}else{
        size_in_bytes_ += value.size() - old_sz;
    }

    return WRITE_PASS;
}


KV_Status MemTable::Get(const std::string &key, std::string &value) {
	//std::cerr << "MemTable::Get" << std::endl;
    
    if (store_.find(key) == store_.end()){
        return READ_FAIL;
    }

    value = store_[key];     // Zero-copy I/O; Make sure only one threads accesses store_ at a time.
    return READ_PASS;
}

bool MemTable::IsEmpty() const {
    return size_in_bytes_ == 0;
}

/* current implementation is based on naive element counting 
* To be precise, we need to use bytes. */
bool MemTable::isFull() const {
	//std::cerr << "Counts: " << size_in_counts_ << std::endl;
    return size_in_bytes_ >= max_size_in_bytes_;
}

bool MemTable::IsMutable() const {
	return mutable_;
}

 void MemTable::SetMutable(bool mut) {

    mutable_ = mut;
}

bool MemTable::InitFlush() {
    if (on_flush_)
        return false;

    if (mutable_)
        return false;

    on_flush_ = true;

	//std::cerr << "InitFlush True" << std::endl;
    return true;
}

bool MemTable::EndFlush() {
    store_.clear();
    size_in_bytes_ = 0;
    on_flush_ = false;
	mutable_ = true;

	//std::cerr << "[Enclave] MemTable::EndFlush done" << std::endl;
    return true;
}

std::vector<std::pair<std::string, std::string>> MemTable::Dump() {
	//std::cerr << "MemTable::Dump" << std::endl;
	//std::cerr << "MemTable::Dump: store size is " << store_.size() << "/" << size_in_counts_  << std::endl;
    std::vector<std::pair<std::string, std::string>> v(store_.size());

    std::copy(store_.begin(), store_.end(), v.begin());

	//std::cerr << "MemTable::Dump End" << std::endl;
    return v;
}
