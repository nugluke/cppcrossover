# Use an official Ubuntu base image
FROM ubuntu:22.04

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    libsndfile1-dev \
    libmpg123-dev \
    pkg-config \
    && rm -rf /var/lib/apt/lists/*

# Set the working directory
WORKDIR /app

# Copy the source code into the container
COPY crossover.cpp .

# Compile the program
RUN g++ -o crossover crossover.cpp $(pkg-config --cflags --libs sndfile) $(pkg-config --cflags --libs libmpg123)

# Set the entry point
ENTRYPOINT ["./crossover"]