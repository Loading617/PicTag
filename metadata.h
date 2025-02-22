#ifndef METADATA_H
#define METADATA_H

#include <string>

std::string get_metadata(const std::string &filename);
void set_metadata(const std::string &filename, const std::string &key, const std::string &value);

#endifv
