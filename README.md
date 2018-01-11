# PS4 GTA V Native Caller
A simple example of calling natives on PS4 GTA V using [idc's adaptation of CTurt's PS4 SDK](https://github.com/idc/ps4-payload-sdk).


## Brief Explanation
### Purpose
A large part of GTA V is controlled by custom script files Rockstar writes in their own format. A virtual processor is included in each version of the game to interpret these scripts. The main way for them to interact with and control the game is to invoke natives. Natives are functions defined within the game's executable. In the same way that scripts use natives to control the game, arbitrarily calling them will allow the caller to control the game to his/her desire. The purpose of this payload is to provide an easy way of doing just that. It includes a few small examples: making the player invincible, giving the player super jump, and teleporting the player when a button combination is pressed.
### Setting Up Execution
First, a kernel payload is executed to escape the sandbox, escalate the web browser's privileges, and make appropriate kernel patches. The kernel payload also disables ASLR for newly created processes, making it much easier to modify them. Afterwards, the browser payload constantly checks the running processes waiting for one called `eboot.bin`. Once a game process is found, the syscall ptrace is used to read and write to it. With ASLR disabled, the EBOOT always starts at `0x400000` in memory. A few bytes are read from the EBOOT to verify that the game is GTA V and to detect the region. Functions for making syscalls, invoking natives, and setting up the environment are copied into free executable space within the EBOOT. The native IS_PLAYER_ONLINE is hooked with the function to set up the execution environment. This function allocates more memory within GTA's process since the EBOOT has limited space. Once this setup function is called from GTA, the browser payload copies a standard main function into that newly allocated space and exits.
### Executing Inside GTA V
Once executing within GTA V, a structure called `gtaVars` is declared in some arbitrarily allocated memory to keep track of global variables. The native table has the same structure as on other platforms, making it easy to work with. The native hashes on PS4 GTA V 1.00 are also the same as the 1.00 native hashes on PC, meaning the documentation on [NativeDB](http://dev-c.com/nativedb/) can be used. The majority of the code in `nativeHook` is filtered to only execute once a frame (IS_PLAYER_ONLINE is called multiple times a frame), in order to keep things smooth. This will be more important for any drawing.


## Restrictions
### Functions
Functions called from `nativeHook` must be always inline or GTA will crash. This is because when compiled, `nativeHook` will expect those functions to be at specific locations relative to itself. Once `nativeHook` is copied to a different location, those relative addresses will be incorrect. The exceptions to this are functions declared in the payload by their absolute address such as `invokeNative`.
### Strings
Since `nativeHook` is copied to a different location, strings will have the same relative address issue as called functions. A simple way around this is to define them on the stack like:
	
	const char helloWorld[] = "hello world";
This is necessary so the compiler does not place the string in the data segment.
### Global Variables
Global variables also have the same relative address issue. In order to mitigate this, keep track of global variables with the `gtaVars` structure defined in `gta.h`.
### Space
By default 0x10000 bytes will be allocated for `nativeHook` and 0x4000 bytes for `gtaVars`. These sizes can be adjusted if need be.
### Multiple Instances
When executed multiple times, the payloads will replace each other rather than executing simultaneously.


## Miscellaneous Notes
### Natives
Only a few natives are defined in `natives.h`. However, more can be defined as necessary.
### Buttons
Button ID's for PS4 were different than last gen and PC so a simple mapping of them is included. `Button_Tpad_X` and `Button_Tpad_Y` can be used with the natives `GET_CONTROL_VALUE` and `GET_CONTROL_NORMAL` for touchpad input. `Button_Tpad` is just for if the touchpad is pressed.
### Testing
Since this payload injects functions into EBOOT memory, different versions of the payload can be tested without having to restart GTA.
### Closing Remarks
This is a fairly primitive way to go about modding a game, expect some strange and quirky bugs while using this.


## Credits
`Specter, CTurt, qwertyoruiopz, flatz, idc, SKFU, droogie, Xerpi, bigboss, Hunger, Takezo, and Proxima` - PS4 research making all this possible

`Alexander Blade and NativeDB Contributors` - Native research making GTA V stuff relatively easy to port to PS4