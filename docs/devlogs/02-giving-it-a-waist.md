# Devlog #2 — Giving the Arm a Waist

V1 was just the shoulder and elbow — a parallelogram floating on nothing. This pass I built the thing it's supposed to sit on: the base. That means a rotation joint, a bearing solution, and — more importantly — a clearer answer to *what this robot is actually trying to be*.

## What's new

- Designed the full base assembly: outer shell, swivel bearing interface, and rotating deck
- Waist joint driven by a 2:1 herringbone gear reduction off a third STS3215
- Locked several critical dimensions (base diameter, deck height, servo offset) to ±0.01 for print/machining review
- Nailed down the project's actual design goal (see below — this matters more than it sounds)
- Started thinking seriously about tool-head modularity for the business end of the arm

## The base / waist joint

**Mechanism:** Servo-driven pinion → herringbone ring gear on the rotating deck, 2:1 reduction.

Why herringbone specifically:
- Self-cancels axial thrust — a straight spur or single-helical gear at this scale wants to walk sideways under load, herringbone doesn't
- Keeps the swivel bearing doing only radial/rotational duty, not fighting gear-induced axial forces
- Smoother tooth engagement than spur, no need for a thrust bearing stack just to tame a helical gear's side-load

Why 2:1 reduction:
- Free torque multiplier at the base — the joint carrying the whole arm's reflected weight is exactly where you want it
- Halves the effective angular error at the output for a given amount of servo-side backlash/encoder step — cheap resolution win from the STS3215's 12-bit encoder
- Servo runs faster/lighter than it would direct-driving the deck, less heat under sustained load

**Bearing:** Going with a simple swivel interface (flat rotating contact, not a crossed-roller or four-point contact bearing) between the shell and the deck. It's the "cheap" part of cheap-yet-precise — accepting that this bearing alone won't be the tightest thing in the system, and leaning on the gear reduction + servo encoder to make up for it. If slop here turns out to dominate the error budget once it's built, that's the first thing I'll swap in V3 (probably toward a proper crossed-roller or a bearing bore taken out to tighter tolerance).

**Deck:** Rotating platform has a bolt pattern up top — outer ring + inner cluster — for mounting link 1 and, longer term, giving me a known-good hole pattern I can standardize other things against.

**Electronics:** Base servo joins the same daisy-chained bus as the shoulder and elbow servos through the Waveshare adapter. Still 3 wires, still one UART line, now 3 STS3215s on it instead of 2.

## Reframing the goal

Most hobby arm projects chase 6 DOF because that's what "real" robots have. I'm deliberately not doing that. 

**The actual goal: a cheap, consistently precise 3-DOF arm** — waist, shoulder, elbow. That's the whole Fanuc/KUKA palletizing topology this thing was inspired by in the first place; those robots don't need a wrist full of DOF to do useful, precise, repeatable work. 3 DOF is enough to reach a volume of space accurately and repeatably, and cutting the DOF count means fewer stacked sources of backlash, fewer things to tune, and a lower total cost — which lines up with the "anyone could build this from a parts list" goal way better than bolting on 3 more joints just to hit a bigger number.

Precision and repeatability become the actual metric to chase, not DOF count.

## Modularity: the tool end is the extensibility point

Instead of building DOF into the arm, the plan is to build **capability into swappable tool heads** at the end effector. Same 3-DOF base arm, different job depending on what's bolted on:

- **Dial gauge mount** — turns the arm into its own precision-measurement rig. Point it at a fixed reference, cycle the joints, read the repeatability directly off the gauge. Built-in self-validation for the "consistently precise" claim.
- **Vacuum pickup head** — palletizing demo, the original inspiration for the geometry, closes the loop back to why this topology was chosen.
- **4th DOF module** — an optional wrist rotation attachment for tasks that actually need it, without making it a tax on every build.

The design constraint going forward: every tool head shares a common mounting interface, so swapping between them is a bolt pattern away, not a redesign. That interface is the next thing I need to nail down before I start modeling individual tool heads.

## Next up

- Finalize the tool-head mounting standard (bolt pattern + electrical/pneumatic passthrough if needed for the vacuum head)
- Print the base and validate the herringbone mesh + swivel fit in PETG
- Get all 3 servos on the bus talking together and do a first coordinated move
