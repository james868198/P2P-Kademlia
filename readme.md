# P2P-Kademlia

## Introduction

We tried to run our peer in multiple Docker containers to simulate the network connection between peers. However, possibly due to the UDP support issue of Docker,  our network connection is not stable. So We run the simulation on our local with multiple processes running on various ports.

## setup tutorial

### Install dependences for openssl

### On Ubuntu

    sudo apt update
    sudo apt install build-essential checkinstall zlib1g-dev -y

### On CentOS

    yum group install 'Development Tools'
    yum install perl-core zlib-devel -y

Download openssl-1.1.1g from  https://www.openssl.org/source/openssl-1.1.1g.tar.gz

Run following codes to install openssl

    tar -xf openssl-1.1.1g.tar.gz

    cd openssl-1.1.1g

    ./config

    make

    make test

    make install

---

## P2P-Kademlia

git clone https://github.com/ric31303/P2P-Kademlia.git

### makefile

    cd P2P-Kademlia

    make clean

    make

### Run executable file

    .output/main 

---

## P2P-Kademlia Build connection

To join the network, you need to run a root P2P-Kademlia peer first and create another peer to join the root peer. So when you run your root(first) peer, Please record its local IP and port. When you want to create another test peer, create another config file for it. You can just copy the default.config in P2P-Kademlia folder. Replace the bootstrap and port by the root node's IP and port. The following shows the format of the config file

    bootstrap #request_to_joing_ip
    port #request_to_joing_port
    local_port #default port
    k 20
    alpha 3
    shared_folder Share/   
    download_folder Download/

After setting up the new config for the second node. You can run

    .output/main -c "new config path"

to create another process and it will send th request to the root to ask it to join the network.

---

## P2P-Kademlia CLI

    ping address            Verify the liveness of the address only if address is in the buckets

    join address            Ask address to join the node the network

    getBuckets              get the buckets the node maintains

    getBucket number        get specific bucket by number(number is between 1 to 160)

    get filename            get file from the network (e.g. get file1.txt)
    
    store filename address  store file to the node
    
    exit                    terminate the process

---

## P2P-Kademlia test

To test the network, you can create run several processes with different ports and with different Shared folder on local. You can use getBuckets to varify whether the distribution of node in bucket list is correct or not. You can also run "get file" to test the function of the network
