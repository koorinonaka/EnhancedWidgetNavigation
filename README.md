# EnhancedWidgetNavigation
EnhancedWidgetNavigation provides navigation functionality for UMG.

![demo](https://raw.githubusercontent.com/wiki/koorinonaka/EnhancedWidgetNavigation/images/overview.gif)

## Features
- Supports multiple input formats (gamepad, mouse, keyboard).
- Uses EnhancedInput to acquire input. Allows free customization of key mapping and events.
- Supports GridPanel/UniformGridPanel/HorizontalBox/VerticalBox.
- Intuitive distance based navigation and eazy setup.
- Implements WidgetNavigation connections and allows nested connections.

## Requirement
- UE 5.0 and more!

## Installation
C++ projects can be built and used for free.

Pre-built binaries are available in the [UE Marketplace](https://www.unrealengine.com/marketplace/en-US/product/1d72418860104f28b0906358434e2090).

The following 3 steps must be set in ProjectSettings.
1. EWN_EnhancedPlayerInput that implements IEWN_Interface_PlayerInputExtension to DefaultPlayerInputClass.
2. EWN_LocalPlayer that implements IEWN_Interface_LocalPlayerExtension to LocalPlayerClass.
3. DA_WidgetInputConfig to WidgetInputConfigName. You can also create your own WidgetInputConfig for default key mapping.

## Usage
See the [Tutorial](https://github.com/koorinonaka/EnhancedWidgetNavigation/wiki) and [Sample Project](https://github.com/koorinonaka/EnhancedWidgetNavigationProject).

## Note
- EnhancedInput required
- Touch input not supported.

## Author
[@koorinonaka](https://twitter.com/koorinonaka)

## License
EnhancedWidgetNavigation is under [MIT license](https://en.wikipedia.org/wiki/MIT_License).

## History

### v1.1.0
#### Modified loop processing when a key is held down.
If keys are entered consecutively, the navigation loop is ignored and executed when the key is entered again.
