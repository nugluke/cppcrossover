# Variables
IMAGE_NAME = crossover_filter
OUTPUT_PREFIX = output

# Build the Docker image from the Dockerfile
build:
	docker build -t $(IMAGE_NAME) .

# Run the Docker container using the built image
run:
	@if [ -z "$(INPUT_FILE)" ]; then \
		echo "Error: INPUT_FILE is not provided. Usage: make run INPUT_FILE=<input_file> CUTOFF_FREQ=<cutoff_freq>"; \
		exit 1; \
	fi
	@if [ -z "$(CUTOFF_FREQ)" ]; then \
		echo "Error: CUTOFF_FREQ is not provided. Usage: make run INPUT_FILE=<input_file> CUTOFF_FREQ=<cutoff_freq>"; \
		exit 1; \
	fi
	docker run --rm -v "$(shell pwd)":/data $(IMAGE_NAME) /data/$(INPUT_FILE) $(CUTOFF_FREQ) /data/$(OUTPUT_PREFIX)

# Clean up (remove Docker image)
clean:
	docker rmi $(IMAGE_NAME)

# Help message
help:
	@echo "Usage:"
	@echo "  make build                          Build the Docker image from the Dockerfile"
	@echo "  make run INPUT_FILE=<input_file> CUTOFF_FREQ=<cutoff_freq>   Run the Docker container"
	@echo "  make clean                          Remove the Docker image"
	@echo "  make help                           Show this help message"