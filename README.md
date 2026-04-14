# Plagiarism Detection Tool

A command-line tool for detecting plagiarism in source code files using advanced fingerprinting techniques. This tool analyzes pairs of files to identify similarities based on code structure rather than exact text matches.

## Features

- **Multi-language Support**: Handles C++ and Python source files
- **Winnowing Algorithm**: Uses robust fingerprinting to detect code similarities
- **Configurable Parameters**: Adjustable similarity threshold, k-gram size, and windowing parameters
- **JSON Output**: Structured output for easy integration with other tools
- **Efficient Processing**: Optimized for large codebases with pairwise comparison

## How It Works

The tool employs a multi-step process:

1. **Preprocessing**: Strips comments, whitespace, and normalizes text to lowercase
2. **Fingerprinting**: Applies the winnowing algorithm to generate document fingerprints
3. **Similarity Calculation**: Computes Jaccard similarity and containment metrics between file pairs
4. **Reporting**: Outputs matches that exceed the specified similarity threshold

## Installation

### Prerequisites

- C++17 compatible compiler (e.g., g++ 7.0 or later)
- Make build system

### Build Instructions

```bash
# Clone or navigate to the project directory
cd Plagiarism_detection

# Build the executable
make

# The binary 'plagiarism_detector' will be created in the current directory
```

## Usage

```bash
./plagiarism_detector --dir <directory> [options]
```

### Required Arguments

- `--dir <directory>`: Path to directory containing source files to analyze

### Optional Arguments

- `--threshold <0.0-1.0>`: Minimum Jaccard similarity threshold to report (default: 0.1)
- `--kgram <int>`: Size of k-grams for fingerprinting (default: 15)
- `--window <int>`: Winnowing window size guarantee threshold (default: 20)

### Example Usage

```bash
# Basic usage with default settings
./plagiarism_detector --dir ./source_code

# Lower threshold for more sensitive detection
./plagiarism_detector --dir ./source_code --threshold 0.05

# Custom parameters for fine-tuning
./plagiarism_detector --dir ./source_code --threshold 0.1 --kgram 20 --window 25
```

### Output Format

The tool outputs results in JSON format:

```json
{
  "threshold_used": 0.1,
  "kgram": 15,
  "window_thresh": 20,
  "matches": [
    {
      "file1": "path/to/file1.cpp",
      "file2": "path/to/file2.cpp",
      "jaccard_similarity": 0.2345,
      "containment_file1_in_file2": 0.4567,
      "containment_file2_in_file1": 0.3210,
      "common_hashes": 42
    }
  ]
}
```

- `jaccard_similarity`: Jaccard index measuring set similarity of fingerprints
- `containment_file1_in_file2`: Percentage of file1's fingerprints found in file2
- `containment_file2_in_file1`: Percentage of file2's fingerprints found in file1
- `common_hashes`: Number of shared fingerprint hashes

## Project Structure

```
Plagiarism_detection/
├── Makefile                    # Build configuration
├── README.md                   # This file
├── example_data/               # Sample files for testing
│   ├── doc1.cpp
│   ├── doc2.cpp
│   ├── doc3.cpp
│   └── doc4.py
└── src/
    ├── main.cpp                # Main application entry point
    ├── fingerprinting/
    │   ├── winnowing.cpp       # Winnowing algorithm implementation
    │   └── winnowing.h         # Fingerprinting interface
    ├── preprocessing/
    │   ├── preprocessor.cpp    # Text normalization and cleaning
    │   └── preprocessor.h      # Preprocessing interface
    ├── similarity/
    │   ├── similarity.cpp      # Similarity calculation algorithms
    │   └── similarity.h        # Similarity interface
    └── utils/
        ├── file_utils.cpp      # File I/O utilities
        └── file_utils.h        # File utilities interface
```

## Dependencies

- **C++ Standard Library**: C++17 features (filesystem, string_view, etc.)
- **Build System**: GNU Make
- **Compiler**: Any C++17 compliant compiler

No external libraries are required - the tool uses only standard C++ features.

## Supported File Types

Currently supports:
- C++ source files (.cpp, .cc, .cxx, .h, .hpp)
- Python source files (.py)

The preprocessing step handles language-specific comment styles:
- C++: `//` line comments and `/* */` block comments
- Python: `#` line comments

## Algorithm Details

### Winnowing Fingerprinting

The tool uses the winnowing algorithm for document fingerprinting:
- Divides normalized text into k-grams
- Computes rolling hashes for each k-gram
- Selects minimum hashes within sliding windows
- Generates compact fingerprints resistant to minor modifications

### Similarity Metrics

- **Jaccard Similarity**: |A ∩ B| / |A ∪ B| where A and B are fingerprint sets
- **Containment**: Measures how much of one document is contained in another
- **Common Hashes**: Raw count of shared fingerprints

## Limitations

- Designed for source code plagiarism detection
- May not perform optimally on very short files
- Parameter tuning may be needed for different codebases
- Pairwise comparison scales quadratically with file count

## Contributing

To contribute to this project:
1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly with the example data
5. Submit a pull request

## License

This project is open source. Please check the license file for details.
