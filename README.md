# PS4 GTA V Native Caller
A simple example of calling natives for GTA V using [CTurt's PS4 SDK](https://github.com/CTurt/PS4-SDK).




## Brief Explanation
### Setting Up Execution
The payload first escalates privileges using the [dlclose kernel exploit](https://github.com/kR105-zz/PS4-dlclose). The kernel payload also disables ASLR for newly created processes, making it much easier to modify them. Afterwards, the browser payload constantly checks the running processes waiting for one called `eboot.bin`. Once a game process is found, the syscall ptrace is used to read and write to it. With ASLR disabled, the EBOOT always starts at `0x400000` in memory. The payload reads a few bytes from the EBOOT to verify that the game is GTA V and to detect the region. The payload makes 3 edits to the EBOOT. Luckily, the EBOOT's main executable memory page has about 0xE000 free bytes at its end. Two functions are copied into that free space: a standard main function, `nativeHook`, and another for calling natives by hash, `invokeNative`. The last edit hooks the IS_PLAYER_ONLINE native to call the recently copied main function. After this, the browser payload exits.
### Executing Inside GTA V
Once executing within GTA V, a structure called `gtaVars` is declared in some more free memory to keep track of global variables as well as a native argument structure. In order to keep the source simple and easily manageable, all functions called from `nativeHook`, besides `invokeNative`, are always inline. This includes the native setup functions which copy the arguments into the native arguments structure in and then invoke the native by hash. The native table has the same structure as on other platforms, making it easy to work with. The native hashes on PS4 GTA V 1.00 are also the same as the 1.00 native hashes on PC, meaning the documentation on [NativeDB](http://dev-c.com/nativedb/) can be used. The majority of the code in `nativeHook` is filtered to only execute once a frame (IS_PLAYER_ONLINE is called multiple times a frame), in order to keep things smooth. This will be more important for any drawing.


## Restrictions
### Functions
As mentioned in the brief explanation, functions called from `nativeHook` must be always inline or it will crash. This is because when compiled, `nativeHook` will expect those functions to be at specific locations relative to itself. Once `nativeHook` is copied to a different location, those locations will be incorrect. The exceptions to this are functions inside the EBOOT that are declared in the payload by their address such as `invokeNative` and any additional functions that may be needed, such as snprintf.
### Strings
Since `nativeHook` is copied to a different location, strings cannot be used inside it in the normal way. A simple way around this is to define them on the stack like:
	
	char helloWorld[] = { 0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0x57, 0x6f, 0x72, 0x6c, 0x64, 0x0 };
This seems strange, but it is a bit of a trick with the compiler. The bytes for the string will be stored inside the function rather than the data segment of the payload.
### Global Variables
Global variables have the same issue as strings. As previous mentioned, a good way to manage them is with a structure. However, there is not that much free space left at the address `gtaVars` and the native arg structure are declared at. For this reason, it is recommended to keep global variable usage to a minimum.
### Space
0xE000 bytes will run out very quickly. Using inline functions and defining strings on the stack will cause the size of `nativeHook` to be quite large. For reference, the simple mod menu v1.0 needed about 0x3800 of those bytes.
### Multiple Instances
Since the functions in this payload are copied into gta at a static address, multiple payloads cannot be run at the same time. They would just replace each other.


## Miscellaneous Notes
### Natives
Only a few natives are defined in `natives.h`, just copied the ones needed for the simple mod menu v1.0. Defining any more is easy enough.
### Buttons
Button ID's for PS4 were different than last gen and PC so a simple mapping of them is included. `Button_Tpad_X` and `Button_Tpad_Y` can be used with the natives `GET_CONTROL_VALUE` and `GET_CONTROL_NORMAL` for touch pad input. `Button_Tpad` is just for if the touch pad is pressed.
### Testing
Since this payload injects functions into EBOOT memory, different versions of the payload can be tested without having to restart GTA. This is a great difference from developing on last gen consoles.
### Bugs & Closing Remarks
While developing the simple gta menu, I ran into numerous unexplained bugs that disappeared just as mysteriously. Expect that while developing with this. It will be an interesting experience. This is fairly simple way to go about modding a game, which has its disadvantages.


## Credits
`BadChoicesZ` - Notifications code

`ZiL0G80` - Disable process ASLR address

`Zecoxao` - Firmware version spoof address

`kR105` - Dlclose exploit sample code

`CTurt, flatz, SKFU, droogie, Xerpi, bigboss, Hunger, Takezo and Proxima` - PS4 SDK and background PS4 research making all this possible

`Alexander Blade and NativeDB Contributors` - Native research making GTA V stuff relatively easy to port to PS4
