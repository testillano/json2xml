FROM ubuntu

ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update && apt-get install -y \
    python3-pip
RUN pip3 install pytest xmltodict

CMD ["pytest"]
