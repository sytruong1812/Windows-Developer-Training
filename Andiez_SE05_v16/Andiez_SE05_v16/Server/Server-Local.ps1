param (
    [Parameter(Mandatory=$true)]
    [string]$hostname,

    [Parameter(Mandatory=$true)]
    [int]$port,

    [Parameter(Mandatory=$true)]
    [string]$filesDirectory
)

function Task {
    param (
        [ScriptBlock]$ScriptBlock
    )

    $Runspace = [RunspaceFactory]::CreateRunspace()
    $Runspace.ApartmentState = "MTA"
    $Runspace.Open()

    $PowerShell = [PowerShell]::Create()
    $PowerShell.Runspace = $Runspace
    $PowerShell.AddScript($ScriptBlock).AddArgument($response).AddArgument($filePath)

    $Job = $PowerShell.BeginInvoke()

    return [PSCustomObject]@{
        PowerShell = $PowerShell
        Runspace = $Runspace
        Job = $Job
    }
}

Write-Host "1. Initializing variables and objects..."
$url = "http://${hostname}:${port}/"

$listener = New-Object System.Net.HttpListener
$listener.Prefixes.Add($url)

$listener.Start()
Write-Host "2. HTTP server running on $url"

try {
    while ($listener.IsListening) {
        Write-Host "----------------------------------"
        Write-Host "3. Waiting for incoming request..."
        $context = $listener.GetContext()
        $request = $context.Request
        $response = $context.Response
        if ($context.Request.HttpMethod -eq 'GET') {
            Write-Host "4. Received GET request."
            $requestedPath = $request.Url.LocalPath
            Write-Host "5. Path: $requestedPath"
            $filePath = Join-Path $filesDirectory $requestedPath.Substring(1)
            if (Test-Path $filePath -PathType Leaf) {
                Write-Host "6. File found: $filePath"
                $taskScript = {
                    param ($response, $filePath)
                    try {
                        Write-Host "7. Processing file: $filePath"
                        $bufferSize = 1024;  # Kích thước chunk
                        $fileStream = [System.IO.File]::OpenRead($filePath)
                        $buffer = New-Object byte[] $bufferSize
                        $bytesRead = 0

                        $response.ContentLength64 = [long]$fileStream.Length
                        $response.SendChunked = $FALSE
                        # $response.ContentType = "text/plain"                  
                        $response.ContentType = "application/octet-stream"
                        $response.StatusCode = 200

                        while (($bytesRead = $fileStream.Read($buffer, 0, $bufferSize)) -gt 0) {
                            $response.OutputStream.Write($buffer, 0, $bytesRead)
                        }

                    } catch {
                        Write-Host "8. Error occurred: $_"
                    } finally {
                        $response.OutputStream.Close()
                        $response.Close()
                        $fileStream.Close()
                        Write-Host "9. Response closed, fileStream closed!"
                    }
                }
                # Tạo và thực hiện công việc bất đồng bộ trong Runspace
                $RunspaceTask = Task -ScriptBlock $taskScript -ArgumentList $response, $filePath, $response


            } else {
                Write-Host "10. File not found: $filePath"
                $response.StatusCode = 404
                $response.Close()
            }
        }
    }
}
finally {
    $listener.Stop()
    $listener.Close()
    Write-Host "11. Listener stopped and closed."
    $RunspacePool.Close()
    $RunspacePool.Dispose()
    Write-Host "12. RunspacePool closed and disposed."
}
