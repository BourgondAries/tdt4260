FROM ubuntu
RUN apt update && apt install -y build-essential g++ scons swig
RUN apt install -y python-dev m4 zlib1g zlib1g-dev
RUN useradd -ms /bin/bash kefin
USER kefin
WORKDIR /home/kefin/
CMD ["bash"]
