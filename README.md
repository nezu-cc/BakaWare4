# BakaWare4

Counter-Strike 2 cheat

## Features

- ESP
  - Box
  - Name
  - Health
  - Skeleton
- Misc
  - bunnyhop

**THIS CHEAT IS STILL WIP, MORE TO COME SOON:tm:**

## Technical "rules"

for those that care

- No modifying any read only memory sections
- No hooks on globally exported interfaces
- No SEH/VEH (except for the one that is required to disable DEP when mapped using perfect injector)
- No TLS
- No persistent threads
- Must load and function correctly when mapped above 0x7FFFFFFEFFFF ;)

NOTE: the above only applies to release builds, anything is fair game  
in debug builds since they are only designed to be loaded in `-insecure`

## Building

This project uses [CMake](https://cmake.org/) to generate build files and [Visual Studio 17 2022](https://visualstudio.microsoft.com/) to compile it.

When cloning the repository, make sure to clone it with the `--recursive` flag to also clone the vcpkg submodule.

### Example
From the `Developer PowerShell for VS 2022` while in the project directory run:

```powershell
# list configure presets
cmake --list-presets
# configure the project using the "x64-windows" preset
# NOTE: this will also install the dependencies using vcpkg
cmake --preset=x64-windows
# list build presets
cmake --build --list-presets
# build the project using the "Release" preset
cmake --build --preset=Release
```

Or just open it in your favorite IDE (that supports CMake and CMakePresets) and build it from there.

### Dependencies

- [Dear ImGui](https://github.com/ocornut/imgui) (included using vcpkg)
- [nlohmann JSON](https://github.com/nlohmann/json) (included using vcpkg)
- [ThePerfectInjector](https://github.com/can1357/ThePerfectInjector) (included directly in the source tree)

## Contributing

Fell free to improve it and make a pull request but there is no
guarantee that I will like/agree with it and merge it.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details

## Q&A

- Q: Is this cheat undetected?  
A: Idk, you tell me. **I'm not responsible for any bans you get!**

- Q: What version of the game does it work on?  
A: It's designed to work on the latest steam version, I do not support cracked/older versions.  
**THIS CHEAT REQUIRES THE STEAM OVERLAY TO BE ON FOR RENDERING**  
**THE VULKAN RENDERER IS NOT SUPPORTED YET**

- Q: What is this?  
A: This is a forth instalment in my counter-strike cheat making adventure.

- Q: Why that name?  
A: Idk, I'm bad at naming things and it was a long time ago when this name was chosen.

- Q: What is the injector included?  
A: It's (ThePerfectInjector)[https://github.com/can1357/ThePerfectInjector]. It maps kernel space memory into
usermode by manipulating page tables. Windows will actively refuse to interact with memory addresses above
0x7FFFFFFEFFFF through their APIs making it basically invisible to external usermode scanners. These pages
are still perfectly valid though and the CPU will happily execute them. That's not to say it's fully undetectable.
We register a VEH to nerf DEP that will sometimes get mad at us and think that the RWX page is in fact not executable.
We just catch the exception and tell it to just continue execution. Another detection vector are of course the hooks,
and while we try to stay stealthy with them, they still need to exist. Anything running directly inside cs can read
our memory just fine, this injector only hides us from windows's virtual memory APIs.

- Q: Why not just use X?  
A: Nothing out there is perfect, and neither is this. This is a cheat designed to be perfect for **ME**.  
If you find it useful that's great. Releasing it because why not, I'm not going to sell it anyway.

- Q: What is the base?  
A: This cheat was heavily inspired by (lstrsrt/csgo_internal_base)[https://github.com/lstrsrt/csgo_internal_base] but
ported to CS2 and with some slight changes. Over time the layout will probably drift further and further away from it though.