$port= new-Object System.IO.Ports.SerialPort COM35,115200,None,8,one
$port.Open()
$port.WriteLine("q")
Start-Sleep -m 50 
$port.Close()