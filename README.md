ReadP1
======

 Reading P1 Companion Standard of Dutch Smart Meter through external RX with an Arduino (or in my case a Nanode 5).

Explanation
===========

  P1 port uses a baudrate of 9600, 7 bits, E parity and 1 stop bit. Use a 7404 to invert data signal. Every 10 seconds you get an output something like: ![alt text](https://img.skitch.com/20120430-1ub7cy17s6rjnchyrbsnuprxcy.jpg "Output P1")

Documentation
=============
 See [Datalogging van "slimme meters"](http://www.zonstraal.be/wiki/Datalogging_van_%22slimme_meters%22) in Dutch

 See [P1 Companion Standard Version 2.2 April 18th 2008 from Enbin](http://read.pudn.com/downloads145/doc/633047/DSMR%20v2.2%20final/Dutch%20Smart%20Meter%20Requirements%20v2.2%20final%20P1.pdf) (also in docs directory)

 See [Representation of P1 telegram P1 Companion Standard Version 4.0 from Netbeheer Nederland](http://www.google.nl/url?sa=t&rct=j&q=p1%20companion%20standard&source=web&cd=1&sqi=2&ved=0CCkQFjAA&url=http%3A%2F%2Fwww.netbeheernederland.nl%2FDecosDocument%2FDownload%2F%3FfileName%3D1uII4GRHFdk98V78_gP-T4GttCG3SzdH9Vc0YXH328SvwKJJVRaTaKAmCYayrXZC%26name%3DDSMR%2BV4.0%2Bfinal%2BP1&ei=CHyeT5PgGc-VOs20-PsB&usg=AFQjCNE3sIY9JZ_RNEStaaA8YYv7iR0XkQ&sig2=PJXsfhIRCwWitgVgNrx2xQ) (also in docs directory)
 
 See [AVR USART INTRODUCTION](http://sites.google.com/site/qeewiki/books/avr-guide/usart) to understand how to set the USART to 7-E-1
