Laser Tracker 

This is a first pass on a Leicia 930 series Laser tracker.

For lack of any specifications documents, this is a guess  at the functionality that might be required.

IMPORTANT NOTES - most screen parameters are only read once on startup.  Could have them be on demand, but will have to make sure that 
that the callbacks creations only happen once.

VERY FEW of the PV's are hooked up to write settings back to the hardware. Trivial to do on any specific one, but a pain to do the
771 that currently exist, since you have to dynamical cast things into the correct classes to be able to correctly
write to them. At compile time, I can only do that in the most redial way.  

I have hard encoded continious collection with a 1 second period as the default  mode.  High Presion one-shots are also implemented.
Not sure what the other modes might be used for. Since I have to be able to STOP things to switch modes, some of the automagic code
that I originally coded so that if it saw a target, it would acquire - has been turned off, since the code will crash if you switch 
measurement profiles while measuring, or isn't too happen even trying to measure when there is nothing actually there *to* measure.

A LOT of PV's and someone really needs to determine how they really want to use this and make up screen that have 
the correct minimal information all-in-one and skip all the fluff.

I am assumeing that some modes and sensors and distance populates more of the readout values that currently are filled wil lots of zero's.
