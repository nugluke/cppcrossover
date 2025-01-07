#include <iostream>
#include <vector>
#include <cmath>
#include <sndfile.h>
#include <mpg123.h>

// Constants
const double PI = 3.14159265358979323846;

// First-order Butterworth low-pass filter
void lowPassFilter(const std::vector<double>& input, std::vector<double>& output, double cutoffFreq, double sampleRate) {
    double dt = 1.0 / sampleRate;
    double RC = 1.0 / (2 * PI * cutoffFreq);
    double alpha = dt / (RC + dt);

    output[0] = input[0];  // Initialize the first sample

    for (size_t i = 1; i < input.size(); ++i) {
        output[i] = output[i - 1] + alpha * (input[i] - output[i - 1]);
    }
}

// First-order Butterworth high-pass filter
void highPassFilter(const std::vector<double>& input, std::vector<double>& output, double cutoffFreq, double sampleRate) {
    double dt = 1.0 / sampleRate;
    double RC = 1.0 / (2 * PI * cutoffFreq);
    double alpha = RC / (RC + dt);

    output[0] = input[0];  // Initialize the first sample

    for (size_t i = 1; i < input.size(); ++i) {
        output[i] = alpha * (output[i - 1] + input[i] - input[i - 1]);
    }
}

// Read audio file using libsndfile (for WAV) or libmpg123 (for MP3)
bool readAudioFile(const std::string& filename, std::vector<double>& audioData, SF_INFO& sfinfo) {
    if (filename.substr(filename.find_last_of(".") + 1) == "wav") {
        // Read WAV file using libsndfile
        SNDFILE* file = sf_open(filename.c_str(), SFM_READ, &sfinfo);
        if (!file) {
            std::cerr << "Error opening WAV file: " << filename << std::endl;
            return false;
        }

        audioData.resize(sfinfo.frames * sfinfo.channels);
        sf_read_double(file, audioData.data(), audioData.size());
        sf_close(file);

        return true;
    } else if (filename.substr(filename.find_last_of(".") + 1) == "mp3") {
        // Read MP3 file using libmpg123
        mpg123_handle* mh = mpg123_new(nullptr, nullptr);
        if (mpg123_open(mh, filename.c_str()) != MPG123_OK) {
            std::cerr << "Error opening MP3 file: " << filename << std::endl;
            mpg123_delete(mh);
            return false;
        }

        long rate;
        int channels, encoding;
        mpg123_getformat(mh, &rate, &channels, &encoding);
        sfinfo.samplerate = rate;
        sfinfo.channels = channels;
        sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;

        size_t bufferSize = mpg123_outblock(mh);
        std::vector<unsigned char> buffer(bufferSize);
        std::vector<double> interleavedData;

        size_t done;
        int err;
        while ((err = mpg123_read(mh, buffer.data(), bufferSize, &done)) == MPG123_OK) {
            for (size_t i = 0; i < done; i += sizeof(short)) {
                short sample = *reinterpret_cast<short*>(&buffer[i]);
                interleavedData.push_back(sample / 32768.0);  // Normalize to [-1, 1]
            }
        }

        if (err != MPG123_DONE) {
            std::cerr << "Error reading MP3 file: " << mpg123_strerror(mh) << std::endl;
            mpg123_delete(mh);
            return false;
        }

        audioData = interleavedData;
        mpg123_delete(mh);
        return true;
    } else {
        std::cerr << "Unsupported file format: " << filename << std::endl;
        return false;
    }
}

// Write audio file using libsndfile
bool writeAudioFile(const std::string& filename, const std::vector<double>& audioData, const SF_INFO& sfinfo) {
    SF_INFO outInfo = sfinfo;
    outInfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;

    SNDFILE* file = sf_open(filename.c_str(), SFM_WRITE, &outInfo);
    if (!file) {
        std::cerr << "Error writing WAV file: " << filename << std::endl;
        return false;
    }

    sf_write_double(file, audioData.data(), audioData.size());
    sf_close(file);
    return true;
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <input_file> <cutoff_freq> <output_prefix>" << std::endl;
        return 1;
    }

    // Parse arguments
    std::string inputFile = argv[1];
    double cutoffFreq = std::stod(argv[2]);
    std::string outputPrefix = argv[3];

    // Read the input audio file
    std::vector<double> audioData;
    SF_INFO sfinfo;
    if (!readAudioFile(inputFile, audioData, sfinfo)) {
        return 1;
    }

    // Prepare output vectors
    std::vector<double> lowPassOutput(audioData.size());
    std::vector<double> highPassOutput(audioData.size());

    // Apply the filters
    lowPassFilter(audioData, lowPassOutput, cutoffFreq, sfinfo.samplerate);
    highPassFilter(audioData, highPassOutput, cutoffFreq, sfinfo.samplerate);

    // Write the filtered data to output files
    std::string outputLowFile = outputPrefix + "_low.wav";
    std::string outputHighFile = outputPrefix + "_high.wav";

    if (!writeAudioFile(outputLowFile, lowPassOutput, sfinfo)) {
        return 1;
    }
    if (!writeAudioFile(outputHighFile, highPassOutput, sfinfo)) {
        return 1;
    }

    std::cout << "Filtering complete! Check " << outputLowFile << " and " << outputHighFile << " for the results." << std::endl;

    return 0;
}