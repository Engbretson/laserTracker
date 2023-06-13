A *lot* of the laser tracker code involves callbacks, or type casting that is almost impossible to guess at. The docs on the header files and methods for Type Libraries leave much to be desired.

So while the epics build system can be used to compile the code, you actually need to use the MSVC GUI in order to have the compiler actually help auto complete the correct types for various things.

The vendor code also says that ceratin constructs are tecnically type conversions, suck as pulling each tracker out of a list of found trackers. HOWEVER, their example code does the exact same thing, and the type library object explorer says that there there is an object or a method to more correctly do this, but the compiler says "that doesn't exist."  Since the vendor code actually does seem to work, not a whole lot that can be done with it.

If I use the Borland compilers to import the Type Library/COM/NET object, the code that gets generated exposes the same values and methods that the C code admits to with the entire set of properties, but also correctly showing an Item[index] that the Object says should exist but seems to be a default value on the C/c++ side of things. In fact , one can use the world default to access the exact same field, but with a similar error message.

The C++ standard changed so that one really should not be doing <whatever it is/was that they were doing> , but not a lot that I can do about it. I assume that a later SDK will resolve this.

I still, however, read the msvc GUI so that all the dozens and dozen, and dozens of callback header will be generated corectly. Although still might have to guess at what type casting or conversions on the retrun data typs might be required.

