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
    $PowerShell.AddScript($ScriptBlock).AddAgrument($response).AddArgument($filePath)

    $Job = $PowerShell.BeginInvoke()

    return [PSCustomObject]@{
        PowerShell = $PowerShell
        Runspace = $Runspace
        Job = $Job
    }
}

$url = "http://${hostname}:${port}/"

$listener = New-Object System.Net.HttpListener
$listener.Prefixes.Add($url)


$RunspacePool = [RunspaceFactory]::CreateRunspacePool(1, 5)
$RunspacePool.Open()

$listener.Start()
Write-Host "HTTP server running on $url"

try {
    while ($listener.IsListening) {
        $context = $listener.GetContext()
        $request = $context.Request
        $response = $context.Response
        if ($context.Request.HttpMethod -eq 'GET') {
            $requestedPath = $request.Url.LocalPath
            Write-Host $requestedPath
            $filePath = Join-Path $filesDirectory $requestedPath.Substring(1)
            if (Test-Path $filePath -PathType Leaf) {
                $taskScript = {
                    param ($response, $filePath)
                    try {
                        $content = [System.IO.File]::ReadAllBytes($filePath)
                        $response.ContentLength64 = $content.Length
                        $response.SendChunked = $FALSE
                        $response.ContentType = "text/plain"
                        $response.StatusCode = 200
                        $response.OutputStream.Write($content, 0, $content.Length)
                    } catch {
                        Write-Host "Error occurred: $_"
                    } finally {
                        $response.OutputStream.Close()
                        $response.Close()
                    }
                }
                # Tạo và thực hiện công việc bất đồng bộ trong Runspace
                $RunspaceTask = Task -ScriptBlock $taskScript -ArgumentList $response, $filePath

            } else {
                $response.StatusCode = 404
                $response.Close()
            }
        }
    }
}
finally {
    $listener.Stop()
    $listener.Close()
    $RunspacePool.Close()
    $RunspacePool.Dispose()
}
