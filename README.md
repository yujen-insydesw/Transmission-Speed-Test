## Objective
It is a simple program for transmission speed test. \
The code also reveal basic socket programming.

## Speed test formula

$$\text{Average Latency} = \frac{\text{Total Round-Trip Time}}{\text{Number of Measurements}}$$

$$\text{Upload Speed} = \frac{\text{Total Data Uploaded}}{\text{Time Taken}}$$

$$\text{Download Speed} = \frac{\text{Total Data Downloaded}}{\text{Time Taken}}$$

## How to build
cmake
```console
$ cd build
$ rm -rf * && cmake .. && make
```

## How to run
server side
```console
$ ./server_side 
Server side listening on port 8080

Received 1048577 bytes

Send 1048576 bytes
```
client side
```console
$ ./client_side 

Average latency (micro second): 37.00

Send 1048576 bytes

Received 1048577 bytes

Average latency (micro second): 37.00
Upload Speed (Mbps): 11848.32
Download Speed (Mbps): 199.66
```

## Compare with benchmark
iperf
```console
$ sudo apt-get install iperf3

$ iperf3 -s

$ iperf3 -c <server_IP>

# To measure bandwidth in both directions simultaneously, use the -d option:
$ iperf3 -c <server_IP> -d
```
speedtest-cli
```console
$ wget -O speedtest-cli https://raw.github.com/sivel/speedtest-cli/master/speedtest.py

$ chmod +x speedtest-cli

$ ./speedtest-cli
```
