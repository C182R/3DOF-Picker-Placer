# Bill of Materials (living doc)

Components finalized so far. Pricing/links intentionally left blank — ping the repo owner or regenerate a fully priced version when ready to shop.

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
| 1 | XL4016 buck converter | Trimmed to 12V for servo bus |
| 1 | MP1584 mini buck converter | Set to 5V, feeds ESP32 VIN |
| 1 | 7.5A inline fuse | |
| 1 | SPST master switch | |
| 1 | 2200µF 25V low-ESR electrolytic cap | Across servo adapter power terminals |
| 1 | SMBJ14A TVS diode | Across servo adapter power terminals |
| 1 | XT60 connector | Tapped upstream of fuse, for bench supply |
| 1 | Mini voltmeter/ammeter module | Diagnostics |

## Structure
| Qty | Part | Notes |
|---|---|---|
| — | PETG filament | Primary structural material for FDM prints |
| — | M3 shoulder screws | Pivot pins — do not substitute printed pins |
| — | Herringbone ring gear + pinion (base) | 2:1 reduction, printed |
| — | Swivel bearing interface (base) | Simple flat swivel, not crossed-roller |

## Not yet finalized
- Vacuum pickup tool head (pump, nozzle, fittings)
- Dial gauge + mount
- Optional 4th-DOF wrist module
