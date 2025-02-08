# archivation_app

This is an application for encoding and decoding files of any format or directory with files. It used  [LZ77 algorithm](https://en.wikipedia.org/wiki/LZ77_and_LZ78) of lossless compressing and decompressing.

## Interface
```
Syntax:     ./archivation_app.exe <options>  
Options:    -c or --compress                        compression mode
            -d or --decompress                      decompression mode
            -f <filepath> or --file <filepath>      input file or directory path to compress or decompress
            -o <filepath> or --output <filepath>    output file or directory path to save a result
            -l <size> or --lasize <size>            lookahead size (optional, default 255)
            -b <size> or --bsize <size>             search-buffer size (optional, default 255)
            -h or --help                            help
```

## What to do next
- Parallel encoding / decoding
- GIU in Qt
- Add other algorithms to gain better efficiency of compression of files of different types (algorithm lz77 is not universal. Some files compressed with lz77 may take up more memory than decompressed ones)