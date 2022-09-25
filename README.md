# EnhancedWidgetNavigation
EnhancedWidgetNavigation provides navigation functionality for UMG.

## Features
- Supports multiple input formats (gamepad, mouse, keyboad).
- Uses EnhancedInput to acquire input. Allows free customization of key mapping and events.
- Supports GridPanel/UniformGridPanel/HorizontalBox/VerticalBox.
- Intuitive distance based navigation and eazy setup.
- Implements WidgetNavigation connections and allows nested connections.

## Requirement
- UE5.0

## Installation
C++ projects can be built and used for free.
Pre-built binaries are available in the UE Marketplace.

## Usage
See the [Tutorial](Documentation/Tutorial.md).

## Note
- Touch input not supported.
- IEWN_Interface_LocalPlayerExtension must be implemented in LocalPlayer.
	- If there are no special restrictions, EWN_LocalPlayer may be used.
- UEnhancedPlayerInput must be specified for DefaultPlayerInput in ProjectSettings.

## Author
[@koorinonaka](https://twitter.com/koorinonaka)

## License
EnhancedWidgetNavigation is under [MIT license](https://en.wikipedia.org/wiki/MIT_License).
