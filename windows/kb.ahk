#SingleInstance Force

; --- Doubletap '.' to replace it with an arrow for c++ ---

global lasttime := -1   ; Timer for doubletaps in ms
global timethres := 200 ; Threshold for doubletaps in ms

~.::
diff := A_TickCount - lasttime
if(diff < timethres)
{
	Send, {Backspace}{Backspace}->
}
lasttime := A_TickCount
return

; --- NUBS replacement for macros on cycle7 ---

NumpadIns::
Numpad0::
Send, <
return

NumpadEnd::
Numpad1::
Send, >
return

NumpadDown::
Numpad2::
Send, |
return

; --- Helper functions ---

getActiveWindowProcessName()
;; returns the process name of the currently active window as lower case string
{
	WinGet, pid, PID, A
	WinGet, process_name, ProcessName, ahk_pid %pid%
	StringLower, process_name, process_name
	return process_name
}

; --- Key remapping only when Visual Stuido is focussed ---
; 
; Home = build
; PgUp = run
; PgDn = stop

Home::
pname := getActiveWindowProcessName()
if(pname == "devenv.exe")
	Send, ^{b}
else 
	Send, {Home}
return

PgUp::
pname := getActiveWindowProcessName()
if(pname == "devenv.exe")
	Send, {F5}
else
	Send, {Home}
return

PgDn::
pname := getActiveWindowProcessName()
if(pname == "devenv.exe")
	Send, +{F5}
else
	Send, {Home}
return
