# Parrot
🦜 — Point and Click Adventure Game Player

## Description

This is a very early days Point and Click Adventure Game Player for the Commodore Amiga. 

## You've heard of ScummVM right?

Yes, and ScummVM has been ported to the Amiga. This is different, in a few ways:-

* We don't use any ScummVM code.
* This is written purely for the Amiga.
* The game assets are converted into Amiga compatible file formats.

## Amiga compatible file formats?

Yes. One of the reasons why ScummVM is slow on the Commodore Amiga, it uses a "chunky" method of rendering content. 
The Amiga is natively planar, so every frame this has to be converted over. This requires non-stock Amiga hardware to get any decent framerates.

Parrot does it differently. Using external tools they pre-convert all the game files into a common data file format (Squawk). 
Sprites, Bitmaps are all in Planar, and the palettes are carefully calculated. Music and Sound effects are in Amiga formats too. 
These tools are run on your Amiga before your first play.

Parrot is data-orientated, so it doesn't know about the game Maniac Mansion, or Lure of the Temptress. It's just Sqwark data, and as
long as the game has Rooms, Actors, Cutscenes and ocassional beep. It can play it.

## Can my Amiga play it?

Probably. Our minimum requirement for a playable Maniac Mansion is an ECS Amiga, running Workbench 2.

## Can I play "Insert Game Here"

Not yet. Current focus is to get Parrot to play Maniac Mansion. It is the grand-daddy of "Point and Click's" and we have to start somewhere.

## What language are you writing this in?

It is C, using the VBCC compiler. Assembly may be used to "tighten up those graphics on Level 3"

## Why the name Parrot?

It's a reference in Monkey Island 2. You've probably never heard of it. And its better name than a bodily fluid.

## Is this Free?

Free and Open Source (MIT).
