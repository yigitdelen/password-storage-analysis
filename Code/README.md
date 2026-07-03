# File Organization Project

A C project that takes a large password dataset, cleans it into a single
standardized reference file, and then stores that same dataset using three
different representations — plain text, hashed, and compressed — in order to
compare their disk usage and search performance.

## Project Structure

```
Project/
|
|-- RawData/            Original password files downloaded from SecLists
|-- Processed/           Cleaned, deduplicated dataset (single reference file)
|-- Storage/
|   |-- Raw/              Plain-text copy of the processed dataset
|   |-- Hashed/           MD5 + SHA-256 digests of each password
|   '-- Compressed/       zlib-compressed (.gz) copy of the processed dataset
|
|-- Code/                All C source files
|-- Results/             Generated performance results (results.txt)
'-- Report/               Project report (.docx)
```

## Source Files (Code/)

| File | Responsibility |
|---|---|
| `main.c` | Entry point. Runs preprocessing, storage, sampling, benchmarking, and prints results. |
| `preprocess.c` | Reads raw files, removes empty lines and duplicates, writes the cleaned dataset. |
| `storage.c` | Writes the processed dataset into Raw, Hashed, and Compressed formats. |
| `search.c` | Implements search for each of the three storage methods. |
| `compression.c` | Computes compression ratio and decompression time using zlib. |
| `utils.c` / `utils.h` | Timing, file size, CPU usage, RAM usage, and result-logging helpers. |
| `md5.c` / `md5.h` / `md5_wrapper.c` | Public-domain MD5 implementation, wrapped as `md5_string()`. |
| `sha256.c` / `sha256.h` / `sha256_wrapper.c` | Public-domain SHA-256 implementation, wrapped as `sha256_string()`. |

## Requirements

- GCC (MinGW-w64 via MSYS2 recommended on Windows)
- zlib (`-lz`)
- Windows API headers `windows.h` and `psapi.h` (for CPU/RAM measurement)
- Standard C library, `math.h`

## Build

Run from the `Code/` directory, using the **MSYS2 MINGW64** terminal on Windows:

```bash
gcc -O2 -o project main.c preprocess.c storage.c search.c compression.c utils.c md5.c md5_wrapper.c sha256.c sha256_wrapper.c -lz -lpsapi -lm
```

## Run

```bash
./project.exe
```

The program will:

1. Read and clean the two raw datasets in `RawData/`, writing the result to
   `Processed/passwords_clean.txt`.
2. Build the three storage representations in `Storage/Raw/`,
   `Storage/Hashed/`, and `Storage/Compressed/`.
3. Randomly select 1,000 passwords from the cleaned dataset (fixed seed,
   reproducible across runs).
4. Benchmark search time, CPU usage, and RAM usage for each storage method.
5. Measure the compression ratio and decompression time.
6. Write all results automatically to `Results/results.txt`.

## Notes

- All storage methods are built exclusively from `Processed/`, never directly
  from `RawData/`, as required by the project specification.
- No case normalization is applied to passwords; they are treated as
  case-sensitive strings exactly as found in the source files.
- The Hashed storage method stores both an MD5 digest (required) and a
  SHA-256 digest (optional) per password. Searches against Hashed storage
  use the MD5 digest for comparison.
- The full write-up of methodology, results, and discussion is in
  `Report/File_Organization_Project_Report.docx`.
