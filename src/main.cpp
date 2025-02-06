#include <fstream>
#include <filesystem>
#include <string>
#include <iostream>
#include <forward_list>
#include <memory>
#include <getopt.h>

#include "lz77.hpp"
#include "readandwrite.hpp"
#include "inputParametersException.hpp"

#define MIN_LA_SIZE 2       /* min lookahead size */
#define MAX_LA_SIZE 255   /* max lookahead size */
#define MIN_SB_SIZE 2       /* min search buffer size */
#define MAX_SB_SIZE 255   /* max search buffer size */

#define DEFAULT_LA_SIZE 255
#define DEFAULT_SB_SIZE 255

int main(int argc, char *argv[])
{
    const char *short_options = "hcdf:o:l::b::";

    const struct option long_options[] = 
    {
        {"help", no_argument, NULL, 'h'},
        {"compress", no_argument, NULL, 'c'},
        {"decompress", no_argument, NULL, 'd'},
        {"file", required_argument, NULL, 'f'},
        {"output", required_argument, NULL, 'o'},
        {"lasize", optional_argument, NULL, 'l'},
        {"bsize", optional_argument, NULL, 'b'},
        {NULL, 0, NULL, 0}
    };

    int opt;
    int option_index;
    std::string input_filepath, output_filepath;
    bool help_flag = false;
    MODES mode = MODES::NOT_CHOSEN;
    unsigned short la_size = DEFAULT_LA_SIZE;
    unsigned short sb_size = DEFAULT_SB_SIZE;

    while ((opt = getopt_long(argc, argv, short_options, long_options, &option_index)) != -1)
    {
        switch (opt)
        {
        case 'h':
        {
            std::cout << "Syntax: ./archivation_app.exe <options>\n" << 
            "Options: -c or --compress                      compression mode\n" <<
            "         -d or --decompress                    decompression mode\n" <<
            "         -f <filepath> or --file <filepath>    input file or directory path to compress or decompress\n" <<
            "         -o <filepath> or --output <filepath>  output file or directory path to save a result\n" <<
            "         -l <size> or --lasize <size>          lookahead size (optional, default 4095)\n" <<
            "         -b <size> or --bsize <size>           search-buffer size (optional, default 4095)\n" <<
            "         -h or --help                          help\n";
            help_flag = true;
            break;
        };
        case 'c':
        {
            mode = MODES::ENCODE;
            break;
        };
        case 'd':
        {
            mode = MODES::DECODE;
            break;
        };
        case 'f':
        {
            input_filepath = optarg;
            break;
        };
        case 'o':
        {
            output_filepath = optarg;
            break;
        };
        case 'l':
        {
            // set look ahead window size
            la_size = static_cast<unsigned short>(std::atoi(optarg));
            break;
        };
        case 'b':
        {
            // set search buffer size
            sb_size = static_cast<unsigned short>(std::atoi(optarg));
            break;
        };
        case '?':
        default:
        {
            throw InputParametersException("Found unknown option");
            break;
        };
        }
    }

    if (!help_flag)
    {
        try
        {
            if (mode == MODES::NOT_CHOSEN)
            {
                throw InputParametersException("Mode not chosen (to compress or to decompress)");
            }

            if (!input_filepath.size())
            {
                throw InputParametersException("Input file path not provided");
            }

            if (!output_filepath.size())
            {
                throw InputParametersException("Output file path not provided");
            }

            if (la_size > MAX_LA_SIZE || la_size < MIN_LA_SIZE)
            {
                throw InputParametersException("Wrong lookahead size provided");
            }

            if (sb_size > MAX_SB_SIZE || sb_size < MIN_SB_SIZE)
            {
                throw InputParametersException("Wrong search buffer size provided");
            }

            process_all_files(input_filepath, output_filepath, mode, la_size, sb_size);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
    }

    return 0;
}

