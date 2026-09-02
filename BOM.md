# Bill of Materials (living doc)

Components finalized so far. Pricing/links intentionally left blank. 3D printed parts should have files included in /hardware

## Actuation
| Qty | Part | Notes |
|---|---|---|
| 3 | Feetech STS3215 smart servo (12V, 30 kg·cm, 1:345) | Waist, shoulder, elbow. Built-in 12-bit magnetic encoder + serial bus daisy-chain + torque-disable for kinesthetic teaching |

## Control
| Qty | Part | Notes |
|---|---|---|
| 1 | ESP32 dev board | Main controller |
| 1 | Waveshare Bus Servo Adapter (A) | Jumper set to UART mode; drives the STS3215 daisy chain |

## Power
| Qty | Part | Notes |
|---|---|---|
| 1 | CH224K USB-C PD trigger board | Set to 20V — mandatory PDO on 65W+ chargers |
| 1 | USB-C receptacle / breakout | Mount the CH224K's own USB-C connector directly at the panel edge rather than routing through a separate panel-mount breakout — many cheap breakouts drop the CC lines and silently kill PD negotiation |
| 1 | USB-C PD charger, 65W+ | USB-C laptop charger works fine, but should be separate from USB C data cable |
| 1 | XL4016 buck converter | Trimmed to 12V for servo bus |
| 1 | MP1584 mini buck converter | Set to 5V, feeds ESP32 VIN |
| 1 | 7.5A inline fuse | |
| 1 | SPST master switch | |
| 1 | 2200µF 25V low-ESR electrolytic cap | Across servo adapter power terminals |
| 1 | SMBJ14A TVS diode | Across servo adapter power terminals |
| 1 | Mini voltmeter/ammeter module | Diagnostics |

# Input Path B -- XT60 (bench supply, battery)
| 1 | XT60 connector | Tapped upstream of fuse, for bench supply |
| - | External ~20V DC source | Bench supply, or a 5S LiPo + charger (~18.5–21V) if going battery-powered |


## Structure
| Qty | Part | Notes |
|---|---|---|
| — | PETG filament | Primary structural material for FDM prints. PLA will suffice for any low friction surface |
| — | M3 shoulder screws | (Ideally) Pivot pins. Substitute with PETG rods included in /hardware for primitive version |
| — | Herringbone ring gear + pinion (base) | 2:1 reduction, PETG printed |
| — | Swivel bearing interface (base) | Simple thrust roller bearing + roller/ ball bearing |

## Not yet finalized
- Vacuum pickup tool head (pump, nozzle, fittings)
- Dial gauge + mount
- Optional 4th-DOF wrist module
