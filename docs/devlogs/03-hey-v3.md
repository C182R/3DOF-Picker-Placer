# Devlog #3 — Hey v3

**Date:** September 17, 2026

This round was all about getting the design closer to the final product, mainly the robot base and the swivel base assembly. A lot more is actually in the CAD now: screws, heat-set inserts, bushings, bearings, and servos. I also designed the parts that mount the main arm assembly to the swivel deck, and fixed a handful of my own design setbacks and assembly issues that only showed up once real hardware was in the model.

## The base, fully detailed

From top to bottom, the stack is:

- the rotating deck with the mounts for the arm
- the top cover with the pocket for the J1 servo
- a hub plate
- the thrust roller bearing
- the deep groove ball bearing
- the 2:1 herringbone gear
- the housing, with a 608 bearing at the bottom for the gear shaft

Every screw that goes into plastic lands in a brass heat-set insert instead of printed threads. PETG threads strip after a few cycles, and this guy will probably get taken apart a lot during build prototyping. Modeling the actual screws also forced me to check head clearance, screw lengths, and whether an allen key or t-handle can even reach them once everything's stacked. It was boring but worth it.

The swivel deck is now the real interface between the base and the arm. The two J2 servos will sit on either side of the deck, and every arm pivot gets a bronze bushing as a cheap, replaceable wear surface. The pins and bushings carry the linkage loads, so the servos only have to supply torque instead of holding the arm up by their splines. I thought of direct driving just now, but it's currently just a thought.

## Some setbacks

- **The coupling with the curved support linkage.** After realizing some ROM by messing around a lot with the assembly, realized the front counterweight option I planned for had to go. To add, I moved the mounting towards the inside of the base instead of on the outside, which should give me some more movement liberty in the long term.
- **The actual shape of the robot base.** I rounded it out and focused on symmetry so I would have an easier time assembling. Converting to a circular shape and not just the simplest form also opened the gate for more equalized load bearing.

## Reality check !!

I don't have any of the hardware yet, so everything is designed off online CAD models. From experience, that'll set me back once the real parts show up.. nothing is ever perfect without real references.

- Downloaded models are usually nominal or simplified.
- Heat-set insert hole sizes change by brand.
- Bearing fits depend on the printer.
- Some more boring stuff that don't match up irl

So I'm trying to keep the critical interfaces easy to tweak, and once everything arrives I'll print small fit tests before committing to full parts.

## Next up I'm planning to..

- Give the arms the same treatment: pins, bushings, spacers, fasteners, the works.. and making it look super wicked. Still, functionality first.
- Double-check shoulder screw lengths against the bushing stacks.
- Update the BOM with the real fastener and insert counts.

The tool-head standard and the 1:1/direct drive question are still on the list too, but those have to wait for hardware.. gabagoo.
