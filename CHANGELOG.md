# Changelog

All notable changes to this project will be documented in this file.

## [2.0.0] - 2026-03-23

### Breaking Changes
- Refactored `Relay` struct:
  - Replaced `state` with `is_on` (logical state)
- `relay_init` now uses `bool initial_on` instead of `int`
- Timer system redesigned using `pending_action`
- Behavior of delayed functions (`*_after`) corrected

### Added
- `relay_toggle()` function
- Internal abstraction for relay control (`relay_apply`)
- Generic timer handler with action callback

###  Fixed
- `relay_turn_on_after()` was not turning relay ON
- `relay_turn_off_after()` inconsistent behavior
- Timer callback always turning relay OFF
- Incorrect logical vs electrical state handling

### Refactored
- Cleaner architecture separating:
  - logical state
  - hardware control
  - timer behavior
- Improved internal API design for extensibility

### Notes
- This version is NOT backward compatible with v1.x