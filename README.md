# EnhancedWidgetNavigation
EnhancedWidgetNavigation provides navigation functionality for UMG.

![demo](https://raw.githubusercontent.com/wiki/koorinonaka/EnhancedWidgetNavigation/images/overview.gif)

## Features
- Supports multiple input formats (gamepad, mouse, keyboad).
- Uses EnhancedInput to acquire input. Allows free customization of key mapping and events.
- Supports GridPanel/UniformGridPanel/HorizontalBox/VerticalBox.
- Intuitive distance based navigation and eazy setup.
- Implements WidgetNavigation connections and allows nested connections.

## Requirement
- UE 5.0

## Installation
C++ projects can be built and used for free.

Pre-built binaries are available in the UE Marketplace.

The following three items must be set in ProjectSettings.
1. UEnhancedPlayerInput to DefaultPlayerInputClass.
2. LocalPlayer that implements IEWN_Interface_LocalPlayerExtension to LocalPlayerClass. If there are no special restrictions, EWN_LocalPlayer may be used.
3. DA_WidgetInputConfig to WidgetInputConfigName. You can also create your own WidgetInputConfig for default key mapping.

## Usage
See the [Tutorial](https://github.com/koorinonaka/EnhancedWidgetNavigation/wiki).

## Note
- Touch input not supported.

## Author
[@koorinonaka](https://twitter.com/koorinonaka)

## License
EnhancedWidgetNavigation is under [MIT license](https://en.wikipedia.org/wiki/MIT_License).
