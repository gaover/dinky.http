#obvious

Simple HTTP server for cpp

test result

os in Virtual Box: opensuse 15.2
cpu: 4 core 3.20G HZ
Memory: 8G

test cmd:
ab -n 400000 -c 8000 http://127.0.0.1:34568/hello
-------------------------------------------------
Server Software:
Server Hostname:        127.0.0.1
Server Port:            34568

Document Path:          /hello
Document Length:        11 bytes

Concurrency Level:      8000
Time taken for tests:   15.288 seconds
Complete requests:      400000
Failed requests:        0
Total transferred:      19600000 bytes
HTML transferred:       4400000 bytes
Requests per second:    26164.20 [#/sec] (mean)
Time per request:       305.761 [ms] (mean)
Time per request:       0.038 [ms] (mean, across all concurrent requests)
Transfer rate:          1252.00 [Kbytes/sec] received

Connection Times (ms)
              min  mean[+/-sd] median   max
Connect:        0  166 280.9    111    3159
Processing:    36  136  33.0    135     556
Waiting:        0   97  29.3     92     522
Total:        101  301 287.6    252    3510

Percentage of the requests served within a certain time (ms)
  50%    252
  66%    266
  75%    275
  80%    280
  90%    296
  95%   1134
  98%   1284
  99%   1312
 100%   3510 (longest request)
