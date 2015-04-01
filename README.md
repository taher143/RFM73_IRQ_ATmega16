## RFM73_IRQ_ATmega16 Receiver code example
Interrupt ebabled RFM73 code for ATmega16
RFM73 code has been updated to use in asynchronous mode. ATmega16 low level interrupt on INT0 is used.
When there is message on RF receiver, Atmega will get low level interrupt and set falg to read data from RX_BUFFER.

code reference is from http://www.embeddedwirelesssolutions.com/projects/EWS_RFM73_Chat_Example.pdf
