//
// Created by WangYingnan on 3/12/17.
//

#ifndef RSKERNELSTEST_RSMATMUL_TEST_H
#define RSKERNELSTEST_RSMATMUL_TEST_H

#include "rsMatmul.h"
#include "rsMatmul_test_data.h"

namespace matmultest {

namespace sgemm {

#define DATA_TYPE float

void getLargeData(DATA_TYPE*& a, DATA_TYPE*& b, int m, int n, int k)
{
    auto a_raw = getA();
    auto b_raw = getB();

    a = new DATA_TYPE[m*k];
    b = new DATA_TYPE[n*k];

    for(int i=0;i<m*k;++i){
        a[i] = (DATA_TYPE)a_raw[i];
    }
    for(int i=0;i<n*k;++i){
        b[i] = (DATA_TYPE)b_raw[i];
    }
}

void getRefResult(DATA_TYPE* a, DATA_TYPE* b, void*& c, int m, int n, int k)
{
    c = new DATA_TYPE[m*n];
    DATA_TYPE* c_casted = static_cast<DATA_TYPE*>(c);

    for (int j = 0; j < n; j++) {
        for (int i = 0; i < m; i++) {
            DATA_TYPE total = 0.f;
            for (int l = 0; l < k; l++) {
                int a_index = ((i * k) + l);
                int b_index = ((l * n) + j);
                DATA_TYPE mult = a[a_index] * b[b_index];
                total += mult;
            }
            int c_index = ((i * n) + j);
            c_casted[c_index] = total;
        }
    }
}

DATA_TYPE calcL2Norm(DATA_TYPE* input, int sz)
{
    float l2Norm = 0.f;
    for (int i = 0; i < sz; ++i) {
        l2Norm += input[i] * input[i];
    }
    return l2Norm;
}

bool testWithTolerance(void* out, void* ref, int m, int n)
{
    DATA_TYPE* casted_out = static_cast<DATA_TYPE*>(out);
    DATA_TYPE* casted_ref = static_cast<DATA_TYPE*>(ref);

    DATA_TYPE l2NormOut = calcL2Norm(casted_out, m*n);
    DATA_TYPE l2NormRef = calcL2Norm(casted_ref, m*n);

    DATA_TYPE tolerance = ALLOWED_ERROR * (l2NormOut < l2NormRef ? l2NormOut : l2NormRef);
    tolerance /= m * n;

    for (int i = 0; i < m*n; ++i) {
        DATA_TYPE err = casted_out[i] - casted_ref[i];
        DATA_TYPE absErr = err * err;
        if (absErr > tolerance) {
            return false;
        }
    }
    return true;
}

void SmallTest(const char * path, bool isValid)
{
    int m=2, n=4, k=3;
    int a_sz = m*k, b_sz = n*k, c_sz = m*n;
    DATA_TYPE* a_ori = new DATA_TYPE[a_sz];
    DATA_TYPE* b_ori = new DATA_TYPE[b_sz];

    int a_data[] = {
                1, 2, 3,
                4, 5, 6,
    };
    int b_data[] = {
                11, 7, 3,
                10, 6, 2,
                9, 5, 1,
                8, 4, 0,
    };

    for(int i=0;i<a_sz;++i){
        a_ori[i] = a_data[i];
    }
    for(int i=0;i<b_sz;++i){
        b_ori[i] = b_data[i];
    }

    void *c_out;
    rsMatmul_sgemm(path, static_cast<void*>(a_ori), static_cast<void*>(b_ori), c_out, m, n, k);

    if(isValid){
        void* c_ref;
        getRefResult(a_ori, b_ori, c_ref, m, n, k);

        if(!testWithTolerance(c_out, c_ref, m, n)){
            LOGE("sgemm small test failed!");
        }else{
            LOGI("sgemm small test passed!");
        }
        delete[] static_cast<DATA_TYPE*>(c_ref);
    }

    delete[] a_ori;
    delete[] b_ori;
    delete[] static_cast<DATA_TYPE*>(c_out);
}

void MediumTest(const char * path, bool isValid)
{
    int m=7, n=9, k=23;
    int a_sz = m*k, b_sz = n*k, c_sz = m*n;
    DATA_TYPE* a_ori = new DATA_TYPE[a_sz];
    DATA_TYPE* b_ori = new DATA_TYPE[b_sz];

    int a_data[] = {
                1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
                23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1,
                1, 23, 2, 22, 3, 21, 4, 20, 5, 19, 6, 18, 7, 17, 8, 16, 9, 15, 10, 14, 11, 13, 12,
                23, 1, 22, 2, 21, 3, 20, 4, 19, 5, 18, 6, 17, 7, 16, 8, 15, 9, 14, 10, 13, 11, 12,
                1, 1, 1, 1, 1, 1, 1, 1, 1, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
                3, 1, 4, 1, 5, 8, 2, 3, 1, 14, 11, 15, 18, 12, 13, 11, 14, 11, 15, 18, 12, 13, 11,
                8, 0, 5, 8, 1, 3, 7, 5, 7, 13, 10, 23, 13, 11, 17, 23, 12, 19, 17, 13, 14, 10, 19,
    };
    int b_data[] = {
                0, 2, 4, 6, 8, 10, 1, 3, 5, 7, 9, 11, 0, 2, 4, 6, 8, 10, 1, 3, 5, 7, 9,
                0, 20, 40, 60, 80, 10, 11, 13, 15, 17, 19, 21, 10, 12, 14, 6, 8, 10, 1, 3, 5, 7, 9,
                1, 21, 41, 61, 81, 11, 12, 14, 16, 18, 20, 22, 11, 13, 15, 7, 9, 11, 2, 4, 6, 8, 9,
                0, 19, 39, 59, 79, 9, 10, 12, 14, 16, 18, 20, 9, 11, 13, 5, 7, 9, 0, 2, 4, 6, 8,
                2, 22, 42, 62, 82, 12, 13, 15, 17, 19, 21, 23, 12, 14, 16, 8, 9, 12, 3, 5, 7, 9, 9,
                0, 18, 38, 58, 78, 8, 9, 11, 13, 15, 17, 19, 8, 10, 12, 4, 6, 8, 0, 1, 3, 5, 7,
                3, 23, 43, 63, 83, 13, 14, 16, 18, 20, 22, 24, 13, 15, 17, 9, 9, 13, 4, 6, 8, 9, 9,
                0, 17, 37, 57, 77, 7, 8, 10, 12, 14, 16, 18, 7, 9, 11, 3, 5, 7, 0, 0, 2, 4, 6,
                10, 20, 30, 40, 50, 1, 2, 3, 4, 5, 11, 12, 13, 14, 15, 21, 22, 23, 24, 25, 1, 2, 3,
    };

    for(int i=0;i<a_sz;++i){
        a_ori[i] = a_data[i];
    }
    for(int i=0;i<b_sz;++i){
        b_ori[i] = b_data[i];
    }

    void *c_out;
    rsMatmul_sgemm(path, static_cast<void*>(a_ori), static_cast<void*>(b_ori), c_out, m, n, k);

    if(isValid){
        void* c_ref;
        getRefResult(a_ori, b_ori, c_ref, m, n, k);

        if(!testWithTolerance(c_out, c_ref, m, n)){
            LOGE("sgemm medium test failed!");
        }else{
            LOGI("sgemm medium test passed!");
        }
        delete[] static_cast<DATA_TYPE*>(c_ref);
    }

    delete[] a_ori;
    delete[] b_ori;
    delete[] static_cast<DATA_TYPE*>(c_out);
}

void LargeTest(const char * path, bool isValid)
{
    int m=256, n=192, k=1152;
    int a_sz = m*k, b_sz = n*k, c_sz = m*n;
    DATA_TYPE *a_ori, *b_ori;
    getLargeData(a_ori, b_ori, m, n, k);

    void *c_out;
    rsMatmul_sgemm(path, static_cast<void*>(a_ori), static_cast<void*>(b_ori), c_out, m, n, k);

    if(isValid){
        void* c_ref;
        getRefResult(a_ori, b_ori, c_ref, m, n, k);

        if(!testWithTolerance(c_out, c_ref, m, n)){
            LOGE("sgemm large test failed!");
        }else{
            LOGI("sgemm large test passed!");
        }
        delete[] static_cast<DATA_TYPE*>(c_ref);
    }

    delete[] a_ori;
    delete[] b_ori;
    delete[] static_cast<DATA_TYPE*>(c_out);
}

#undef DATA_TYPE
}










}

#endif //RSKERNELSTEST_RSMATMUL_TEST_H