# ofxTBB

ofxAddon that packages [Threading Building Blocks](https://www.threadingbuildingblocks.org/) for use in [openFrameworks](https://github.com/openframeworks/openFrameworks).

Nothing fancy here, the addon just includes the `tbb` library, an `addon_config.mk` to get started quickly, and a couple of examples.

## Installation

* Clone this repository in your openFrameworks `addons` folder.
* Use the OF Project Generator to generate projects with the appropriate headers and libraries included. ✌️

## Compatibility

Tested with: 
* openFrameworks 0.11.x
* Windows 10
* Visual Studio 2017

## Examples

* `example-loop` demonstrates how to use parallel for-loops, and how they compare to serial loops.
* `example-vector` demonstrates how to use concurrent vectors, and how they compare to standard `std::vector`.
