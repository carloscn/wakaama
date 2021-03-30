Wakaama add object of client.

## Wakaama Server & Client start
### start

`./lwm2mclient -4 -n carlostest -h 127.0.0.1 -p 4994`

`./lwm2mserver -4 -l 4994`

### Use server

#### read temperature data from client.

read + #client_code + #source_url

`read 0 /25`  # *read source 25 all data.*

`read 0 /25/0`  # *read source 25 all object data.*

`read 0 /25/0/1`  # *read source 25 all object data.*

#### write temperature data from client.

write #client_code + *#source_url data*

`write 0 /25/0/1 25.5`  *# set max temp data is 25.5*

## Add a object by user

etc

### Using the Leshan server demo
GUI : https://leshan.eclipseprojects.io/#/clients
CMD : ./lwm2mclient -4 -n carlostest -h leshan.eclipseprojects.io -p 5683


```cpp
./configure --with-ssl=openss --with-openssl=/usr/local/openssl/ --without-zlib --with-libssl-prefix=/usr/local/openssl/
```


