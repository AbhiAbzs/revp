# revp
### Build
Run `make` and then use the resulting executable `revp.exe`.
### Instructions
Run `server` with `./revp.exe -s -l 5000 -p 6000`, where 5000 is where your `client` will connect and 6000 is where `proxy connections` will be serviced.

Run `client` with `./revp.exe -c -a 192.168.1.1 -z 5000`, where 192.168.1.1 is your `server` address and 5000 is your chosen `server` port that listens for a `client`.

Open up a browser in your `server` instance and configure your HTTP proxy to port 6000, now you will be effectively browsing the Internet with `client`'s connection.
### Explanation
There are 3 components to revp: `server`, `client`, and `proxy connections`.

`Server` listens for `client` and starts listening for `proxy connections` once `client` connects.

All `proxy connection` requests are routed to the `client` through the `server`.

`Proxy connections` are expected to make a first request of HTTP CONNECT and once connected they can exchange any data of any protocol with the destination address:port requested in the HTTP CONNECT. `Server` transparently routes all traffic from and to `client` and `client` routes all traffic from and to the remote destination.

What makes this a "reverse" proxy is the fact that the `client` initiates connection to the `server`, so there's no need to open ports or write firewall rules for the `client` whose network access you want to reach into -- you can basically run this on any network node whose firewall allows outbound traffic to your `server` port (essentially all firewall configurations found in the wild allow all outbound traffic).
### Options
```Options:
  -h [ --help ]              Help screen
  -s [ --server ]            Starts revp in server mode
  -c [ --client ]            Starts revp in client mode
  -l [ --clientPort ] arg    [Server mode] Port that listens for client
  -p [ --proxyPort ] arg     [Server mode] Port that listens for proxy 
                             connections
  -a [ --serverAddress ] arg [Client mode] Address of server
  -z [ --serverPort ] arg    [Client mode] Port of server
```
