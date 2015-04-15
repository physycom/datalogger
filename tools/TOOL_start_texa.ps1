$port= new-Object System.IO.Ports.SerialPort COM35,115200,None,8,one
$port.Open()
$port.WriteLine("")
Start-Sleep -m 50 
$port.WriteLine("root")
Start-Sleep -m 50
$port.WriteLine("texa987")
Start-Sleep -m 50
$port.WriteLine("SensorsTest -c -k -o stdout")
Start-Sleep -m 50
$port.Close()