FROM gcc:13

ENV DEBIAN_FRONTEND=noninteractive

# cmake >= 3.14 (FetchContent_MakeAvailable)
RUN apt-get update && apt-get install -y --no-install-recommends cmake && rm -rf /var/lib/apt/lists/*

CMD ["cmake"]
