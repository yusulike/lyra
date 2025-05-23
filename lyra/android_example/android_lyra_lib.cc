// Copyright 2021 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <jni.h>

#include <string>
#include <vector>

#include "absl/random/random.h"
#include "lyra/cli_example/decoder_main_lib.h"
#include "lyra/cli_example/encoder_main_lib.h"
#include "lyra/lyra_benchmark_lib.h"
#include "lyra/lyra_config.h"

#ifdef __ANDROID__

#include <android/log.h>

#define LOG_TAG "ENCODER_MAIN_LIB"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#else

#define LOGI(...)
#define LOGE(...)

#endif


namespace chromemedia {
namespace codec {

/**
 * @brief Initialize the Lyra audio encoder
 * 
 * @param sample_rate_hz Sample rate in Hz (e.g., 16000)
 * @param num_channels Number of audio channels (1 for mono)
 * @param bitrate Target encoding bitrate in bits per second
 * @param enable_dtx Enable discontinuous transmission for silence periods
 * @param model_path Path to the directory containing model files
 * @return true if initialization succeeded, false otherwise
 */
bool lyra_encoder_initialize(int sample_rate_hz, int num_channels, int bitrate,
                                      bool enable_dtx,
                                      const std::string& model_path) {
    LOGI("initialize_encoder called with sample_rate_hz: %d, num_channels: %d, bitrate: %d, enable_dtx: %d, model_path: %s",
         sample_rate_hz, num_channels, bitrate, enable_dtx, model_path.c_str());
    initialize_encoder(sample_rate_hz, num_channels, bitrate, enable_dtx, model_path);
    return true;
}

/**
 * @brief Release resources used by the Lyra encoder
 */
void lyra_encoder_release() {
    LOGI("release_encoder called");
    release_encoder();
}

/**
 * @brief Initialize the Lyra audio decoder
 * 
 * @param sample_rate_hz Sample rate in Hz (e.g., 16000)
 * @param num_channels Number of audio channels (1 for mono)
 * @param model_path Path to the directory containing model files
 * @return true if initialization succeeded, false otherwise
 */
bool lyra_decoder_initialize(int sample_rate_hz, int num_channels,
                                      const std::string& model_path) {
    LOGI("initialize_decoder called with sample_rate_hz: %d, num_channels: %d, model_path: %s",
         sample_rate_hz, num_channels, model_path.c_str());
    initialize_decoder(sample_rate_hz, num_channels, model_path);
    return true;
}

/**
 * @brief Release resources used by the Lyra decoder
 */
void lyra_decoder_release() {
    LOGI("release_decoder called");
    release_decoder();
}

/**
 * @brief Change the target bitrate of the encoder
 * 
 * @param bitrate New target bitrate in bits per second
 */
void lyra_set_bitrate(int bitrate) {
    set_bitrate_encoder(bitrate);
}

/**
 * @brief Encode raw audio samples into Lyra compressed format
 * 
 * @param wav_data Input audio samples in PCM format
 * @param sample_rate_hz Sample rate of the input audio in Hz
 * @param enable_preprocessing Enable audio preprocessing before encoding
 * @param[out] encoded_features Output buffer for compressed audio data
 * @return true if encoding succeeded, false otherwise
 */
bool lyra_encode_wav(const std::vector<int16_t>& wav_data,
               int sample_rate_hz, 
               bool enable_preprocessing,
               std::vector<uint8_t>* encoded_features) {

    return chromemedia::codec::encodeWav(
             wav_data, 
             sample_rate_hz,
             enable_preprocessing, 
             encoded_features);
}

/**
 * @brief Decode Lyra compressed data back to audio samples
 * 
 * @param packet_stream Input compressed data
 * @param bit_rate Bitrate of the encoded stream
 * @param[out] decoded_audio Output buffer for decoded PCM audio samples
 * @return true if decoding succeeded, false otherwise
 */
bool lyra_decode_features(const std::vector<uint8_t>& packet_stream, int bitrate,
                    std::vector<int16_t>* decoded_audio){
    absl::BitGen gen;

    return chromemedia::codec::decodeFeatures(
            packet_stream, chromemedia::codec::BitrateToPacketSize(bitrate),
            false, gen,
            nullptr, decoded_audio);
}

/**
 * @brief Encode a WAV file into Lyra compressed format
 * 
 * @param wav_path Path to input WAV file
 * @param output_path Path where compressed file will be saved
 * @param bitrate Target encoding bitrate in bits per second
 * @param enable_preprocessing Enable audio preprocessing
 * @param enable_dtx Enable discontinuous transmission
 * @param model_path Path to model files directory
 * @return true if file encoding succeeded, false otherwise
 */
bool lyra_encode_file(const std::string& wav_path,
    const std::string& output_path, int bitrate,
    bool enable_preprocessing, bool enable_dtx,
    const std::string& model_path) {
    return chromemedia::codec::EncodeFile(wav_path, output_path, bitrate, enable_preprocessing, enable_dtx, model_path);
}

/**
 * @brief Decode a Lyra compressed file back to WAV format
 * 
 * @param encoded_path Path to input Lyra compressed file
 * @param output_path Path where decoded WAV file will be saved
 * @param sample_rate_hz Sample rate for the output audio in Hz
 * @param bitrate Bitrate of the encoded file
 * @param model_path Path to model files directory
 * @return true if file decoding succeeded, false otherwise
 */
bool lyra_decode_file(const std::string& encoded_path,
    const std::string& output_path, int sample_rate_hz,
    int bitrate,
    const std::string& model_path) {
    return  chromemedia::codec::DecodeFile(
            encoded_path, output_path, sample_rate_hz, bitrate,
            false, 0.0, 1.0, chromemedia::codec::PacketLossPattern({}, {}),
            model_path);
}

} // codec
} // chromedia
