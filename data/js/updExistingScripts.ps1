$content = Get-Content "coreDefinitions.json" | ConvertFrom-Json

$jsDocMatch = '/** @type {'

$content.events | ForEach-Object {
  # Let's construct our JSDoc and regex match
  # "name": "inRange",
  # "cppName": "InRange",
  # "params": {
  #   "num": 2,
  #   "list": [
  #     { "name": "srcObj",     "type": "BaseObject", "required": true },
  #     { "name": "objInRange", "type": "BaseObject", "required": true }
  #   ]
  # },
  # "returns": "void"
  $newRegex = "function $($_.name)\("
  if( $_.name -eq "onCallback" ) {
    $newRegex = "function $($_.name)\d+\("
  }
  # /** @type { ( srcObj: BaseObject, objInRange: BaseObject ) => void } */
  $jsDoc = '/** @type { ('
  $pLen = $_.params.list.Length
  if( $pLen -gt 0 ) {
    for( $i = 0; $i -lt $pLen; $i++ ) {
      $p = $_.params.list[$i]
      $jsDoc += " $($p.name): $($p.type)"
      if( $i -ne ($pLen - 1) ) {
        $jsDoc += ','
      }
    }
  }
  $jsDoc += " ) => $($_.returns) } */"
  $_ | Add-Member -MemberType NoteProperty -Name "Regex" -Value $newRegex
  $_ | Add-Member -MemberType NoteProperty -Name "JSDoc" -Value $jsDoc
}

$possibles = $content.events | Select-Object -Property name, Regex, JSDoc

# Now add the extra e.g. command registration
$possibles += @( [PSCustomObject]@{ name = "Command Registration"; Regex = "function command_.+\("; JSDoc = '/** @type { ( socket: Socket, cmdString: string ) => void } */'  } )

$jsFiles = Get-ChildItem -Path $PSScriptRoot -Recurse -Include *.js
foreach( $js in $jsFiles ) {
  Write-Output "Processing $($js.FullName)"
  $lines = Get-Content $js
  Write-Output " .. Found $($lines.Length) lines"
  $anyChange = $false
  $inserts = @()
  for( $line = 0 ; $line -lt $lines.Length; $line++ ) {
    foreach( $possible in $possibles ) {
      #Write-Output "Checking $($possible.name)"
      if( $lines[$line] -match $possible.Regex ) {
        # Got a match ... did the previous line look like JSDoc?
        # Because if so, we may have to replace it
        if( $line -gt 0 -and $lines[$line - 1].StartsWith( $jsDocMatch ) ) {
          # OK, Javadoc to replace
          Write-Output "Updating JSDoc for $($possible.name) at $($line - 1)"
          $lines[$line - 1] = $possible.JSDoc
        } else {
          # We have to insert a line, now ...
          $inserts += [PSCustomObject]@{ Index = $line - 1; Value = $possible.JSDoc }
          Write-Output "Inserting JSDoc for $($possible.name) at $($line - 1)"
        }
        $anyChange = $true
      }
    }
  }
  if( $anyChange -eq $true ) {
    if( $inserts.Length -eq 0 ) {
      Write-Output "Replacing contents with JSDoc updates"
      # Only replacements of existing JSDoc, not additions
      $lines | Set-Content $js
    } else {
      Write-Output "Adding new content now"
      $newContent = @()
      $idx = 0
      for( $line = 0 ; $line -lt $lines.Length; $line++ ) {
        $newContent += $lines[$line]
        if( $line -eq $inserts[$idx].Index ) {
          $newContent += @( $inserts[$idx].Value )
          $idx++
        }
      }
      $newContent | Set-Content $js
    }
  }
}