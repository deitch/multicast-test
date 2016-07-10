# Multicast Over Loopback

This repository is a testing sample describing how to use multicast over loopback.

At times, you want to communicate between two processes on the same host using multicast. It is a convenient way to send a large volume of small chunks of data between multiple processes. This is common in financial applications pricing.

For example, if process A wants to stream data to processes B, C and D, there are multiple ways to send the data. One of those is simply multicast. The advantages of multicast are:

* low impact (relatively), although direct memory copy would be faster
* decoupled: since each process is sending and receiving standard network packets, they are decoupled from each other. They easily can be reconfigured to work over the network without changing the code itself.
* fan-out: it is easy to have one sender and multiple receivers

When you have all of the listeners on the same host as the sender, it makes sense to use the loopback interface, alternately called `lo0` or `lo`. The primary benefit is keeping network packets off of the external network, either for performance - why send a very large number of unused packets on the network - or for security/privacy.

However, depending on how you use multicast, your attempt to send over localhost might fail, leaking the packets out to the network.

This repository shows the two primary ways to send multicast to processes on the same host, which works and which doesn't.

Of course, if you are sending to other hosts on the network, you needn't worry about any of this.

And to be honest, if your application design requires a large volume stream of multicast, yet your network cannot handle it, you need either a new application design or a new network.

## The Basics
Multicast packets are, by definiton, UDP. UDP, by definition, is connectionless. That means you do not establish a channel between sender and receiver, and the operating system makes no guarantees about receipt.

Multicast *must* be UDP, since the sender has no idea if there are zero, one or many receivers. 

TCP provides one way to send a packet:

1. Open a socket
2. Set up a connection to a listener
3. Send data via the connection

This is represented by the code below:



Since UDP is connectionless and therefore has no concept of setting up a connection to a listener, *in principle* it provides only one way to send a packet:

1. Open a socket
2. Send data to an address

However, the operating system provides *two* interfaces to send UDP packets. Both have **almost** but not quite *exactly* the same result:

* Open a socket and send data to an address
* Open a socket, create a "connection" representing sending data to the far end and send data via the "connection"

Since no "connection" really exists, the second option is just a convenience method to save the developer from entering the target address every time. It is the equivalent of saying to the operating system:

> give me a handle for remote address a.b.c.d port xxxx, and every time I send a UDP packet to that handle, send it to that address/port combination

It turns out, that convenience of a handle makes all of the difference.

Here is code representing each of the two cases:

First, sending to an address:

    fd = socket(AF_INET, SOCK_DGRAM, 0)
    sendto(fd, buf, recv_len, 0, remote_address_info->ai_addr, remote_address_info->ai_addrlen)

Second, sending with the convenience method

    fd = socket(AF_INET, SOCK_DGRAM, 0)
    connect(fd,(struct sockaddr *) &remote_address_info,sizeof(remote_address_info)
    send(fd,buf,strlen(buf))

In almost all cases, both of those work exactly the same. But if you are trying to send multicast over loopback, it does not. The `connect()` call over loopback behaves in odd ways, routing the packets over the default interface.

Trying to send multicast over loopback usually looks like the following:

1. `route add 224.2.0.0/24 via 127.0.0.1 dev lo0`
2. Send the multicast packets to, for example, `224.2.0.6:6666`

If you do, **only the direct, sendto() will work**. The convenience method **will fail**, ignoring the `route` and sending the packets via the default interface.

The code in this repository shows examples of packets sent in each of the two methods and how they behave. It does the following:

1. Generate a random port
2. Create an unused multicast `/32` route and add it to the local routing table. It is set in `run.sh` to use `224.16.17.8`
3. Start a UDP listener on the multicast address and on the previously unused port on the loopback interface
4. Send two packets, one each using the direct and the convenience method, each with appropriate content. For the direct, the contents are "sendto()"; for the convenience method, the contents are "send()"
5. When the listener receives a packet, it records the interface on which it was received, and sends the interface and contents to a single line on STDOUT
6. Shut down the listener
7. Remove the route

## WARNING

Under certain circumstances, depending on how your interfaces and kernel are configured, the "server" might not receive any packets over `lo`, even though it is configured to listen there.

In that case, you don't even need the server, just run `tcpdump -i lo port 6666` to see the packets on loopback and `tcpdump -i eth0 port 6666` to see the packets on eth0.

You should expect to see `send()` packets on `eth0`, since `connect()/send()` does not respect the route, while `sendto()` on `lo`, since it *does* respect the route.















