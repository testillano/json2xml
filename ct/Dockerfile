FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    python3-pytest python3-xmltodict \
    && rm -rf /var/lib/apt/lists/*

CMD ["pytest-3"]
