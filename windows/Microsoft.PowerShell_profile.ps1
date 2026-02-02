# set regular console colors
#[console]::backgroundcolor = "darkmagenta"
#[console]::foregroundcolor = "darkyellow"

# showcase system colors
#$colors = [enum]::GetValues([System.ConsoleColor])
#Foreach ($bgcolor in $colors){
#    Foreach ($fgcolor in $colors) { Write-Host "$fgcolor|"  -ForegroundColor $fgcolor -BackgroundColor $bgcolor -NoNewLine }
#    Write-Host " on $bgcolor"
#}

# set special colors

$p = $host.privatedata

# default colors - output
#$p.ErrorForegroundColor    = "Red"
#$p.ErrorBackgroundColor    = "Black"
#$p.WarningForegroundColor  = "Yellow"
#$p.WarningBackgroundColor  = "Black"
#$p.DebugForegroundColor    = "Yellow"
#$p.DebugBackgroundColor    = "Black"
#$p.VerboseForegroundColor  = "Yellow"
#$p.VerboseBackgroundColor  = "Black"
#$p.ProgressForegroundColor = "Yellow"
#$p.ProgressBackgroundColor = "DarkCyan"

#Set-PSReadLineOption -Colors @{
#  Command            = 'Yellow'
#  Number             = 'White'
#  Member             = 'White'
#  Operator           = 'DarkGray'
#  Type               = 'DarkGray'
#  Variable           = 'Green'
#  Parameter          = 'DarkGray'
#  ContinuationPrompt = 'Black'
#  Default            = 'Black' # ('DefaultToken')
#}

# custom colors - output
$p.ErrorForegroundColor    = "DarkRed"
#$p.ErrorBackgroundColor    = "White"
$p.WarningForegroundColor  = "DarkYellow"
#$p.WarningBackgroundColor  = "White"
$p.DebugForegroundColor    = "DarkYellow"
#$p.DebugBackgroundColor    = "White"
$p.VerboseForegroundColor  = "DarkYellow"
#$p.VerboseBackgroundColor  = "White"
$p.ProgressForegroundColor = "DarkYellow"
$p.ProgressBackgroundColor = "DarkCyan"

# custom colors - input

if( $PSVersionTable.PSVersion.Major -eq 7 ) {
	# powershell 7
	Set-PSReadLineOption -Colors @{
		Command            = 'DarkMagenta'
		Number             = 'DarkBlue'
		Member             = 'DarkBlue'
		Operator           = 'DarkBlue'
		Type               = 'DarkGray'
		Variable           = 'DarkGreen'
		Parameter          = 'DarkGray'
		ContinuationPrompt = "#cccccc"
		InlinePrediction   = "#cccccc"
		Default            = "#202020"
	}
	# MARK JAN: causes issues with znx commit hooks scrips
	#Write-Host "Loaded profile for Powershell 7"
} else {
	# powershell 5
	Set-PSReadLineOption -Colors @{
		Command            = 'Magenta'
		Number             = 'DarkMagenta'
		Member             = 'DarkMagenta'
		Operator           = 'DarkMagenta'
		Type               = 'DarkGray'
		Variable           = 'DarkGreen'
		Parameter          = 'DarkGray'
		ContinuationPrompt = 'Gray'
		Default            = 'DarkGray'
	}
	# MARK JAN: causes issues with znx commit hooks scrips
	#Write-Host "Loaded profile for Powershell 5"
}

# clear screen
#clear-host
