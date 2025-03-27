#include <jni.h>
#include <string>
#include <vector>
#include <android/log.h>
#include "include/android_lyra_lib.h"

using namespace chromemedia::codec;

#define LOG_TAG "LyraWrapper"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// Convert a jstring to a std::string
std::string jstringToString(JNIEnv* env, jstring jStr) {
    if (!jStr) return "";

    const char* cStr = env->GetStringUTFChars(jStr, nullptr);
    std::string result(cStr);
    env->ReleaseStringUTFChars(jStr, cStr);

    return result;
}

// Convert a jstring to a ghc::filesystem::path
ghc::filesystem::path jstringToPath(JNIEnv* env, jstring jStr) {
    return ghc::filesystem::path(jstringToString(env, jStr));
}

extern "C" {

JNIEXPORT jbyteArray JNICALL
Java_ai_onnxruntime_example_hilcodec_lyraWrapper_encodeWav(
        JNIEnv *env, jobject /* thiz */, jshortArray jWavData, jint numChannels,
        jint sampleRateHz, jint bitrate, jboolean enablePreprocessing,
        jboolean enableDtx, jstring jModelPath) {

    LOGI("encodeWav called with numChannels=%d, sampleRateHz=%d, bitrate=%d", numChannels,
         sampleRateHz, bitrate);

    // Convert Java short array to C++ vector
    jsize wavLength = env->GetArrayLength(jWavData);
    std::vector<int16_t> wavData(wavLength);
    env->GetShortArrayRegion(jWavData, 0, wavLength, &wavData[0]);
    LOGI("Wav data length: %d", wavLength);

    // Get model path
    ghc::filesystem::path modelPath = jstringToPath(env, jModelPath);
    LOGI("Model path: %s", modelPath.string().c_str());

    // Create output vector for encoded features
    std::vector<uint8_t> encodedFeatures;

    // Call the C++ function
    bool success = EncodeWav_LYRA(
            wavData,
            numChannels,
            sampleRateHz,
            bitrate,
            enablePreprocessing,
            enableDtx,
            modelPath.string(),
            &encodedFeatures
    );

    if (!success) {
        LOGE("Encoding failed");
        return nullptr; // Return null if encoding fails
    }

    LOGI("Encoding successful, encoded features size: %zu", encodedFeatures.size());

    // Create a new Java byte array and copy the encoded features into it
    jsize encodedSize = static_cast<jsize>(encodedFeatures.size());
    jbyteArray jEncodedArray = env->NewByteArray(encodedSize);
    if (jEncodedArray == nullptr) {
        LOGE("Failed to allocate jbyteArray");
        return nullptr;
    }

    env->SetByteArrayRegion(jEncodedArray, 0, encodedSize,
                            reinterpret_cast<jbyte *>(encodedFeatures.data()));

    return jEncodedArray;
}

JNIEXPORT jshortArray JNICALL
Java_ai_onnxruntime_example_hilcodec_lyraWrapper_decodeFeature(
        JNIEnv *env, jobject /* thiz */, jbyteArray jEncodedFeatures, jint bitrate,
        jstring jModelPath) {

    LOGI("decodeFeatures called with bitrate=%d", bitrate);

    // Convert Java byte array to C++ vector
    jsize encodedLength = env->GetArrayLength(jEncodedFeatures);
    std::vector<uint8_t> encodedFeatures(encodedLength);
    env->GetByteArrayRegion(jEncodedFeatures, 0, encodedLength,
                            (jbyte *) (&encodedFeatures[0])
    );
    LOGI("Encoded features size: %d", encodedLength);

    // Get model path
    ghc::filesystem::path modelPath = jstringToPath(env, jModelPath);
    LOGI("Model path: %s", modelPath.string().c_str());

    // Create output vector for decoded audio
    std::vector<int16_t> decodedAudio;

    // Call the C++ function
    bool success = DecodeFeatures_LYRA(
            encodedFeatures,
            bitrate,
            &decodedAudio,
            modelPath.string()
    );

    if (!success) {
        LOGE("Decoding failed");
        return nullptr; // Return null if decoding fails
    }

    LOGI("Decoding successful, decoded audio size: %zu", decodedAudio.size());

    jshortArray jDecodedArray = env->NewShortArray(decodedAudio.size());
    env->SetShortArrayRegion(jDecodedArray, 0, decodedAudio.size(),
                             &decodedAudio[0]);
    return jDecodedArray;
}

JNIEXPORT jboolean JNICALL
Java_ai_onnxruntime_example_hilcodec_lyraWrapper_encodeFile(
        JNIEnv *env, jobject /* thiz */, jstring jWavPath, jstring jOutputPath,
        jint bitrate, jboolean enablePreprocessing, jboolean enableDtx,
        jstring jModelPath) {

    LOGI("encodeFile called with bitrate=%d", bitrate);

    // Convert Java strings to filesystem paths
    ghc::filesystem::path wavPath = jstringToPath(env, jWavPath);
    ghc::filesystem::path outputPath = jstringToPath(env, jOutputPath);
    ghc::filesystem::path modelPath = jstringToPath(env, jModelPath);

    LOGI("Wav path: %s", wavPath.string().c_str());
    LOGI("Output path: %s", outputPath.string().c_str());
    LOGI("Model path: %s", modelPath.string().c_str());

    // Call the C++ function
    bool success = EncodeFile_LYRA(
            wavPath.string(),
            outputPath.string(),
            static_cast<int>(bitrate),
            static_cast<bool>(enablePreprocessing),
            static_cast<bool>(enableDtx),
            modelPath.string()
    );

    if (success) {
        LOGI("File encoding successful");
    } else {
        LOGE("File encoding failed");
    }

    return static_cast<jboolean>(success);
}

JNIEXPORT jboolean JNICALL
Java_ai_onnxruntime_example_hilcodec_lyraWrapper_decodeFile(
        JNIEnv *env, jobject /* thiz */, jstring jEncodedPath, jstring jOutputPath,
        jint sampleRateHz, jint bitrate, jstring jModelPath) {

    LOGI("decodeFile called with sampleRateHz=%d, bitrate=%d", sampleRateHz, bitrate);

    // Convert Java strings to filesystem paths
    ghc::filesystem::path encodedPath = jstringToPath(env, jEncodedPath);
    ghc::filesystem::path outputPath = jstringToPath(env, jOutputPath);
    ghc::filesystem::path modelPath = jstringToPath(env, jModelPath);

    LOGI("Encoded path: %s", encodedPath.string().c_str());
    LOGI("Output path: %s", outputPath.string().c_str());
    LOGI("Model path: %s", modelPath.string().c_str());

    // Call the C++ function to decode the file
    bool success = DecodeFile_LYRA(
            encodedPath.string(),
            outputPath.string(),
            sampleRateHz,
            bitrate,
            modelPath.string()
    );

    if (success) {
        LOGI("File decoding successful");
    } else {
        LOGE("File decoding failed");
    }

    return static_cast<jboolean>(success);
}

// You might want to add a method to check if the library is properly loaded
JNIEXPORT jboolean JNICALL
Java_ai_onnxruntime_example_hilcodec_lyraWrapper_isLibraryLoaded(
        JNIEnv * /* env */, jobject /* thiz */) {
    LOGI("Checking if library is loaded");
    return JNI_TRUE;
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        LOGE("Failed to get JNIEnv");
        return -1; // Indicate failure to load
    }

    // Optionally, you can do other initialization here, such as registering native methods
    return JNI_VERSION_1_6; // Return the JNI version to indicate successful load
}
}

