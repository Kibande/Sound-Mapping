#include<iostream>
#include <fstream>
#include<cmath>

using namespace std;

//Riff Chunk
const string chunck_id = "RIFF";
const string  chunk_size = "----";
const string format = "WAVE";

//FMT sub-chunk
const string subchunk1_id = "fmt ";
const int subchunk1_size = 16; // 4 bytes
const int audio_format = 1; // 2 bytes
const int num_channels = 2; // 2 bytes
const int sample_rate = 44100;
const int byte_rate = sample_rate * num_channels * (subchunk1_size / 8 );
const int block_align = num_channels * (subchunk1_size / 8);
const int bits_per_sample = 16;

// Data sub chunk
const string subchunk2_id   =  "data";
const string subchunk2_size =  "----";

const int duration = 2;
const int max_amplitude = 32760;
const double frequency = 250;


// helps us enforce that ant integer takes up only a particular number of 
// bytes 

void write_as_bytes(ofstream& file, int value, int byte_size) {
	file.write(reinterpret_cast<const char*>(&value),byte_size);
}

int main() {
	 
	ofstream wav;
	wav.open("test.wav", ios::binary);

	if (wav.is_open()) {
		wav << chunck_id;
		wav << chunk_size;
		wav << format;

		wav << subchunk1_id;
		write_as_bytes(wav, subchunk1_size, 4);


		write_as_bytes(wav, audio_format, 2);
		write_as_bytes(wav, num_channels, 2);
		write_as_bytes(wav, sample_rate, 4);
		write_as_bytes(wav, byte_rate, 4);
		write_as_bytes(wav, block_align, 2);
		write_as_bytes(wav, bits_per_sample, 2);

		wav << subchunk2_id;
		wav << subchunk2_size;

		int start_audio = wav.tellp();

		for (size_t i = 0; i < sample_rate * duration; i++)
		{
			//respect the max amplitude
			// operate as a wave
			// calculate amplitude for our sin function

			double amplitude = (double)i / sample_rate * max_amplitude;

			double value = sin(( 2 * 3.14 *  i * frequency) / sample_rate);

			//supply channel data
			double channel_1 = amplitude * value / 2; // left ear
			double channel_2 = (max_amplitude - amplitude) * value;  // right ear
			write_as_bytes(wav, channel_1, 2);
			write_as_bytes(wav, channel_2, 2);

		}

		//update the size of the data
		int end_audio = wav.tellp();

		wav.seekp(start_audio - 4);

		write_as_bytes(wav, end_audio - start_audio, 4);

		// update our header file
		wav.seekp(4, ios::beg);
		write_as_bytes(wav, end_audio - 8, 4);
	}

	wav.close();
	return 0;
}