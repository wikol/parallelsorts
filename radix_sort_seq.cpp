#include <algorithm>
#include <cstring>
#include "radix_sort.h"

void radix_sort_seq(int* T, unsigned n) {
    
    // xor with sign bit
    int sign_bit = 1 << 31;
    for (int i=0; i<n; i++) {
        T[i] ^= sign_bit;
    }
    unsigned* U = (unsigned*) T; 
    unsigned* U_res = new unsigned[n];
    unsigned* pref_sum = new unsigned[n];
    memset(pref_sum, 0, n*sizeof(unsigned));
    
    for (int i=0; i<32; i++) {
        unsigned ith_bit = 1 << i;
        // prefix_sum for 0s
        pref_sum[0] = U[0] & ith_bit ? 0 : 1;
        for (int j=1; j<n; j++) {
            pref_sum[j] = pref_sum[j-1];
            if (!(U[j] & ith_bit)) {
                ++pref_sum[j];
            }
        }
        int zeros = pref_sum[n-1];
        // rewrite to U_res
        for (int j=0; j<n; j++) {
            if (U[j] & ith_bit) {
                U_res[zeros + j - pref_sum[j]] = U[j];
            } else {
                U_res[pref_sum[j]-1] = U[j];
            }
        }
        // swap U with U_res;
        std::swap(U, U_res);
    }
    // xor back
    for (int i=0; i<n; i++) {
        T[i] ^= sign_bit;
    }
    delete[] U_res;
    delete[] pref_sum;
}


