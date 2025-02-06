#pragma once

#include <list>
#include <string>
#include <memory>
#include <forward_list>

#include "lz77.hpp"

namespace fs = std::filesystem;

void read_original_file(const std::string& path, std::unique_ptr<std::string>& buffer);
void write_decompressed_file(const std::string& path, const std::unique_ptr<std::string>& s);
void write_compressed_file(const std::string& path, const std::unique_ptr<std::forward_list<CodeNode>>& code);
void read_compressed_file(const std::string& path, std::unique_ptr<std::forward_list<CodeNode>>& code);
std::string get_path_to_write(const std::string& original_path, const std::string& new_path, const fs::directory_entry &dir_entry);
void process_all_files(const std::string& input_filepath, const std::string& output_filepath, const MODES mode, const unsigned char la_size, const unsigned char sb_size);
