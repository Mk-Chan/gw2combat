[![Master build](https://github.com/Mk-Chan/gw2combat/actions/workflows/master-build.yml/badge.svg)](https://github.com/Mk-Chan/gw2combat/actions/workflows/master-build.yml)
# gw2combat
This program aims to accurately simulate Guild Wars 2's in game combat.

It works by implementing a configuration system which allows someone to define how the combat actually plays out in the game based on 2 parts:
1. Build
    - Gear-based stats
    - Effects (standard boons & conditions)
    - Unique Effects (includes traits, food, utils and more)
    - Skill definitions (cast times, damage coefficients, cast durations and more)
    - Other gw2combat specific parts

2. Rotation
    - A list of skills (possibly with the exact times they were cast)

Currently, the program is extremely accurate at simulating benchmarks as long as build definitions and rotations are provided correctly.
It does so by playing out the skills provided in the rotation according to skill definitions and attributes/effects in the build configuration.

Documentation for how to create and edit build configurations is under development.

Build the program with a single command: `make` (faster with `make -j<number of cores>`)

Reference implementations for some classes are provided in resources/. The starting point for any simulation is an encounter json `resources/encounter.json`.

Here's a live deployment of gw2combat to power the gw2wingman rotation optimizer tool.
Click to view the video on YouTube.
[![Watch the video](https://img.youtube.com/vi/O_kDiFJINt8/maxresdefault.jpg)](https://youtu.be/O_kDiFJINt8)
