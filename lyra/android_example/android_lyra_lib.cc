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

namespace chromemedia {
namespace codec {

// Encodes a vector of wav_data into encoded_features.
// Uses the quant files located under |model_path|.
bool EncodeWav_LYRA(const std::vector<int16_t>& wav_data, int num_channels,
               int sample_rate_hz, int bitrate, bool enable_preprocessing,
               bool enable_dtx, const std::string& model_path,
               std::vector<uint8_t>* encoded_features) {

    return chromemedia::codec::EncodeWav(
             wav_data, num_channels, sample_rate_hz, bitrate,
             enable_preprocessing, enable_dtx, model_path, encoded_features);
}

bool DecodeFeatures_LYRA(const std::vector<uint8_t>& packet_stream, int bitrate,
                    std::vector<int16_t>* decoded_audio, 
                    const std::string& model_path){
    std::unique_ptr<chromemedia::codec::LyraDecoder> decoder =
         chromemedia::codec::LyraDecoder::Create(
             16000, chromemedia::codec::kNumChannels, model_path);

    absl::BitGen gen;

    return chromemedia::codec::DecodeFeatures(
            packet_stream, chromemedia::codec::BitrateToPacketSize(bitrate),
            /*randomize_num_samples_requested=*/false, gen, decoder.get(),
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
