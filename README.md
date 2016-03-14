# firmware
Firmware for blinkenrocket

Use `make && sudo make program` to flash a blinkenrocket and
`cd utilities; ./modem_transmit string1 string2 string3 ...` to load some patterns.

## Known Bugs / TODOs

* Pattern transfers which are longer than ~16kbit don't work reliably and
  may leave the device in a bogus state (requiring a power on reset before
  new patterns can be transferred)
