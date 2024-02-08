// cpu_MatrixMultiplication.cpp
#include "cpu_MatrixMultiplication.h"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iterator>
#include <omp.h>
#include <iostream>
#include <vector>
#include <jni.h>

// Set tile size
const int TILE_SIZE = 128;

// Utility function declarations
std::vector<std::vector<float>> jobjectArrayToVector(JNIEnv* env, jobjectArray array);
jobjectArray vectorToJobjectArray(JNIEnv* env, const std::vector<std::vector<float>>& matrix);

// JNI function implementation
extern "C" JNIEXPORT jobjectArray JNICALL
Java_com_example_barge_MainActivity_runMatrixMultiplication(
        JNIEnv* env,
        jobject /* this */,
        jobjectArray matrix1,
        jobjectArray matrix2,
        jint numThreads) {

    // Convert Java matrices to C++ vectors
    std::vector<std::vector<float>> mat1 = jobjectArrayToVector(env, matrix1);
    std::vector<std::vector<float>> mat2 = jobjectArrayToVector(env, matrix2);
    std::vector<std::vector<float>> result(mat1.size(), std::vector<float>(mat2[0].size(), 0.0f)); // Initialize result matrix

    // Multiply matrices using OpenMP
    multiplyMatrices(mat1, mat2, result, numThreads);

    // Convert the result matrix back to a Java array
    jobjectArray resultArray = vectorToJobjectArray(env, result);

    return resultArray;
}

// Converts a jobjectArray (2D Java array) to a std::vector<std::vector<float>>
std::vector<std::vector<float>> jobjectArrayToVector(JNIEnv* env, jobjectArray array) {
    std::vector<std::vector<float>> result;

    jsize rows = env->GetArrayLength(array);
    for (jsize i = 0; i < rows; ++i) {
        jfloatArray oneDim = (jfloatArray)env->GetObjectArrayElement(array, i);
        jsize cols = env->GetArrayLength(oneDim);
        std::vector<float> temp(cols);
        env->GetFloatArrayRegion(oneDim, 0, cols, &temp[0]);
        result.push_back(temp);
        env->DeleteLocalRef(oneDim);
    }

    return result;
}

// Converts a std::vector<std::vector<float>> to a jobjectArray (2D Java array)
jobjectArray vectorToJobjectArray(JNIEnv* env, const std::vector<std::vector<float>>& matrix) {
    jsize rows = matrix.size();
    jclass floatArrayClass = env->FindClass("[F");
    jobjectArray resultArray = env->NewObjectArray(rows, floatArrayClass, nullptr);

    for (jsize i = 0; i < rows; ++i) {
        jsize cols = matrix[i].size();
        jfloatArray floatArray = env->NewFloatArray(cols);
        env->SetFloatArrayRegion(floatArray, 0, cols, &matrix[i][0]);
        env->SetObjectArrayElement(resultArray, i, floatArray);
        env->DeleteLocalRef(floatArray);
    }

    return resultArray;
}

// Function to multiply two matrices with tiling
void multiplyMatrices(const std::vector<std::vector<float>>& mat1, const std::vector<std::vector<float>>& mat2, std::vector<std::vector<float>>& result, int numThreads) {
    int rows1 = mat1.size();
    int cols1 = mat1[0].size();
    int cols2 = mat2[0].size();

    omp_set_num_threads(numThreads); // Set number of threads for OpenMP

    // Initialize result matrix with zeros
    for (auto& row : result) {
        fill(row.begin(), row.end(), 0.0f);
    }

#pragma omp parallel for collapse(2)
    for (int i = 0; i < rows1; i += TILE_SIZE) {
        for (int j = 0; j < cols2; j += TILE_SIZE) {
            for (int k = 0; k < cols1; k += TILE_SIZE) {
                for (int ii = i; ii < std::min(i + TILE_SIZE, rows1); ++ii) {
                    for (int jj = j; jj < std::min(j + TILE_SIZE, cols2); ++jj) {
                        float sum = result[ii][jj];
                        for (int kk = k; kk < std::min(k + TILE_SIZE, cols1); ++kk) {
                            sum += mat1[ii][kk] * mat2[kk][jj];
                        }
                        result[ii][jj] = sum;
                    }
                }
            }
        }
    }
}
