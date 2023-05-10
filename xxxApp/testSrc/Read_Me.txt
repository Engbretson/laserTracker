Ok, the first generation test code worked fine with a simulator since everything valid  and fake values exist for everything.

Talking to real hardware . . . i.e., a laser tracker right out of the box - there are no active tragets, warning messages might be spit out that might be signicicant,
the laser might not be on, and a whole load of other  issues - 'not supported features' for this model . . .  so everything really needs to be heavily try blocked.

So instead of implementing and testing things in alphabetical order, need to work though these things in a more logical order.

Connecting and initializing the device
Dumping out the values and state of things which don't actually require things be running,
and have a different test routine get called dealing with doing things that actually require "working" hardware.

So redo the test program(s) here to deal with 

Original version up this is point is/was
LaserTrackerTestMain

New Versions are 
QueryTrackerState

