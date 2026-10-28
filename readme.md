# Clean NerdQAxe++ firmware for the 480×320 (3.5") screen

> **Only for NerdQAxe++ units with the 480×320 (3.5") screen.** If yours has the stock 1.9" 320×170 screen, use upstream [`shufps/ESP-Miner-NerdQAxePlus`](https://github.com/shufps/ESP-Miner-NerdQAxePlus/releases) directly — it already includes W5500 Ethernet. This repo no longer publishes 320×170 builds.

Open, auditable firmware for the **NerdQAxe++** clones with the **480×320 (3.5")** panel sold on AliExpress (sellers such as YYSlupping, among others). Those clones ship with a **preinstalled binary firmware whose source isn't published**, so there's no way to see what it does with your pool credentials, payout address, or hashrate. This repo lets you replace it with a clean build compiled from the open [`shufps/ESP-Miner-NerdQAxePlus`](https://github.com/shufps/ESP-Miner-NerdQAxePlus) source, the same firmware the rest of the NerdQAxe community runs, so you know exactly what's on your miner.

| Supported Targets | ESP32-S3           |
| ----------------- | ------------------ |
| Required Platform | >= ESP-IDF v5.3.X  |

On top of upstream (tracking `shufps`'s `develop`), this fork adds:

1. **480×320 (3.5") screen support** — ported from [brunneis/nerdqaxeplus2-3.5-inches](https://github.com/brunneis/nerdqaxeplus2-3.5-inches); upstream has said they won't support the 480×320 panel, so it's maintained here.
2. **Small Ethernet fixes** — the screen keeps showing "LAN Connected!" while WiFi retries in the background.

**Ethernet itself comes from upstream**: W5500 support for the NerdQAxe++ is part of `shufps`'s firmware since [#731](https://github.com/shufps/ESP-Miner-NerdQAxePlus/pull/731). It's optional and auto-detected — with no Ethernet shield the firmware runs on WiFi, exactly like stock. Wiring is [below](#ethernet-w5500-wiring).

<p align="center"><img src="miner.jpeg" alt="480x320 NerdQAxe++ clone running this firmware with the W5500 Ethernet shield" height="300"></p>

*Running this fork with the W5500 shield installed. The **chain-link icon** at the top of the screen (beside the IP) means it's on a wired Ethernet connection — it's absent when running over WiFi.*

Credits:
- BitAxe devs on OSMU: @skot/ESP-Miner, @ben and @jhonny
- NerdAxe dev @BitMaker
- Upstream NerdQAxe firmware: @shufps

### Status

- ✅ Builds clean for `BOARD=NERDQAXEPLUS2`, target `esp32s3`, `BIGSCREEN=1`. CI compile-checks it on every change.
- ✅ **Verified on real hardware.** The SquareLine-Studio layout (authored for 320×170) is scaled to fill the 480×320 panel; mining screen, fonts, network/status icons, and the block-found overlay all render correctly.
- ✅ **Ethernet is hardware-verified** on a NerdQAxe++ with a W5500 shield: the wiring below is confirmed working and the board runs on Ethernet.

### Ethernet (W5500) wiring

Only 4 signal wires needed (plus power/ground), the same pinout upstream uses:

| Signal | GPIO |
|---|---|
| MOSI | 12 |
| MISO | 16 |
| SCLK | 2  |
| CS   | 21 |

![W5500 shield wiring — 20×14 perfboard, front and back](board.png)

*The W5500 module is drawn on top for reference only; it actually mounts on the opposite side of the board. The 12-pin pass-through header is where the NerdQAxe++ (ESP32) plugs in; the W5500 sits on the 5-pin headers.*

INT and RST aren't used — the driver polls, and resets the W5500 over SPI on boot. The firmware enables Ethernet only when it finds a W5500 on these pins at boot (not when a CAN add-on is fitted, which uses GPIO16/21 too).

## Building this fork

Uses the repo's Docker toolchain, so you don't need ESP-IDF or Node installed locally (the image pins the tested ESP-IDF 5.3.6). **First time only**, build the container:

```bash
cd docker && ./build_docker.sh && cd ..
git submodule update --init --recursive   # if you didn't clone with --recursive
```

Then build — the two fork-specific bits are `BOARD=NERDQAXEPLUS2` and `BIGSCREEN=1` (the 480×320 panel):

```bash
export BOARD="NERDQAXEPLUS2"
export BIGSCREEN=1          # 480x320 (3.5") panel
./docker/idf.sh set-target esp32s3
./docker/idf.sh build
```

That produces `build/esp-miner.bin` (app) and `build/www.bin` (web UI).

## Installing & updating

Every [Release](https://github.com/AguiMr/nerdqaxeplus2-ethernet/releases) attaches ready-to-flash 480×320 binaries (or build your own — see above):

| First-time USB flash | In-app / OTA update |
|---|---|
| `esp-miner-factory-NerdQAxe++-480x320-<ver>.bin` | `esp-miner-NerdQAxe++-480x320.bin` + `www.bin` |

**First install (coming from the stock/unknown firmware): USB serial.** The partition table differs from a stock NerdQAxePlus2 (app partitions enlarged for the theme assets — see `partitions.csv`), so the first flash can't be done over the air. Hold the `boot` button to enter bootloader mode, then flash the factory image at `0x0`:

```bash
esptool.py --chip esp32s3 -p /dev/ttyACM0 write_flash 0x0 esp-miner-factory-NerdQAxe++-480x320-<ver>.bin
# or:  ./docker/bitaxetool.sh --firmware esp-miner-factory-NerdQAxe++-480x320-<ver>.bin -p /dev/ttyACM0
```

(From a self-built tree instead: `idf.py -p /dev/ttyACM0 flash` inside `./docker/idf-shell.sh`, or `./merge_bin.sh nerdqaxe+.bin` to produce your own factory image.)

**Updating once you're already on this firmware — no USB needed:**
- **In-app (easiest):** web UI → Settings → **Update via GitHub** → pick the latest release. It installs the 480×320 build, preserving your settings (pool, overclock) in NVS.
- **Manual upload:** web UI → firmware upload (`esp-miner-NerdQAxe++-480x320.bin`) and website upload (`www.bin`).

> Not on shufps's Webflasher — that only carries upstream builds, not these. Use this repo's own releases.

**Going back to upstream** (e.g. a 320×170 unit that ran an earlier release of this fork): flash upstream's `esp-miner-factory-NerdQAxe++-<ver>.bin` over USB at `0x0`, the same way as above. It has to be USB, because it restores the stock partition table.

## Upstream features

This fork tracks `shufps/ESP-Miner-NerdQAxePlus`, so its generic features work unchanged — see the [upstream README](https://github.com/shufps/ESP-Miner-NerdQAxePlus) for full details:

- **Grafana / InfluxDB monitoring** — the firmware supports InfluxDB; upstream ships a Grafana dashboard and compose setup: https://github.com/shufps/ESP-Miner-NerdQAxePlus/tree/master/monitoring
- **Opt-in panic core dumps** — a diagnostic build, enabled by layering `SDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.coredump"` before `idf.py set-target esp32s3 && idf.py build`. (Upstream's release-CI path for this doesn't apply here — this fork doesn't carry that workflow.) ⚠️ Core dumps can contain pool/WiFi credentials from task RAM, so don't share a raw dump casually.
