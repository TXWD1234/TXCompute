# **TXCompute**
***// Copyright (c) 2026 TXCompute. Licensed under the MIT License.***

TXCompute is a device to perform mathematical calculations. It is targeting to be a more powerful calculator, that don't only rely on buttons, but can handle complicated logic.
*This project is for the hackathon event Fallout (https://fallout.hackclub.com) hosted by Hackclub (https://www.hackclub.com).*

# Project Usage

## What it does
TXCompute device has a screen and USB ports that allow user to connect a USB keyboard to it, and input to a terminal-like text editor displayed by the screen. 

The input of user will be processed by a command line parser, which can handle simple commands, as well as TXCSL code.
If user input is TXCSL code, it will be forward to the TXCSL engine, which executes the input code, and outputs the execution result.

## How to use it
**To provide power:**
- Plug in an USB-C cable connecting with power into the USB-C socket

**To connect with keyboard:**
- Plug in an USB-A cable connecting with the keyboard (for wired keyboard) / wireless receiver (for wireless keyboard) into the USB-A socket

**To boot the device:**
- Press the power button on the device until the device is booted.

**To shutdown the device:**
- Enter `exit` or `shutdown` in the terminal
  ```
  exit
  shutdown
  ```
- or Press the power button until the device is shutdowned

**To perform a TXCSL calculation:**
- Enter the expression in the terminal directly
  ```
  1 + 2 * (5 + 2)
  ```
- or With prefix `csl`
  ```
  csl 1 + 2 * (5 + 2)
  ```

## How to build it
### Preparation
#### Hardware Parts
- An ESP32-S3-DevKitC-1 Devboard
- A LCD Screen that is 480x320px sized, and supports Parallel 8080 (i80) and ILI9488
- An USB-C receptacle
- An USB-A receptacle
- A push button
- A MOSFET Transistor
- 4 NPN Transistor
- 2 capacitors: 0.1uf, 10uf
- Several resistors: 10k, 5.1k
- Several wires

#### Hardware Environment
- 3D Printer
  - Several filament
- Soldering Iron
  - Several solder

#### Software Environment
- ESP-IDF
- CMake
- Git
- TXLib
- (Optional) VSCode (With ESP_IDF plugin)
- (Optional) KiCad
- (Optional) Fusion
- (Optional) OrcaSlicer

### Procedure
#### Part 1: Shell
*Working Directory: `/Hardware/CAD`*
1. Use OrcaSlicer to process a component (an individual body) in the `Main.step` file
2. Send the result to your 3D printer
3. Wait for print to finish, and repeat for every component

#### Part 2: Software
*Working Directory: `/Software`*
1. Open the directory with VSCode
2. Connect the ESP32 Devboard to your computer
3. Run command `ESP-IDF: Build, Flash and Start a Monitor on Your Device` in VSCode:
   1. Ctrl+Shift+P open the palette
   2. Enter the command title `ESP-IDF: Build, Flash and Start a Monitor on Your Device`

#### Part 3: PCB
*Working Directory: `/Hardware/PCB`*
1. Contact a manufacturing company to fabricate the PowerButton PCB
   - Use the Gerber files in directory: `PowerButton/Gerbers`
   - Suggestion: Do this part in China

#### Part 4: Assembly
*Working Directory: `/Hardware`*
1. Place the ESP32 Devboard and the PowerButton PCB on their according foundation
   - Use screws to secure the mounting
2. Connect the components except the screen, according to the schematic: `PCB/Fallout.kicad_sch`
   1. Connect each pin of the USB-C receptacle to their respective GPIO pin
   2. Connect each pin of the USB-A receptacle to their respective GPIO pin
   3. Connect each pin of the PowerButton receptacle to their respective GPIO pin
3. Assemble the Hinge
   1. Slide body Hinge_Left onto body Hinge_Edge_Left, such that the through hole of Hinge_Left is containing the extruding cylinder of Hinge_Edge_Left
   2. Slide body Hinge_Center_Left into the other side of Hinge_Left, such that the through hole of Hinge_Left is also containing the extruding cylinder of Hinge_Center_Left
![HingeLogicPicture](https://github.com/TXWD1234/TXCompute/raw/main/docs/readme-assets/hinge-logic.png)
   3. Do the same thing with Hinge_Right, Hinge_Edge_Right and Hinge_Center_Right
   4. Slide each of the assembled hinge component onto body Screen/Body. Make sure the sunmao structure on the Hinge_Edge_* fit
   5. Insert the tenon part of the sunmao structure (body group: Hinge_Body_Connection) into the mortise in center created by the 4 overlaping rectangle holes
4. Connect the 2 main bodies
   1. Secure the hinge on the MCU/Body with screws in the indicated screw holes
   2. Run wires from the ESP32 through the hinge to the screen
5. Place the LCD Screen on it's foundation in the Screen/Body
   1. Place the LCD Screen
   2. Place the Cover (Body Cover_Left and Body Cover_Right) on top of the LCD Screen
6. 
# Project Story (Why it exists)
> *"Lower, lower, lower..."*

## My Programming Obsession
From the first day I started programming, I found myself constantly pursuing a direction involuntarily.
And that direction is **low level**.
Where I first started was already pretty low: C++ is almost everyone think the lowest.
But I am not satisfied with that.
Exploring more lower level things such as Windows API and OpenGL, while making my own library and creating data structures, considering architecture, building complex systems and engines... I am on the way toward the lowest level.

## Where Did TXCompute Came From
When I first heard about this hardware hackathon: Fallout, and the fact that I need to make a hardware project, the first thing I thought about is **calculator**.
This is the first time I tried hardware development; before I used to think that hardware are all logic gates.
And I thought using logic gates to make something would be really cool, and a really interesting challange.
In this case, a calculator would be a perfect project to do that. Implement the most basic adder, subtractor, and multiplier are exactly the tasks I was imagining.
Therefore I set my goal, and started reseaarching.

## The Design Struggle
After a bit of research, I quickly found out that there are way too many obstacles to achieve my original idea, therefore made it impossible.
The major problem is that logic gates are... kind of way too low, making them very hard to connect with other existing infrastructure.
After that realization, I was stuck in a design idea struggle. A design struggle about what I should make.
Not using logic gates just made everything feels so simple. MCUs can just perform the calculation, the entire project idea of a calculator just suddenly seems to be too easy and not impressive enough.
**I was worried about the project being too simple.**
For almost a week, I did not know what to do.
I was numbing myself by exploring hardware programming environment and new knowledge, but knowledge without usage is useless. And everytime I think about the project the only thing I felt is dilemma...
Until I finally found an solution. I do not need to make exactly the project I was planning to make before.
**If making the basic is too easy, then just expand on the basic.**
> **"Make it advanced, make it better."**

This is my conclusion. If just make a basic calculator is too easy with MCU, then make a better calculator, that has more feature, more capability, and faster.
When I realized that, I immediately had an idea:
Make a computation DSL, and an emulator for it.
Write a compiler and a virtual machine in the tight memory and performance constrained environment of embedded system is definitely hard enough... I thought.
So then, I start working.

## Back to Low Level
After 2 month of hardworking, TXCompute finally came close to the end.
During development, I found out that the project is actually not as simple as I thought.
The memory constraint really made software became a struggle. Architecture and structure design need to have awareness of the memory constraint, therefore much more complicated. I made several new data structure during development, and the entire project software design is also heavily optimized for tight memory.
At the very last, in the process of finishing up the project, I suddenly found out there is a unplanned component in the project: The PowerButton.
Brief explaination about the component: It is a hardware push button controling the power input of the device, but at the same time need to be able to be turned off by the software.
And as I developed deeper into this component, I learned the mechanics of transistors, and built logic gates (the NOT gate specificly) with it. Finally I utilized all these knowledge and created a PCB.
That's when I realized. I am back at what I was imagining at the start of the project: interact and make things with logic gates.
Additionally, even lower! I made the logic gate itself!
And that is exactly the pursuit of this project, as well as my programming career:
> *Lower, lower, and lower....*

# Project Documentation

This project consist of 4 main parts:
- The Hardware Assembly
- Fireware Infrastructure
- Terminal Engine
- TXCSL Languge Processor

And finally the `MainClass` that combines everything together.

*The purpose of this documentation is to provide an overview of the implementation, and an entry point of understanding the logic.*
*To understand the detailed logic, please read the source code, and take reference to the comments.*

## The Hardware Assembly

The TXCompute device, or the *"TXComputer"* consists of 2 parts:
- The 3D Printed Shell
- Circuit
- Hardware Parts

### Shell
*The 3D shell design took inspiration from my personal laptop: `Legion Y7000 IRX10`.*
The 3D printed shell is the hardware container containing all the wire connection from the circuit, who acts like the interface of the project for users.
The model design is highly focused on mechanical functionalities. Its implementation consists of:
- Hinge
- Circuit Foundation - support the wiring and the PCB to sustain it's position
- Several Connections
  - connect the screen to teh shell
  - connect the screen part to the hinge
  - connect the MCU part to the hinge
  - connect the front and back plate to the MCU part

*Highlight worth mentioning: The usage of sunmao in the connection.*
// add pictures <--------------------

#### Hinge Design
The hinge is designed such that wires connecting the ESP32 and the screen (the display bus) can run through.
It has 2 hollow cylinders, one containing the other. The smaller one will be containing the wires.
// add pictures <--------------------

### Circuit
There are 2 main parts for the circuit: parts connection and the power button.

#### Parts Connection
Connect the components including:
- LCD Screen
- PowerButton
- USB-A / USB-C receptacle
to the ESP32 devboard.

// add pictures <--------------------

#### PowerButton
The logic circuit that controls the power and life time of the device.
The circuit allow a push button to turn on and off the device, while when turned on, device can turn itself off as well.

// add pictures <--------------------

## Firmware
*Also known as TXESP_Infrastructure.*

There are 4 branches in the firmware infrastructure, each serve a usage in the main class:
- TextRenderer
- USBKeyboardInputHandler
- Buffer
- PowerManager

### TextRenderer
TextRenderer is the highest level interface of the rendering pipeline, that is based off of the ILI9488 driver and ESP_LCD.
The dependency tree:
```
TextRenderer
FrameComposer
ILI9488DriverPanel
ESP_LCD
```

#### `ILI9488DriverPanel`
*This Driver is made according to the data sheet: https://www.hpinfotech.ro/ILI9488.pdf*
*It's basically just a wrapper of the io system.*
ILI9488DriverPanel manage the lowest level communication between the higher level firmware and the LCD screen hardware.
It has stores the commands to the hardware, and specificly focuses on these two jobs:
- Initialization of the LCD screen;
- Write data to specific location of the LCD screen's framebuffer.

Call `ILI9488DriverPanel::init()` to initialize; Call `ILI9488DriverPanel::draw()` to write pixels.

#### `FrameComposer`
Manage the bus and io, and also wraps around the `ILI9488DriverPanel`, bringing the render process one level higher.
It takes care of the transformation of the pixel data, manage the lifetime of the bus and io provided by ESP_LCD.
`FrameComposer` is the end of talking to hardware APIs. It fully wraps the configuration and initialization, leaving only an API `draw(Coord topLeft, Coord dimension, u16* data)` for the software to perform the rendering.

#### `TextRenderer`
Manage the pixel data, consists of a simple double buffer design, and the job of unpacking the bit-packed character data.
`TextRenderer` abstract the rendering pipeline one step furthur, now the software don't need to thing about the actual pixel data anymore, instead only the character that is being drawed need to be considered.
*This class is technically a software as it does not use hardware APIs directly, but I still considered it as firmware because it is such a low level component.*

#### Font
Because of the limited storage of ESP32, the font data is stored in bitmap, bit packed as one bit is one pixel.
The font data is in the form of a C array in hex with type `u8`, in a header file, along with a `size_t` constant containing the size of the font data, all in `constexpr const`.
The header file will be included by the user of the font.

The font file used in this project is generated by a 2 stage pipeline:
- convert ttf file into png file via a python script
- read the png image file and pack the color into bits via a C++ program

### USBKeyboardInputHandler
USBKeyboardInputHandler is the highest level interface of the keyboard input pipeline. It is based on HID and ESP's USB framework.
The dependency tree:
```
USBKeyboardInputHandler
USBFramework
ESP_USB; ESP_HID
```

#### `USBFramework`
Simplifies the pipeline of USB Host.
`USBFramework` provide a set of simplified init / uninit APIs to manage the driver and interface APIs.
It is simply just a wrapper around ESP_USB's API.

#### `USBKeyboardInputHandler`
*This class is made according to the data sheet: https://usb.org/sites/default/files/hut1_5.pdf*
*USBFramework is a generalized USB framework that work for any type of usb connection, while `USBKeyboardInputHandler` is made specificly for HID keyboard input.*
`USBKeyboardInputHandler` provide the interface and driver callback to the `USBFramework` to receive keyboard input.
After received the raw keyboard input report, `USBKeyboardInputHandler` also map the raw data into a custom key code set.
It also manages the pressing state of each key, tracking the state of each key pressed.
At the end, every time an report is updated, an user callback provided by the user of this class will be called with the parameter of the information of this key press event.
The overall design took inspiration and reference from GLFW 3. *Credit: `https://www.glfw.org`.*
`USBKeyboardInputHandler` is also a global state machine, adopted the design of *"pure static class"* from `USBFramework`.

#### Driver
Contains the implementation of `driverCallback_impl` and `interfaceCallback_impl`, as the lifetime manager of the process.
Also contain the essential enum types such as `Key`, `Action` and `Mod`, as well as `keyCodeTable`, which is the lookup table describing the translation between HID code and a uniformed code set that's corresponding to ASCII.

#### Logic
Handles:
- The decode process of the Modifiers
- The Action state
  - Release
  - Repeat
  - Press
- Current pressing key
  - Key cache
The core entry function is `handleReport_impl()`, which is being called by the `interfaceCallback` when an keyboard input event happens.

#### Interface
The user is only exposed to 3 functions:
- `init()`
	Initialize the entire system, including the `USBFramework`. *(it's actually just initing `USBFramework` but.. it's just for encapsulation)*
- `uninit()`
	Uninitialize the entire system, including the `USBFramework`.
- `setCallback(InputCallbackFunc_t func)`
	Set the event callback when a keyboard input event happens.

**`InputCallbackFunc_t`**:
```cpp
using USBKeyboardInputHandler::InputCallbackFunc_t = void (*)(
	USBKeyboardInputHandler::Key,
	USBKeyboardInputHandler::Action,
	USBKeyboardInputHandler::Mod); // key, action, mod
```

### Buffer
`buffer.hpp` is an wrapper around ESP_HEAP_CAPS, that provide a generalized API `allocate` and `free` with template to manage memory.
It also provide the memory managing wrapper for a few data structure from TXLib that are used in the project, including:
- `Buffer_GrowArray`
- `Buffer_CircularQueue`
- `Buffer_RingBuffer`

And a plain memory container `Buffer`.

### PowerManager
`PowerManager` is a small class that wraps the ESP_GPIO APIs, especially the intiialization setup, and gave the user a fast way to operate an pin's charge (voltage).
It is designed targeting to serve the power button functionality of the project.

## Terminal Engine

The Terminal Engine is a text render and input processor, that performs resemble a terminal.
It is designed targeting specificly for memory constrained environment, such as the embedded system environment in ESP32, following the KISS rule (Keep It Simple, Stupid).

---
Logics are separated into different classes, each with their own logic to consider: *"Do one thing and do it well"*.
Then the overall class `TemrinalEngine` combines all of the sub-logic classes into a set of uniform terminal APIs.

With that architecture in mind, there are 3 sub classes:
- `StringPool`
- `InputLine`
- `InputHandler`
And an overall class: `TerminalEngine`, which consist of 3 logical parts:
- Line Storage
- Input Pipeline
- Render Pipeline

### `StringPool`
The `StringPool` is a specialized implementation of an ring buffer.
There are 2 more features that it achieved beyond the classic ring buffer:
- Data entry grouping
- Automatic eviction

#### Data Layout
`m_data`: the buffer that stores the actual data
`m_meta`: the buffer that stores the meta data of each logical data group (object): where it start and how big is it.

#### Terminology:
> - Evict:
>   The operation of deleting old data to make space for incoming new data when buffer is full. The deleted data is always the oldest data, and one evition might delete multiple data entry depending on the condition
> - Object / Meta / Range:
>   These terms refers to an entry in `m_meta`, which is corresponding to a range of data in `m_data`. Conceptually simillar to a partition. In the case of the StringPool, it is a string.

#### Data Entry Grouping
Each object consist of one or more entry in `m_data`, and they are grouped together by logic. They are added to the buffer at the same time, and deleted together when eviction happens.
Because the size of each object is variadic, some object might be much bigger then others, or they might all have size of 1.

#### Automatic Eviction
The total memory usage of `StringPool` is fixed at construction. There are no runtime allocation after initialization.
Therefore, the size of `m_meta` and `m_data` are both fixed. Because the data entry grouping mechanism, especially because the variadic size, both `m_meta` and `m_data` have a potential of becomeing full.
Hence, there are 2 condition to invoke an eviction: `m_meta` is full or `m_data` is full.
In the case of MetaEviction (`m_meta` is full), the oldest and only the oldest object need to be deleted.
In the case of MemoryEviction (`m_data` is full), a range of objects need to be deleted, since the incoming object might have a size that is bigger then the oldest object.
Both evictions have a potential of leaving interspace (memory gap) between the oldest object and the second oldest object, since the incoming object's size might be smaller then size of the evicted object(s).

### `InputLine`
`InputLine` is a classic "gap buffer" inputer design, that provide APIs for the `TerminalEngine` to perform standard input actions (such as moving cursor, input characters, and deleting characters). It also provide a set of APIs for the `TerminalEngine` to query it's internal state and get the final inputed string when user finishes inputing.

#### Logic
There are 2 buffers in `InputLine`, each one has the capacity of the maximum length of one line in the terminal (which is set by `TerminalEngine`).
They are the left and right buffers. Intuitively, they stores characters to the left / right of the cursor.
Whenever a character is inputed, the char value is pushed to the back of the left buffer.
Whenever the cursor move left, the back element of left buffer (`left.back()`) is pushed to the back of right buffer. And vice versa for moving right.
The layout can be displayed like this:
```
Normal buffer:
[<<buf<<]
^       ^
front   back

InputLine's Gap Buffer:
[<<left<<]     |       [>>right>>]
^        ^     ^       ^         ^
front    back  cursor  back      front
```

### `InputHandler`
`inputHandler` is a wrapper of `tx::esp::USBKeyboardInputHandler` from TXESP_Infrastructure.
It convert the event callback style input into flattened buffer input, which can be easily parsed by `TerminalEngine`.

#### Reason
`tx::esp::USBKeyboardInputHandler` has a callback based input system, and that's not ideal for complex logic, because the callback is happened in a system level loop, which staling might cause problem.
Therefore, `InputHandler` is created. It provide `tx::esp::USBKeyboardInputHandler` a callback, which will push input event entries to a buffer provided by the `TerminalEngine`. It flattens the input stream, thereby the Terminal Engine can perform complex logic in async while not staling the system driver.

#### Logic
The buffer that holds the flattened input event data is the most complicated part of this sub-system.
Essentially, it is a ring buffer, that wraps around to the front when reached the back.
`TerminalEngine` and `InputHandler` share a same object of `tx::CircularQueue`. `InputHandler` push new element to the buffer when a new event happens; `TerminalEngine` read and delete the entries it read every update, creating a perfect producer-consumer cycle.
As long as `TemrinalEngine` don't stale while InputHandler is not `stale()`ed, the ring buffer will not collide.

### Line Storage
*Also know as LineBuffer.*

Stores the information of each line. Separated by `'\n'`.
It is a CircularQueue of type `Line_impl`.

#### Line_impl
A line could be either an entry in the `StringPool` or a string that's outside of the `TerminalEngine` (not owned by `TerminalEngine`).
There is only one u32 value in this struct. It could either be a `u32` id in the `StringPool`, or a `const char*` pointer since on 32 bit architecture a pointer is 4 bytes.
The last bit is used as the boolean flag to identify whether it's an id *(1)* or a pointer *(0)*.
*All of these just to save memory.*

#### `lineBufferEvict_impl()`
The line buffer is obviously fixed sized and will obviously fill up. If line buffer is filled up, but non of `StringPool`'s buffer is filled up, the eviction of line buffer will trigger. *Which is also know as LineBufferEviction. (following the naming convension of `StringPool`)*
It will first pop the last element of the line buffer, then try to communicate to `StringPool`, and delete the stored string that is corresponding to this deleting line entry, **if** this line entry is a id type.
*So in conclusion, this entire line buffer structure is a 3 way eviction system.*

#### `handleDeleteEvent_impl()`
This function is the callback function of `StringPool`. It handles the job of syncing the state of `StringPool` with lineBuffer.
Whenever a string (aka an object in StringPool terminology) is deleted, the corresponding line must be deleted as well.
Therefore, `lineBufferEvict_impl()` is called. And since meanwhile a line could also be an external string that don't belong to `StringPool`, a string pool eviction might invoke the deletion of multiple lines, since deletion must happen in chronological order, also since the line from `StringPool` must be delete, every line before the first `StringPool` line must be deleted as well.

#### The double lock between Line Buffer Eviction and StringPool Eviction
Because LineBuffer and StringPool are both ring buffer design, they all have possibility to trigger an eviction.
And the eviction need to sync with each other to prevent currupted data.
Since `handleDeleteEvent_impl()` calls `lineBufferEvict_impl()`, and `lineBufferEvict_impl()` also indirectly calls `handleDeleteEvent_impl()` by deleteing object from `StringPool`, a cycle is created. If not interrupted, an infinite recursive loop will stale everything.
Therefore in `TerminalEngine` there is a `m_state`, who contains 2 boolean flags, which are used for the 2 eviction to declare their state.
If one eviction is happening, and it calles the other eviction, the other eviction will not trigger the eviction back again: the infinite recursion loop is prevented by an if check.

### Input Pipeline

#### `TerminalEngine::poll()`
This is a public API called by the user - in the program sense - the higher layer of the TerminalEngine, which is the TXCompute main class.
`poll()` is the main entry of the async processing pipeline. When this function is called, the caller should expect the possibility of the input callback be called, and old references to the user input string to be invalidated (as they are stored in the `StringPool` of the TerminalEngine, which might delete some string entries to make free space for new spaces).
If `poll()` is not called for a long period of time, an exception might happen in the InputHandler, because the input buffer of the Terminal Engine is filled up. To prevent this, a pair of `stale()` / `unstale()` API is provided in `InputHandler` to ignore the input from source. ***You only need to manually call them if stale is planed to happen during input session.***
`poll()` copies the current input entries, and pop them from the buffer.
After that, the lock is released, and the processing of input begins.
`handleInputEvent_impl()` is called for every input event. Each input event is one key pressing, repeative pressing down, or releasing.

#### `TerminalEngine::handleInputEvent_impl()`
Process one key input event. It could be either pressing, repeative pressing down, or releasing.
If releasing, the event will be ignored.
Otherwise, if the key is printable, it will go through modifier modifications beforehand (such as CapsLock and Shift), and then be pushed into `InputLine` as a `char`.
If the key is non-printable, aka a function key, it will be processed in a `swtich`, determinding what action it is doing.
In this stage, all operations are methods of `InputLine`, except `Enter`, which triggers `handleNewLine_impl()`.

#### `TerminalEngine::handleNewLine_impl`
First it extracts the input string from `InputLine` and store it in the StringPool.
Then line buffer will be updated, potentialy triggering eviction: `lineBufferEvict_impl()`.
Finally, the input callback will be called with the input string stored in StringPool.

#### Sessions
There are 2 sessions in the terminal engine:
**Input Session:**
Start when `startInputSession()` is called.
End when terminal user presses enter or `endInputSession()` is called.
During input session, all `print()` will be ignored (including `printStatic()`), and inputHandler will be `unstale()`ed.

Default session. Start when TerminalEngine object is initiated, or input session had ended.
End when input session had started or TerminalEngine object is destroied.
During output session, all terminal user input will be ignored, and `print()` will be allowed.

### Render Pipeline
The render pipeline of the TerminalEngine consists of 4 parts:
- lhCache (line height cache) book keeping
- render state and cursor logic
- render events
- drawing characters and lines

#### Terminology:
| Word | Explaination |
|-|-|
Logical Line / Line_impl | the entry in lineBuffer, that represents one line without wrapping
Screen Line / Render Line | the actual displaying line of text on the screen, consider wrapping

#### lhCache
Front is top (of the screen); Back is bottom (of the screen)
Stores the height (screen line count) of every logical line that are visible on the screen.
It's index is linearly parallel to lineBuffer, meaning that the second entry in lhCache is corresponding to:
```cpp
m_lineBuffer[m_lhCacheState.topLine + 1]
```
lhCache represent the current display state, which serve for the scrolling logic.

##### `m_lhCacheState`
```cpp
struct LhCacheState_impl {
	/**
	 * topLine is index of the first Line_impl that is visible on
	 * the screen. It is also the index of Line_impl that the front() of
	 * lhCache is according to.
	 * topLineBegin is the index of the screen line that is the
	 * actual first line on the screen. it is the index within the range
	 * of the topLine object
	 */
	u32 topLine, topLineBegin, bottomLineEnd;
	/**
	 * the total amount of screen lines of all the logical lines that are
	 * currently visible on screen, including the invisible screen lines of
	 * the top and bottom logical line
	 */
	u32 screenLineCount;
} m_lhCacheState;
```

It is necessary to update screenLineCount whenever a push or pop to the lhCache happened.

#### RenderState
```cpp
struct RenderState_impl {
	/**
	 * cursorPos always points to the end of left buffer (exclusively), and
	 * points to the begin of right buffer (inclusively).
	 * Speaking in plain english: cursorPos is the first character of right
	 * buffer, which is also the character behind the last character of
	 * left buffer
	 */
	Coord cursorPos;
} m_renderState;
```
*Yeah an entire struct just to store one Coord. It's all for good practice trust.*

Cursor will wrap to the begining if moved horizontally to the end.

#### Render events
Event functions that have their name directly matching with the keyboard action event. *encapsulating inside encapsulation.*
The `handleInputEvent_impl` will call these event functions, which will perform their logic and update the graphics.

#### Drawing
Consist of simple grid -> pixel converter, line drawer and full redraw function

## TXCSL

TXCSL (TX Computational Scripting Language) is a bash-like, virtual machine based scripting language, which is capable of computing basic expressions.

*TXCSL is targeting to be a DSL for computation. It is an enhancement of regular calculators, but not a turing complete programming language.*

TXCSL operates in the pipeline described below:
> **Every line of code will be *compiled* into instructions, then executed.**
Even the result appears instantly, it had been through the process of compilation.

### Interface Data Structs

#### `Expression`
```cpp
struct Expression {
	std::span<Command> commandBuffer; // the commands / instructions
	std::span<num> constantBuffer; // constant values that are involved in the expression
	std::span<num> variableBuffer; // variable values that are involved in the expression
	tx::u32 registerCount; // the required registers when evaluating
	tx::u32 commandCount; // the count of commands / instructions
};
```
`Expression` is the **compiled binary instruction code** after compilation.
It will provide everything the `Evaluator` requires to calculate an output.
An object of this struct should be created before compilation, and all the buffers should be allocated with their respective size:
| Buffer Name      | Element Count |
| ---------------- | ------------- |
| `commandBuffer`  | 256           |
| `constantBuffer` | 64            |
| `variableBuffer` | 64            |

During compilation, the `commandBuffer` and `constantBuffer` will be populated.
But the `variableBuffer` will not be touched. The user need to provide values of the variables themself.

#### `CompileResult`
```cpp
struct CompileResult {
	tx::u32 commandCount;
	tx::u32 constantCount;
	tx::u32 variableCount;
	std::vector<std::string_view> varibaleNames; // for the caller of compile() to fill in the variabe buffer
};
```

#### How to populate `variableBuffer`

#### Note for then span design
You can have a big buffer that's bigger then the buffer's required size for each buffer.
Then you can store multiple compiled expressions. (The function design)
And that's what the `*Count`s in `CompileResult` is for.

### Compilation

*There is the where the major processing logic of TXCSL is.*
Every line of code in TXCSL is treated as expression, and will be processed by the `tx::csl::Compiler`.

#### The design pattern of the compilation
Each stage will have it's own class.
That class will be a temporary object, which will outputs a certain result of the stage.
The purpose of the class is to maintain it's own internal state (because they are all state machine design), and the temporary memory they allocate.

#### Compilation pipeline
The compilation pipeline flow is:
1. Raw string
2. BracketParser - compose bracket structure
3. Tokenlizer    - tokenlize raw string. identify and convert value
4. Compiler      - transform tokens into instructions, flatten the operation tree
The process of nested expressions will be lazy, meaning that it will not be processed until necessary. 
When the Compiler encounters an unexpanded expression, it will call the Tokenlizer again, and tokenlize the unexpanded expression.

*See detailed logic explaination in inlined comments of source files.*

#### APIs

`static CompileResult compile(std::string_view source, Expression& result)`

Compile the raw source string `source` into instruction code in `result`.
**Note:** the buffers in `result` should be allocated before calling this function. the buffers listed below will then be populated in the function.