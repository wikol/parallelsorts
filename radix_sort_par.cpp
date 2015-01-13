#include <omp.h>
#include <algorithm>
#include <cmath>
#include <cstring>
#include "radix_sort.h"

void radix_sort_par(int* T, unsigned n) {
    
    // xor with sign bit
    int sign_bit = 1 << 31;
    
    #pragma omp parallel for firstprivate(sign_bit, T)
    for (int i=0; i<n; i++) {
        T[i] ^= sign_bit;
    }
    
    int nthreads = omp_get_max_threads();
    int th_size = n/nthreads; 
    unsigned* last = new unsigned[nthreads]; 
    
    unsigned* U = (unsigned*) T; 
    unsigned* U_res = new unsigned[n];
    unsigned* pref_sum = new unsigned[n];
    memset(pref_sum, 0, n*sizeof(unsigned));
    
    for (int i=0; i<32; i++) {
        unsigned ith_bit = 1 << i;
        // prefix_sum for 0s
        
        #pragma omp parallel num_threads(nthreads) firstprivate(pref_sum, U, last, nthreads, ith_bit, th_size) 
        {
            int thid = omp_get_thread_num();
            int beg = thid * th_size;
            int end = thid == nthreads-1 ? n: (thid + 1) * th_size;
            pref_sum[beg] = U[beg] & ith_bit ? 0 : 1;
            for (int j=beg+1; j<end; j++) {
                pref_sum[j] = pref_sum[j-1];
                if (!(U[j] & ith_bit)) {
                    ++pref_sum[j];
                }
            }
            last[thid] = pref_sum[end-1];
        } 
        for (int j=1; j<nthreads; j++) {
            last[j] += last[j-1];
        } 
        #pragma omp parallel num_threads(nthreads) firstprivate(pref_sum, last, th_size) 
        {
            int thid = omp_get_thread_num();
            int beg = thid * th_size;
            int end = thid == nthreads-1 ? n: (thid + 1) * th_size;
            int add = thid == 0 ? 0 : last[thid-1];
            for (int j=beg; j<end; j++) {
                pref_sum[j] += add;
            }
        }
        
        int zeros = pref_sum[n-1];
        // rewrite to U_res
        
        #pragma omp parallel for firstprivate(ith_bit, zeros, U, U_res, pref_sum)
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
    #pragma omp parallel for firstprivate(sign_bit, T)
    for (int i=0; i<n; i++) {
        T[i] ^= sign_bit;
    }
    
    delete[] last;
    delete[] U_res;
    delete[] pref_sum;
}
