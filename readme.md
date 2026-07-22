[![](https://dcbadge.vercel.app/api/server/3E8ca2dkcC)](https://discord.gg/3E8ca2dkcC)

# ESP-Miner-Nerdaxe version

| Supported Targets | ESP32-S3              |
| ----------------- | --------------------- |
| Required Platform | >= ESP-IDF v5.3.X       |
| ----------------- | --------------------- |

This is a forked version from the NerdAxe miner that was modified for using on the [NerdQAxe+](https://github.com/shufps/qaxe).

Credits to the devs:
- BitAxe devs on OSMU: @skot/ESP-Miner, @ben and @jhonny
- NerdAxe dev @BitMaker

## About this fork

This branch (`lan-480x320`) merges two things onto current `shufps/ESP-Miner-NerdQAxePlus` (`develop`) for the **NerdQAxePlus2 with a 480x320 (3.5") screen**:

1. **480x320 display support**, ported from [brunneis/nerdqaxeplus2-3.5-inches](https://github.com/brunneis/nerdqaxeplus2-3.5-inches). Upstream (`shufps`) doesn't support this panel and has said they won't, due to concerns about the Chinese manufacturer of the 480x320 variant — this is a community-maintained addition, not something expected to land upstream.
2. **W5500 Ethernet**, enabled via upstream's own native `Board::hasEthernet()` / `NetworkManager` infrastructure (originally built for the Q1370/Q1373 boards). No code was pulled in from [CryptoIceMLH/ESP-Miner-NerdQAxePlusLAN](https://github.com/CryptoIceMLH/ESP-Miner-NerdQAxePlusLAN) — upstream's own implementation is more current — but that project's README confirmed the community-standard SPI pinout used below.

### Status

- ✅ Builds clean for `BOARD=NERDQAXEPLUS2`, target `esp32s3`, with `BIGSCREEN=1`.
- ⚠️ **`main/displays/ui.cpp` has not been adapted for the bigger canvas.** It's SquareLine-Studio-generated layout code written for the 320x170 screen; the UI will currently render using those old coordinates on the 480x320 panel instead of filling it. This needs to be done by iterating against a real device, not guessed from source.
- ⚠️ **Ethernet pinout is source-verified, not hardware-verified.** See below.

### Ethernet (W5500) wiring

| Signal | GPIO | Notes |
|---|---|---|
| MOSI | 12 | |
| MISO | 16 | |
| SCLK | 2  | |
| CS   | 21 | |
| INT  | 11 | not required — driver works in polling mode too |
| RST  | **4** | moved from the upstream default (GPIO13); GPIO13 is already used as `LDO_EN_PIN` on this board (see `main/boards/nerdqaxeplus.cpp`), so reusing it for W5500 reset would conflict with the board's power sequencing |

MOSI/MISO/SCLK/CS/INT match the pinout documented in CryptoIceMLH's README, which appears to be a de facto community standard for W5500 add-on/interposer boards for these boards. **RST was changed to GPIO4 in firmware because of the GPIO13 conflict — if your specific W5500 add-on hardwires RST to GPIO13, that's a physical conflict no firmware change can fix**, and would need a hardware modification (bend/cut the pin and jump it to GPIO4) or confirmation from whoever built that add-on board about its actual wiring. Override point is `NerdQaxePlus2::getEthResetPin()` in `main/boards/nerdqaxeplus2.h` if a different pin is ever needed.

### Building this fork

Same as below, with two additions: set `BIGSCREEN=1` to get the 480x320 display code, and note the board is always `NERDQAXEPLUS2`.

```bash
export BOARD="NERDQAXEPLUS2"
export BIGSCREEN=1
./docker/idf.sh set-target esp32s3
./docker/idf.sh build
```

### Flashing

**The partition table was changed** (app partitions enlarged, `www` shrunk) to fit the larger 480x320 theme assets — see `partitions.csv`. This means the **first flash of this firmware must be a full serial flash** (`idf.py flash`, `bitaxetool`, or the merge_bin scripts below), not an OTA update from an existing NerdQAxePlus2 firmware, since OTA doesn't repartition the flash.


## How to flash/update firmware

The newest releases are always here:

https://github.com/shufps/ESP-Miner-NerdQAxePlus/releases

### Recommended Method: The Webflasher

The [Webflasher](https://shufps.github.io/nerdqaxe-web-flasher/) (modified fork of the great [Bitaxe Webflasher](https://github.com/bitaxeorg/bitaxe-web-flasher) by [Wantclue](https://github.com/WantClue)) is the easiest method of updating all Nerd*axe variants.

[<img src="https://github.com/user-attachments/assets/4168f23a-bfe7-4536-91e3-7af6df9a203a" style="border:5px solid red;width:200px">](https://shufps.github.io/nerdqaxe-web-flasher/)

It uses the official releases published on this repository and is always up-to-date.

### Other Methods

#### Clone repository and prepare config

First you need to clone the repository and create a local copy of the config file:

```bash
# clone repository
git clone --recursive https://github.com/shufps/ESP-Miner-NerdQAxePlus

# change into the cloned repository
cd ESP-Miner-NerdQAxePlus

# copy the example config
cp config.cvs.example config.cvs
```

Then you can edit the fields like `stratumurl` and so on.

#### Bitaxetool

After the changes on the `config.cvs` files are done, you use the `bitaxetool` to flash factory binary and the config onto the device.

To switch it into bootload mode, reset the device with presset `boot` button.

```
bitaxetool --config ./config.cvs --firmware esp-miner-factory-NERDQAXEPLUS-v1.0.10.bin

```


## How to build firmware

### Using Docker

Docker containers allow to use the toolchain without installing `esp-idf` or `Node 20.x` on the system.

#### 0. TL;DR - `esp-miner.bin`, `www.bin`
```bash

# only once
cd docker
./build_docker.sh
cd ..

# only needed if you cloned without --recursive
git submodule update --init --recursive

export BOARD="NERDQAXEPLUS2"
./docker/idf.sh set-target esp32s3

# after each change on the source code
./docker/idf.sh build
```

Afterwards you will have a `esp-miner.bin` and `www.bin` in your `build` directory.


#### 1. First build the docker container

```bash
cd docker
./build_docker.sh
```

#### 2. How to use it

There are several scripts in the `docker` directory but what is most flexible is to just start the container as bash via

```bash
./docker/idf-shell.sh
```

You will get a new terminal that provides tools like:
- `idf.py`
- `bitaxetool`
- `esptool.py`
- `nvs_partition_gen.py`

The current repository will be mounted to `/home/builder/project`.

The default `builder` user has `uid:gid = 1000:1000` (like the main user on *buntu/Mint)

#### 3. Compiling & Flashing using the shell

#### 3.1. Just flashing with dockered `bitaxetool` with factory binary

(no `idf-shell.sh` version)

```bash
./docker/bitaxetool.sh --config config.cvs --firmware esp-miner-factory-NERDQAXEPLUS-v1.0.10.bin -p /dev/ttyACM0
```

##### 3.2. Compiling & Flashing using BitAxe tool

(inside of `idf-shell.sh`)

```bash
# start idf-shell
./docker/idf-shell.sh

# set board
export BOARD="NERDQAXEPLUS2"

# set target and build the binaries
idf.py set-target esp32s3
idf.py build

# merge all partitions including config into a single binary
./merge_bin.sh nerdqaxe+.bin

bitaxetool --config config.cvs --firmware esp-miner-factory-nerdqaxe+.bin  -p /dev/ttyACM0
```

#### 3.3. All manual steps for building and flashing

(inside of `idf-shell.sh`)

```bash
# start idf-shell
./docker/idf-shell.sh

# set board
export BOARD="NERDQAXEPLUS2"

# set target and build the binaries
idf.py set-target esp32s3

# optional if you want to change the sdkconfig
idf.py menuconfig

# build the binaries
idf.py build

# creat config.bin nvm partition from config.cvs
nvs_partition_gen.py generate config.cvs config.bin 12288

# merge all partitions including config into a single binary
./merge_bin_with_config.sh nerdqaxe+.bin

# flash using esptool
esptool.py --chip esp32s3 -p /dev/ttyACM0 -b 460800 \
  --before=default_reset --after=hard_reset write_flash \
  --flash_mode dio --flash_freq 80m --flash_size 16MB 0x0 nerdqaxe+.bin
```


When done just `exit` the shell.


### Without Docker

Install bitaxetool from pip. pip is included with Python 3.4 but if you need to install it check <https://pip.pypa.io/en/stable/installation/>

```
pip install --upgrade bitaxetool
```

## Grafana Monitoring

<img src="https://github.com/user-attachments/assets/3c485428-5e48-4761-9717-bd88579a747d" width="600px">

The NerdQaxe+ firmware supports Influx and the repository provides an installation with Grafana dashboard that can be started with a few bash commands: https://github.com/shufps/ESP-Miner-NerdQAxePlus/tree/master/monitoring


