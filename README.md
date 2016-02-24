# firmware
Firmware for blinkenrocket

Use `make && sudo make program` to flash a blinkenrocket and
`cd utilities; ./modem_transmit` to load example patterns.

## Known Bugs / TODOs

* Patterns longer than 252 bytes (excluding headers) are not yet supported
* Patterns longer than 28 bytes (excluding headers) can be saved, but only the
  first 28 bytes will be displayed
* Only slow transmissions are reliable at the moment (16kHz and a few
  ms of silence between patterns)
* The first `./modem_transmit` call does not work reliably --- Calling it a
  second time immediately afterwards (or a few seconds later) works fine
