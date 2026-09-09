# Devlog #2 — Giving the Arm a Waist

This is v2.

## What's new

I designed the full base assembly: outer shell, swivel bearing interface, and rotating deck. The waist joint (J1) driven by a 2:1 herringbone gear reduction off a STS3215. After research, I chose to use these motors with a 345:1 @12V on each, up to four motors total: one for the yaw, 2 powering the shoulder linkage (J2), and another for an eventual 4th DOF (J4) on the toolhead. This tiny motor already has a built in 12-bit encoder, which eliminates the need for an additional electronic part, aka additional point of failure.

Nailed down the project's actual design goal (see below — this matters more than it sounds)
Started thinking seriously about tool-head modularity for the business end of the arm.

## The base / waist joint

**Mechanism:** Servo-driven pinion → herringbone ring gear on the rotating deck, 2:1 reduction.

I decided on herringbone gears since they self-cancels axial thrust; a straight spur or single-helical gear at this scale wants to walk sideways under load, herringbone doesn't (that much). Plus, I'm 3D printing it, so machining manufacturing isn't a concern (yet). I also use a deep groove ball bearing on the outside for smooth rotation and a thrust roller bearing on top to reduce my axial loads. Initially, I was planning to use opposing tapered roller bearing, but this approach is cheaper (and lighter..).

For the 2:1 ratio, my main motivator was to get the least strain (and heat) on the only tiny motor carrying the weight of the entire robot (excluding payload). Using torque calculations, I realized that the 2:1 might be unnecessary and slow, so I already envision converting to a 1:1 or direct drive mechanism after seeing the arm in action. In the next iteration, I'll tackle the modularity of the bearing base to be able to use an assortment of different sized bearings and sprockets, making it easier for anybody wanting to give the build a shot. My only concern is that this joint could be a silent generator of slop, which can hopefully be mitigated by the servo encoder.

**Electronics:** Base servo joins the same daisy-chained bus as the shoulder and elbow servos through the Waveshare adapter. Still 3 wires, still one UART line. Diving more into this later on.

## The goal

While most hobby arm projects chase 6 DOF because that's what "real" robots have, I'm deliberately not doing that. It's kind of overdone/ overrated, but it still amazingly effective. This robot has a different purpose.

**The actual goal:** A cheap, consistently precise 3-DOF arm waist, shoulder, elbow, amazing to use by itself but increasingly more capable with added attachments/ accessories.. a bit like an Ender 3..lol. That's the basis of the palletizing topology this guy was inspired by in the first place; those robots don't need a wrist full of DOF to do useful, precise, and repeatable work, and one robot can be adjusted/ modified to perform a plethora of tasks. Precision, repeatability, and real world practicality is the actual metric I'm chasing, not DOF count. In my eyes, I don't want to build this guy and be done with him, I want to get and learn the most of the robot, especially since he'll be mostly plastic.

## Modularity: the tool end is the extensibility point

Instead of building DOF into the arm, the plan is to build capability into swappable tool heads at the end effector. Same 3-DOF base arm, different job depending on what's connected on, for instance:

- **Dial gauge mount** — turns the arm into its own precision-measurement rig. Point it at a fixed reference, cycle the joints, read the repeatability directly off the gauge. Built-in self-validation for the "consistently precise" claim. (idea!). Even if it not an actual dial gauge (since those things are serious measuring apparatus), something similar than can display the precise repeatability. 
- **Vacuum pickup head** —  palletizing demo, the original inspiration for the geometry. Palletizing is actually a main driver on why the linkages are designed like they are. They're designed to hold REAL weight, with some FANUC and KUKA robots with similar linkages designed to precisely pick and place 700kg payloads.
- **4th DOF module** — an optional wrist rotation attachment for tasks that actually need it, without making it a tax on every build.

The design constraint going forward: every tool head shares a common mounting interface, so swapping between them is a bolt pattern away, not a redesign. That interface is the next thing I need to nail down before I start modeling individual tool heads.

## Next up I'm planning to..

- Finalize the tool-head mounting standard (connection pattern + electrical/pneumatic passthrough if needed for the vacuum head, but pneumatics should not be treated lightly, so I think this would be a final design).
- Print the base and validate the herringbone mesh + swivel fit in (preferably in) PETG
- Get all 3 servos on the bus talking together and do a first coordinated move
