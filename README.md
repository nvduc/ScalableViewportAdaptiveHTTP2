# ScalableViewportAdaptiveHTTP2
## Overview
  This repository contains the source codes of HTTP2-based Viewport Adaptive Streaming for Scalable 360 Video system presented in the following two papers:
  
  [1] D. V. Nguyen, H. Van Trung, H. L. Dieu Huong, T. T. Huong, N. P. Ngoc and T. C. Thang, "Scalable 360 Video Streaming using HTTP/2," 2019 IEEE 21st International Workshop on Multimedia Signal Processing (MMSP), 2019, pp. 1-6, doi: 10.1109/MMSP.2019.8901805.
  
  [2] H. L. Dieu Huong, D. V. Nguyen, T. T. Huong, P. Ngoc Nam and T. C. Thang, "Smooth Viewport Bitrate Adaptation for 360 Video Streaming," 2019 6th NAFOSTED Conference on Information and Computer Science (NICS), 2019, pp. 512-517, doi: 10.1109/NICS48868.2019.9023807. 

## Installation
### General
- To build and run this code, you will need to prepare two PCs running Ubuntu-18.04 LTS, one for client and another for server. We recomend using Ubuntu running on VirtualBox.
- We originally use DummyNet to emulate client-server network. However, since DummyNet is not supported in Ubuntu-18.04, we use a new tool called `wondershaper` on client machine to limit network throughput, and using Ubuntu's built-in `netem` utility on the server machine to emulate network delay. 
### Client
Follow instructions in `client/INSTALL_NOTE` to build the client. 
### Server
Follow instructions in `server/INSTALL_NOTE` to build the server. 
## Running
- To make sure the code run correctly, you MUST run the server first, then start the client.
- The client will run the algorithm proposed in paper [1] by default (ALG=5). To run the algoirthm proposed in paper [2], set the value of `ALG` in file `client/metadata/metadata.cfg` to 11.  
## Understanding log files
* The results of each run are recorded in 4 log files in `client/`:
  * `log_iter_ALG_5.txt`: Download information per download iteration
  * `log_stream_ALG_5.txt`: Download information per HTTP2's stream
  * `log_player_ALG_5.txt`: Player status per video segment
  * `log_player_ALG_5_frame`.txt: Player status per video frame

