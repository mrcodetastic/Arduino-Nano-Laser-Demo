Write-Host "Waiting 6 seconds before playing"
Start-Sleep -Seconds 6

$port = New-Object System.IO.Ports.SerialPort
$port.PortName = "COM5"
$port.BaudRate = "115200"
$port.Parity = "None"
$port.DataBits = 8
$port.StopBits = 1
$port.ReadTimeout = 9000 # 9 seconds
$port.DtrEnable = "true"

$port.open() #opens serial connection
Start-Sleep 3 # wait 2 seconds until Arduino is ready
$port.Write("2") #writes your content to the serial connection

$PlayWav=New-Object System.Media.SoundPlayer
			
try
{
  while($myinput = $port.ReadLine())
  {
	$myinput
	
	if ($myinput.Trim() -eq "START2")
	{
			Write-Host "Starting music playback...."
			$PlayWav.SoundLocation="C:\.....\KipodBeat1.wav"
			$PlayWav.playsync()
	
	}
	else
	{
	#	Write-Host "Not Started...."
	}
  }
  
  
}

catch [TimeoutException]
{
# Error handling code here
}

finally
{
# Any cleanup code goes here
} 

$port.Close() #closes serial connection
