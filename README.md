# PS4 GTA V Native Caller
A simple example of calling natives on PS4 GTA V using [idc's adaptation of CTurt's PS4 SDK](https://github.com/idc/ps4-payload-sdk).


## Brief Explanation
### Purpose
A large part of GTA V is controlled by custom script files Rockstar writes in their own format. A virtual processor is included in each version of the game to interpret these scripts. The main way for them to interact with and control the game is to invoke natives. Natives are functions defined within the game's executable. In the same way that scripts use natives to control the game, arbitrarily calling them will allow the caller to control the game to his/her desire. The purpose of this payload is to provide an easy way of doing just that. It includes a few small examples: making the player invincible, giving the player super jump, and teleporting the player when a button combination is pressed.
### Setting Up Execution
First, a kernel payload is executed to escape the sandbox, escalate the web browser's privileges, and make appropriate kernel patches. The kernel payload also disables ASLR for newly created processes, making it easier to modify them. Afterwards, the browser payload constantly checks the running processes waiting for one called `eboot.bin`. Once a game process is found, the syscall ptrace is used to read and write to it. With ASLR disabled, the EBOOT always starts at `0x400000` in memory. A few bytes are read from the EBOOT to verify that the game is GTA V and to detect the region. A small function for setting up the execution environment is copied into free executable space in the EBOOT and `IS_PLAYER_ONLINE` is hooked to call it. This function allocates more memory within GTA's process since the EBOOT has limited space. Once the setup is complete, the GTA payload is copied into the newly allocated space, the setup function starts executing it, and the browser payload exits.
### Executing Inside GTA V
The GTA payload will execute any time the native `IS_PLAYER_ONLINE` is called by one of Rockstar's scripts. The native table has the same structure as on other platforms, making it easy to work with. Native hashes on PS4 GTA V 1.00 are also the same as the 1.00 native hashes on PC, meaning the documentation on [NativeDB](http://dev-c.com/nativedb/) can be used. The majority of the code in the GTA payload is filtered to only execute once a frame (`IS_PLAYER_ONLINE` is called multiple times a frame), in order to keep things smooth. This will be more important for any drawing.

## Miscellaneous Notes
### Building
The GTA payload is automatically built and included in the browser payload, so you only need to run `make` in the main directory.
### Buttons
Button ID's for PS4 were different than last gen and PC so a simple mapping of them is included. `Button_Tpad_X` and `Button_Tpad_Y` can be used with the natives `GET_CONTROL_VALUE` and `GET_CONTROL_NORMAL` for touchpad input. `Button_Tpad` is just for if the touchpad is pressed.
### Testing
Since this injects a payload into GTA's process, multiple versions can be tested without having to resart the game. Each instance will replace the previous one, so multiple payloads cannot be run at once.


## Credits
`Specter, CTurt, qwertyoruiopz, flatz, idc, SKFU, droogie, Xerpi, bigboss, Hunger, Takezo, and Proxima` - PS4 research making all this possible

`Alexander Blade and NativeDB Contributors` - Native research making GTA V stuff relatively easy to port to PS4