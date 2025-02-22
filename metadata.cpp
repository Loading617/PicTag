#include "metadata.h"
#include <exiv2/exiv2.hpp>
#include <iostream>

std::string get_metadata(const std::string &filename) {
    try {
        Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(filename);
        image->readMetadata();
        Exiv2::ExifData &exifData = image->exifData();

        std::string result;
        for (auto &entry : exifData) {
            result += entry.key() + " = " + entry.toString() + "\n";
        }
        return result;
    } catch (Exiv2::Error &e) {
        return "Error reading metadata: " + std::string(e.what());
    }
}

void set_metadata(const std::string &filename, const std::string &key, const std::string &value) {
    try {
        Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(filename);
        image->readMetadata();
        image->exifData()[key] = value;
        image->writeMetadata();
    } catch (Exiv2::Error &e) {
        std::cerr << "Error writing metadata: " << e.what() << std::endl;
    }
}
