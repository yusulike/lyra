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


bool Initialize_Encoder_LYRA(int sample_rate_hz, int num_channels, int bitrate,
                                      bool enable_dtx,
                                      const std::string& model_path) {
    LOGI("initialize_encoder called with sample_rate_hz: %d, num_channels: %d, bitrate: %d, enable_dtx: %d, model_path: %s",
         sample_rate_hz, num_channels, bitrate, enable_dtx, model_path.c_str());
    initialize_encoder(sample_rate_hz, num_channels, bitrate, enable_dtx, model_path);
    return true;
}

void Release_Encoder_LYRA() {
    LOGI("release_encoder called");
    release_encoder();
}

bool Initialize_Decoder_LYRA(int sample_rate_hz, int num_channels,
                                      const std::string& model_path) {
    LOGI("initialize_decoder called with sample_rate_hz: %d, num_channels: %d, model_path: %s",
         sample_rate_hz, num_channels, model_path.c_str());
    initialize_decoder(sample_rate_hz, num_channels, model_path);
    return true;
}

void Release_Decoder_LYRA() {
    LOGI("release_decoder called");
    release_decoder();
}

void Set_Bitrate_LYRA(int bitrate) {
  set_bitrate_encoder(bitrate);
}

// Encodes a vector of wav_data into encoded_features.
// Uses the quant files located under |model_path|.
bool EncodeWav_LYRA(const std::vector<int16_t>& wav_data,
               int sample_rate_hz, 
               bool enable_preprocessing,
               std::vector<uint8_t>* encoded_features) {

    return chromemedia::codec::encodeWav(
             wav_data, 
             sample_rate_hz,
             enable_preprocessing, 
             encoded_features);
}

bool DecodeFeatures_LYRA(const std::vector<uint8_t>& packet_stream, int bitrate,
                    std::vector<int16_t>* decoded_audio){
    absl::BitGen gen;

    return chromemedia::codec::decodeFeatures(
            packet_stream, chromemedia::codec::BitrateToPacketSize(bitrate),
            false, gen,
            nullptr, decoded_audio);
}

// Encodes a wav file into an encoded feature file. Encodes num_samples from the
// file at |wav_path| and writes the encoded features out to |output_path|.
// Uses the quant files located under |model_path|.
bool EncodeFile_LYRA(const std::string& wav_path,
    const std::string& output_path, int bitrate,
    bool enable_preprocessing, bool enable_dtx,
    const std::string& model_path) {
    return chromemedia::codec::EncodeFile(wav_path, output_path, bitrate, enable_preprocessing, enable_dtx, model_path);
}

bool DecodeFile_LYRA(const std::string& encoded_path,
        const std::string& output_path, int sample_rate_hz,
        int bitrate, 
        //bool randomize_num_samples_requested,
        //float packet_loss_rate, 
        //float average_burst_length,
        //const PacketLossPattern& fixed_packet_loss_pattern,
        const std::string& model_path) {
    return  chromemedia::codec::DecodeFile(
            encoded_path, output_path, sample_rate_hz, bitrate,
            false, 0.0, 1.0, chromemedia::codec::PacketLossPattern({}, {}),
            model_path);
}


// int lyra_benchmark_LYRA(int num_cond_vectors, const std::string& model_base_path,
//                    bool benchmark_feature_extraction, bool benchmark_quantizer,
//                    bool benchmark_generative_model) {
//     return chromemedia::codec::lyra_benchmark(num_cond_vectors, model_base_path,
//                     benchmark_feature_extraction, benchmark_quantizer, benchmark_generative_model);
// }

} // codec
} // chromedia
