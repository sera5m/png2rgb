#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cstdint>
#include <sstream>

uint16_t rgb888_to_custom(uint8_t r, uint8_t g, uint8_t b, int r_bits, int g_bits, int b_bits) {
    int r_val = r >> (8 - r_bits);
    int g_val = g >> (8 - g_bits);
    int b_val = b >> (8 - b_bits);

    return (r_val << (g_bits + b_bits)) | (g_val << b_bits) | b_val;
}

int main() {
    int bit_depth;
    std::string schema_input;
    std::string filename;

    // Step 1: Ask for bit depth
    std::cout << "Desired bit depth of image: ";
    std::cin >> bit_depth;

    // Step 2: Ask for encoding schema
    std::cout << "bit encoding.          bits for r,g,b \n";
    std::cout << "data Encoding schema? (example: 5,6,5): ";
    std::cin >> schema_input;

    int r_bits, g_bits, b_bits;
    char sep;
    std::stringstream schema_stream(schema_input);
    schema_stream >> r_bits >> sep >> g_bits >> sep >> b_bits;

    if ((r_bits + g_bits + b_bits) != bit_depth) {
        std::cerr << "Error: Schema doesn't match the bit depth.\n tip: bits in r,g,b must add up to be your desired bit depth bits in (r+g+b) must equal bit depth!\n";
        return 1;
    }

    // Step 3: Ask for filename
    std::cout << "drop in your [.png] file and hit enter ";
    std::cin >> filename;

    int width, height, channels;
    stbi_uc* img = stbi_load(filename.c_str(), &width, &height, &channels, 3);
    if (!img) {
        std::cerr << "Failed to load image: " << filename << "\n";
        return 1;
    }

    std::string output_file = filename + ".converted.cpp";
    std::ofstream out(output_file);
    if (!out) {
        std::cerr << "Failed to open output file: " << output_file << "\n";
        stbi_image_free(img);
        return 1;
    }

    out << "#include <cstdint>\n\n";
    out << "// Image size: " << width << "x" << height << "\n";
    out << "const uint16_t image_data[" << width * height << "] = {\n";

    for (int y = 0; y < height; ++y) {
        out << "    ";
        for (int x = 0; x < width; ++x) {
            int idx = (y * width + x) * 3;
            uint8_t r = img[idx];
            uint8_t g = img[idx + 1];
            uint8_t b = img[idx + 2];

            uint16_t value = rgb888_to_custom(r, g, b, r_bits, g_bits, b_bits);
            out << "0x" << std::hex << std::setw(4) << std::setfill('0') << value << ", ";
        }
        out << "\n";
    }

    out << "};\n";
    stbi_image_free(img);

    std::cout << "Image converted and saved to: " << output_file << "\n";
    return 0;
}
