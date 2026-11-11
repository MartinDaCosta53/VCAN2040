# Release Notes for VCAN2040

Releases are listed on GitHub: https://github.com/MartinDaCosta53/VCAN2040/releases

# 1.0.0 - Initial Release

# 1.0.1 - Change directory spelling

Enables workflows to be added

#1.1.0 - Remove ACAN2040 files from includes

Changes in `VCAN2040.h` and `VCAN2040.cpp` made so VCAN2040 talks to can2040 directly.


## Structure: ACAN2040 removed

- **Context:** the `can2040` context is a single file-static in `VCAN2040.cpp`, replacing the old 3-element array (roughly 600 bytes of RAM saved, estimated from the struct layout).
- **IRQ handlers:** the PIO IRQ handlers are static functions in `VCAN2040.cpp`. A `pioIrqInfo()` helper maps PIO 0, 1 or (RP2350 only) 2 to its IRQ and handler.
- **Startup:** `begin()` calls `can2040_setup`, `can2040_callback_config`, the IRQ setup and `can2040_start` directly. It returns `false` for a PIO instance the chip doesn't have.
- **Heap:** the `new ACAN2040` / `delete` pair and the `canp` pointer are gone.
- **Sending:** `can2040_check_transmit` and `can2040_transmit` are called directly.
- **Header:** `VCAN2040.h` includes `can2040.h` inside `extern "C"` and explicitly includes `hardware/irq.h` and `pico/util/queue.h`.
- **Delete:** `ACAN2040.h` and `ACAN2040.cpp` are no longer needed.

## Bug fixes

- **TX drain counter:** `available()` was incrementing the received counter when it drained the TX queue. That increment is gone (see Counters).
- **Serial output in the ISR:** the `Serial.printf` calls in the callback are removed.
- **Uninitialised frame:** `getNextCanFrame()` now returns a zeroed frame when the queue is empty.
- **Data copy:** `sendCanFrame()` clamps the `memcpy` to 8 bytes.
- **Reset:** `reset()` now disables the PIO IRQ and removes its handler (`stopDriver()`) before restarting. The old code re-registered an exclusive handler without removing the first.
- **Error notifications:** the callback tests notify flags instead of using `switch (notify)`, so notifications with an error code in the low bits are no longer missed.
- **Frame ID:** `getNextCanFrame()` masks off the can2040 RTR and extended-ID flag bits (`& 0x1fffffff`). `rtr` and `ext` are set from the flags, as before.

## Behaviour change

- **Send ordering:** `sendCanFrame()` sends directly only if the software TX queue is empty and can2040 has room. Otherwise it buffers the frame behind the backlog, so frames can't overtake each other.

## Counters

- **`transmitCounter()`:** counts `CAN2040_NOTIFY_TX` in the callback, so only frames that completed on the bus with an ACK are counted. The increments in `available()` and `sendCanFrame()` are removed.
- **`transmitErrorCounter()`:** returns `tx_attempt - tx_total` from `can2040_get_statistics()`, clamped at 0. It counts only bus-level failures.
- **`receiveErrorCounter()`:** returns `parse_error` plus the rx-overflow resets counted in the callback (`_numRxErr`).
- **`droppedFrameCounter()`:** new accessor for frames lost locally because both the can2040 queue and the software TX queue were full. It replaces the old `_numSendErr`, now `_numTxDropped`.
- **`getStatistics()`:** new public wrapper around `can2040_get_statistics()`.

## Error code

- **`lastErrorCode()`:** new accessor. On `CAN2040_NOTIFY_ERROR` the callback stores `notify & 0xFFFFF`. With the current can2040 it is always 0, because the only error reported is an rx FIFO stall and no code is passed. `begin()` and `reset()` zero it.

## Unchanged

`setPIO()`, `setPins(tx, rx)`, `setNumBuffers()`, the CBUS 125 kbps bitrate, `F_CPU` as the system clock, the IRQ priority of 1, and `printStatus()` (still empty).
