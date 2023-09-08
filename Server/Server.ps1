param (
    [Parameter(Mandatory=$true)]
    [string]$hostname,

    [Parameter(Mandatory=$true)]
    [int]$port,

    [Parameter(Mandatory=$true)]
    [string]$filesDirectory
)

$url = "http://${hostname}:${port}/"
$listener = New-Object System.Net.HttpListener
$listener.Prefixes.Add($url)
$listener.Start()
Write-Host "HTTP server running on $url"

try{
    while ($listener.IsListening) {
        $context = $listener.GetContext()
        $request = $context.Request
        $response = $context.Response

        if ($context.Request.HttpMethod -eq 'GET'){
            $requestedPath = $request.Url.LocalPath
            Write-Host $requestedPath
            $filePath = Join-Path $filesDirectory $requestedPath.Substring(1)
            if (Test-Path $filePath -PathType Leaf) {
                $content = [System.IO.File]::ReadAllBytes($filePath)
                $response.ContentLength64 = $content.Length
                $response.SendChunked = $FALSE
                $response.ContentType = "text/plain"
                $response.StatusCode = 200
                $response.OutputStream.Write($content, 0, $content.Length)

                $response.OutputStream.Close()
                $response.Close()
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
}