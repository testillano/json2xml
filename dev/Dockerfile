FROM gcc:9.2

ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update && apt-get install -y cmake && rm -rf /var/lib/apt/lists/*

CMD ["cmake"]
