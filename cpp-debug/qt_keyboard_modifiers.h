#ifndef JAN_MAP
#pragma message("ERROR: Please define 'JAN_MAP' as a local variable of choice")
static_assert(false, "ERROR: Please define 'JAN_MAP' as a local variable of choice")
#endif
JAN_MAP[Qt::NoModifier] = "NoModifier";
JAN_MAP[Qt::ShiftModifier] = "ShiftModifier";
JAN_MAP[Qt::ControlModifier] = "ControlModifier";
JAN_MAP[Qt::AltModifier] = "AltModifier";
JAN_MAP[Qt::MetaModifier] = "MetaModifier";
JAN_MAP[Qt::KeypadModifier] = "KeypadModifier";
JAN_MAP[Qt::GroupSwitchModifier] = "GroupSwitchModifier";