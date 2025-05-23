#ifndef ADNROID_LYRA_LIB_H_
#define ADNROID_LYRA_LIB_H_

#include <cstdint>
#include <vector>

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
                                     const std::string& model_path);
/**
 * @brief Release resources used by the Lyra encoder
 */
void lyra_encoder_release();

/**
 * @brief Initialize the Lyra audio decoder
 * 
 * @param sample_rate_hz Sample rate in Hz (e.g., 16000)
 * @param num_channels Number of audio channels (1 for mono)
 * @param model_path Path to the directory containing model files
 * @return true if initialization succeeded, false otherwise
 */
bool lyra_decoder_initialize(int sample_rate_hz, int num_channels,
                                     const std::string& model_path);

/**
 * @brief Release resources used by the Lyra decoder
 */
void lyra_decoder_release();

/**
 * @brief Change the target bitrate of the encoder
 * 
 * @param bitrate New target bitrate in bits per second
 */
void lyra_set_bitrate(int bitrate);

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
               std::vector<uint8_t>* encoded_features);

/**
 * @brief Decode Lyra compressed data back to audio samples
 * 
 * @param packet_stream Input compressed data
 * @param bit_rate Bitrate of the encoded stream
 * @param[out] decoded_audio Output buffer for decoded PCM audio samples
 * @return true if decoding succeeded, false otherwise
 */
bool lyra_decode_features(const std::vector<uint8_t>& packet_stream, int bit_rate,
                    std::vector<int16_t>* decoded_audio);

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
    const std::string& model_path);

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
    const std::string& model_path);

} // codec
} // chromedia

#endif


