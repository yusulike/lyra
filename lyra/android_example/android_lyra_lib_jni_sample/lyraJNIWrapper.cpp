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

extern "C" {

JNIEXPORT jbyteArray JNICALL
Java_ai_onnxruntime_example_hilcodec_lyraWrapper_encodeWav(
        JNIEnv *env, jobject /* thiz */, jshortArray jWavData,
        jint sampleRateHz, jboolean enablePreprocessing) {

    LOGI("encodeWav called with sampleRateHz=%d", sampleRateHz);

    // Convert Java short array to C++ vector
    jsize wavLength = env->GetArrayLength(jWavData);
    std::vector<int16_t> wavData(wavLength);
    env->GetShortArrayRegion(jWavData, 0, wavLength, &wavData[0]);
    LOGI("Wav data length: %d", wavLength);

    // Create output vector for encoded features
    std::vector<uint8_t> encodedFeatures;

    // Call the encoding function
    bool success = lyra_encode_wav(wavData, sampleRateHz,
                                 static_cast<bool>(enablePreprocessing),
                                 &encodedFeatures);

    if (!success) {
        LOGE("Encoding failed");
        return nullptr;
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
                           reinterpret_cast<jbyte*>(encodedFeatures.data()));

    return jEncodedArray;
}

JNIEXPORT jshortArray JNICALL
Java_ai_onnxruntime_example_hilcodec_lyraWrapper_decodeFeature(
        JNIEnv *env, jobject /* thiz */, jbyteArray jEncodedFeatures, jint bitrate) {

    LOGI("decodeFeatures called with bitrate=%d", bitrate);

    // Convert Java byte array to C++ vector
    jsize encodedLength = env->GetArrayLength(jEncodedFeatures);
    std::vector<uint8_t> encodedFeatures(encodedLength);
    env->GetByteArrayRegion(jEncodedFeatures, 0, encodedLength,
                           reinterpret_cast<jbyte*>(&encodedFeatures[0]));
    LOGI("Encoded features size: %d", encodedLength);

    // Create output vector for decoded audio
    std::vector<int16_t> decodedAudio;

    // Call the decoding function
    bool success = lyra_decode_features(encodedFeatures, bitrate, &decodedAudio);

    if (!success) {
        LOGE("Decoding failed");
        return nullptr;
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

    std::string wavPath = jstringToString(env, jWavPath);
    std::string outputPath = jstringToString(env, jOutputPath);
    std::string modelPath = jstringToString(env, jModelPath);

    LOGI("Wav path: %s", wavPath.c_str());
    LOGI("Output path: %s", outputPath.c_str());
    LOGI("Model path: %s", modelPath.c_str());

    // Call the C++ function
    bool success = lyra_encode_file(
            wavPath,
            outputPath,
            static_cast<int>(bitrate),
            static_cast<bool>(enablePreprocessing),
            static_cast<bool>(enableDtx),
            modelPath
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

    std::string encodedPath = jstringToString(env, jEncodedPath);
    std::string outputPath = jstringToString(env, jOutputPath);
    std::string modelPath = jstringToString(env, jModelPath);

    LOGI("Encoded path: %s", encodedPath.c_str());
    LOGI("Output path: %s", outputPath.c_str());
    LOGI("Model path: %s", modelPath.c_str());

    // Call the C++ function to decode the file
    bool success = lyra_decode_file(
            encodedPath,
            outputPath,
            sampleRateHz,
            bitrate,
            modelPath
    );

    if (success) {
        LOGI("File decoding successful");
    } else {
        LOGE("File decoding failed");
    }

    return static_cast<jboolean>(success);
}

JNIEXPORT jboolean JNICALL
Java_ai_onnxruntime_example_hilcodec_lyraWrapper_initializeEncoder(
        JNIEnv *env, jobject /* thiz */, jint sampleRateHz, jint numChannels,
        jint bitrate, jboolean enableDtx, jstring jModelPath) {
    
    LOGI("Initializing encoder with sampleRate=%d, channels=%d, bitrate=%d", 
         sampleRateHz, numChannels, bitrate);
    
    std::string modelPath = jstringToString(env, jModelPath);
    bool success = lyra_encoder_initialize(sampleRateHz, numChannels, bitrate,
                                         static_cast<bool>(enableDtx), modelPath);
    
    if (!success) {
        LOGE("Failed to initialize encoder");
    }
    
    return static_cast<jboolean>(success);
}

JNIEXPORT jboolean JNICALL
Java_ai_onnxruntime_example_hilcodec_lyraWrapper_initializeDecoder(
        JNIEnv *env, jobject /* thiz */, jint sampleRateHz, jint numChannels,
        jstring jModelPath) {
    
    LOGI("Initializing decoder with sampleRate=%d, channels=%d", 
         sampleRateHz, numChannels);
    
    std::string modelPath = jstringToString(env, jModelPath);
    bool success = lyra_decoder_initialize(sampleRateHz, numChannels, modelPath);
    
    if (!success) {
        LOGE("Failed to initialize decoder");
    }
    
    return static_cast<jboolean>(success);
}

JNIEXPORT void JNICALL
Java_ai_onnxruntime_example_hilcodec_lyraWrapper_setBitrate(
        JNIEnv * /* env */, jobject /* thiz */, jint bitrate) {
    LOGI("Setting bitrate to %d", bitrate);
    lyra_set_bitrate(bitrate);
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

