# CRC EXPLAIN

a simple crc process simulate program. default setup is usb-crc5. 
of course, it can easyly simulate some other crc algorithmic by modify some macro.

### HOW TO:
````
> make
> ./crc
````

### Result:
````
----------------step  0----------------
             00101<==this is poly
              XOR
01001110000->11111<==this is crc reg
              NOT
             00000<==this is output
--------------------------------------
````
