$scriptPath = Split-Path -Parent -Path $MyInvocation.MyCommand.Definition
$parentPath = (get-item $scriptPath).parent.FullName

$READER = $parentPath + "\Release\serial_reader.exe"
$VIEWER = $parentPath + "\Release\serial_viewer.exe"

$META_PARAM = "-t 5 -b 115200 -p COM27"
$TEXA_PARAM = "-t 3 -b 115200 -p COM21"
$OCTO_PARAM = "-t 7 -b 115200 -p COM17"
$MM_2_PARAM = "-t 9 -b 115200 -p COM20"

Start-Process $VIEWER

Start-Sleep -m 500 # wait for 500 milliseconds

Start-Process $READER $META_PARAM
#Start-Process $READER $TEXA_PARAM
Start-Process $READER $OCTO_PARAM
Start-Process $READER $MM_2_PARAM
