#pragma once

#include <string>
#include <sstream>

/* Creates a dummy encoding of bytes to alphanumeric characters only.
 * Safe as filename.
 * Better use base32 (not base64). Until then, this should suffice. 
 * Probably won't be needed once we start storing in DataCapsules directly.
 */


char __DUMMY_ENCODE_MAP[256][3] = {
    "aa", "ab", "ac", "ad", "ae", "af", "ag", "ah",
    "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7",
    "ba", "bb", "bc", "bd", "be", "bf", "bg", "bh",
    "b0", "b1", "b2", "b3", "b4", "b5", "b6", "b7",
    "ca", "cb", "cc", "cd", "ce", "cf", "cg", "ch",
    "c0", "c1", "c2", "c3", "c4", "c5", "c6", "c7",
    "da", "db", "dc", "dd", "de", "df", "dg", "dh",
    "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7",
    "ea", "eb", "ec", "ed", "ee", "ef", "eg", "eh",
    "e0", "e1", "e2", "e3", "e4", "e5", "e6", "e7",
    "fa", "fb", "fc", "fd", "fe", "ff", "fg", "fh",
    "f0", "f1", "f2", "f3", "f4", "f5", "f6", "f7",
    "ga", "gb", "gc", "gd", "ge", "gf", "gg", "gh",
    "g0", "g1", "g2", "g3", "g4", "g5", "g6", "g7",
    "ha", "hb", "hc", "hd", "he", "hf", "hg", "hh",
    "h0", "h1", "h2", "h3", "h4", "h5", "h6", "h7",
    "ia", "ib", "ic", "id", "ie", "if", "ig", "ih",
    "i0", "i1", "i2", "i3", "i4", "i5", "i6", "i7",
    "ja", "jb", "jc", "jd", "je", "jf", "jg", "jh",
    "j0", "j1", "j2", "j3", "j4", "j5", "j6", "j7", 
    "ka", "kb", "kc", "kd", "ke", "kf", "kg", "kh", 
    "k0", "k1", "k2", "k3", "k4", "k5", "k6", "k7", 
    "la", "lb", "lc", "ld", "le", "lf", "lg", "lh", 
    "l0", "l1", "l2", "l3", "l4", "l5", "l6", "l7", 
    "ma", "mb", "mc", "md", "me", "mf", "mg", "mh", 
    "m0", "m1", "m2", "m3", "m4", "m5", "m6", "m7", 
    "na", "nb", "nc", "nd", "ne", "nf", "ng", "nh", 
    "n0", "n1", "n2", "n3", "n4", "n5", "n6", "n7", 
    "oa", "ob", "oc", "od", "oe", "of", "og", "oh", 
    "o0", "o1", "o2", "o3", "o4", "o5", "o6", "o7", 
    "pa", "pb", "pc", "pd", "pe", "pf", "pg", "ph", 
    "p0", "p1", "p2", "p3", "p4", "p5", "p6", "p7"
};

std::string dummy_encode(unsigned char *hsh, size_t len)
{
    std::stringstream ss;

    for (int i = 0; i < len; i++){
        ss << __DUMMY_ENCODE_MAP[hsh[i]];
    }

    return ss.str();
}

std::string dummy_encode(const char *hsh, size_t len)
{
    std::stringstream ss;

    for (int i = 0; i < len; i++){
        unsigned char single = hsh[i] + 128;
        ss << __DUMMY_ENCODE_MAP[single];
    }

    return ss.str();
}

