# Robot Arm Build — Devlog #1: V1 Design

**Date:** August 25, 2026

## Overview

This is v1 of the overall robot arm build, which is a 3 DOF antagonistic drive pick and placer, inspired by factory palletizing robots. The first physical version of a linkage-driven arm I've been working on in SolidWorks. I've already got a few issues with this version, just from looking at it on screen, but I'm going to 3D print it to see how it actually behaves in the real world before making changes.
## Design Approach
I used existing automation robot references for primitive geometry; I pulled references from Fanuc, KUKA, and Panasonic industrial/palletizing robots. These companies have decades of field-proven arm geometry behind them, so rather than guessing at joint offsets and link ratios from scratch, I used their designs as a sanity check for what actually works in practice. I'm already researching their electronic work using different control options like CANBUS and EtherCAT to implement (at least one version) into this robot.

**Topology optimization for the big structural pieces**
For the largest (by volume) components (anything with organic-looking cutouts or holes) I ran SolidWorks' topology optimization instead of hand-sketching lightening patterns. To make sure my studies were actually improving my design instead of making it worse, I cross referenced different parts across the industry (excavators, cranes, and other heavy equipment, since an excavator is what inspired me to create this robot) and used my knowledge in steel mechanics to see if a part would hold from a glance.


**Industrial robot references for the geometry**
For the practical link geometry and proportions, I pulled references from Fanuc, KUKA, and Panasonic industrial/palletizing robots. These companies have decades of field-proven arm geometry behind them, so rather than guessing at joint offsets and link ratios from scratch, I used their designs as a sanity check for what actually works in practice.

**DFM for FDM — with an eye toward aluminum later**
I designed with FDM 3D printing's strengths and weaknesses specifically in mind: layer orientation, load direction relative to layer lines, wall thickness, that kind of thing — rather than just printing "machined part" geometry and hoping it holds up. At the same time, I kept track of which components are candidates to eventually remake in aluminum once the design is proven out, so the plastic-specific DFM choices don't paint me into a corner for the metal version down the line.

## What's Next

- Print V1 and put it through real motion / hand-stress testing to confirm (or rule out) the issues I'm already suspecting
- Revise the design based on what breaks, binds, or flexes more than expected
- Start on the electronics side — though I'm going in with limited parts on hand, so that'll be its own challenge
