#pragma once

#include <list>
#include <string>
#include <memory>
#include <forward_list>

#include "lz77.hpp"

void read_raw_file(const std::string& path, std::shared_ptr<std::string> buffer);

void write_orig_file(const std::string& path, std::shared_ptr<std::string> s);

void write_bin_file(const std::string& path, const std::shared_ptr<std::forward_list<CodeNode>> code);

std::shared_ptr<std::forward_list<CodeNode>> read_bin_file(const std::string& path);
