#ifndef ADNROID_LYRA_LIB_H_
#define ADNROID_LYRA_LIB_H_

#include <cstdint>
#include <vector>
#include "ghc/filesystem.hpp"
namespace chromemedia {
namespace codec {

// Encodes a vector of wav_data into encoded_features.
// Uses the quant files located under |model_path|.

bool Initialize_Encoder_LYRA(int sample_rate_hz, int num_channels, int bitrate,
                                     bool enable_dtx,
                                     const std::string& model_path);
void Release_Encoder_LYRA();

bool Initialize_Decoder_LYRA(int sample_rate_hz, int num_channels,
                                     const std::string& model_path);

void Release_Decoder_LYRA();

void Set_Bitrate_LYRA(int bitrate);

bool EncodeWav_LYRA(const std::vector<int16_t>& wav_data,
               int sample_rate_hz, 
               bool enable_preprocessing,
               std::vector<uint8_t>* encoded_features);

bool DecodeFeatures_LYRA(const std::vector<uint8_t>& packet_stream, int bit_rate,
                    std::vector<int16_t>* decoded_audio);

// Encodes a wav file into an encoded feature file. Encodes num_samples from the
// file at |wav_path| and writes the encoded features out to |output_path|.
// Uses the quant files located under |model_path|.
bool EncodeFile_LYRA(const std::string& wav_path,
    const std::string& output_path, int bitrate,
    bool enable_preprocessing, bool enable_dtx,
    const std::string& model_path);

bool DecodeFile_LYRA(const std::string& encoded_path,
    const std::string& output_path, int sample_rate_hz,
    int bitrate, 
    //bool randomize_num_samples_requested,
    //float packet_loss_rate, 
    //float average_burst_length,
    //const PacketLossPattern& fixed_packet_loss_pattern,
    const std::string& model_path);

// int lyra_benchmark_LYRA(int num_cond_vectors, const std::string& model_base_path,
//                    bool benchmark_feature_extraction, bool benchmark_quantizer,
//                    bool benchmark_generative_model);

} // codec
} // chromedia

#endif


