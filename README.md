# Crossover Filter Application

This is a Dockerized C++ application that applies a crossover filter to an audio file. It splits the input audio file into two output files:
1. A **low-pass filtered** audio file (frequencies below the cutoff frequency).
2. A **high-pass filtered** audio file (frequencies above the cutoff frequency).

The application supports **WAV** and **MP3** input files and outputs filtered audio in **WAV** format.

---

## Prerequisites

Before using this application, ensure you have the following installed:

1. **Docker**: Install Docker from [here](https://docs.docker.com/get-docker/).
2. **Make**: Install Make (usually pre-installed on Linux/macOS). For Windows, use WSL or a similar tool.

---

## Getting Started

### 1. Clone the Repository
Clone this repository to your local machine:

```bash
git clone <repository-url>
cd <repository-directory>
```

### 2. Building the Docker Image
Build the Docker image using the provided `Dockerfile`:

```bash
make build
```
This will create a Docker image named `crossover_filter`

## Usage
### Running the Application
To run the application, use the make run command with the following arguments:

INPUT_FILE: Path to the input audio file (WAV or MP3).

CUTOFF_FREQ: Cutoff frequency in Hz (e.g., 1000 for 1 kHz).

Example:

```bash 
make run INPUT_FILE=input.mp3 CUTOFF_FREQ=1000 
```

### Output Files
The application will generate two output files in the current directory:

1. output_low.wav: Low-pass filtered audio.

2. output_high.wav: High-pass filtered audio.

