Inspired by the recent wave of source code releases and the upcoming 10th
birthday of Conspiracy, we give you the tools used to produce the bulk of
our releases. This pack contains the source code for all of our stuff up
until Chaos Theory, including the tools and some of the intros themselves.

Contents
-aDDict:
	-The original aDDict tool used in Project Genesis and A Place
	 Called Universe
-aDDict2:
	-The tool used for everything up until Chaos Theory

-APCU:
	-Buildable version of A Place Called Universe

-ChaosTheory:
	-Buildable version of Chaos Theory. The rest of the aDDict2 intros
	 can be built using this codebase if you feel like it. 
	 The includelist.h contains #ifdefs for parts of the engine required
	 for the specific release, you can either uncomment all of it or
	 generate a new file using the includelist command in the aDDict2
	 console prompt.

-ProjectGenesis:
	-Buildable version of Project Genesis

All of the code has been fixed to work under VS2010, however keep in mind
that most of this has been hacked beyond recognition and should be considered
quite unstable code.

Here follows a probably incomplete list of what has inspired parts of the code:
-The ATG texture generator (Greetings to Ile ;)
-An attempt at loading scenes from zoom3 (Hey AND ;)
-A Kasparov scene loader (Sadly Elitegroup has perished)
-Some of the filters used in PG and APCU:
	-http://www.musicdsp.org/showone.php?id=117
	-http://www.musicdsp.org/showone.php?id=27
-Spline code
	http://debian.fmi.uni-sofia.bg/~sergei/cgsr/docs/splines.txt
-A hacked to hell minifmod
-Bass sound system
-Fmod sound system