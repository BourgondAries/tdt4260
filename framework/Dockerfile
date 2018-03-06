FROM ubuntu
RUN apt update && apt install -y build-essential g++ scons swig
RUN apt install -y python-dev m4 zlib1g zlib1g-dev
CMD ["bash", "-c", "cd /home/default && ./try.sh"]
