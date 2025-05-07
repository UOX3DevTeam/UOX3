$content = Get-Content "coreDefinitions.json" | ConvertFrom-Json

$lines = @(
  "declare global {"
)
foreach ( $types in $content.types ) {
  if ( $null -ne $types.definition ) {
    $lines += "  type $($types.Name) = $($types.definition);"
  }
  else {
    $decl = "  interface $($types.Name)"
    if ( $null -ne $types.extends ) {
      $decl += " extends " + $types.extends
    }
    $decl += " {"
    $lines += @(
      $decl,
      "  }"
    )
    if ( $null -ne $types.aka ) {
      $lines += "  type $($types.aka) = $($types.Name);"
    }
  }
}

function createFunc( $funcName, $funcParms, $funcReturn ) {
  $toAdd = "  function $funcName("
  $pCount = 0
  $pad = ""
  foreach ( $parms in $funcParms ) {
    if ( $parms.required ) {
      $qual = ""
    }
    else {
      $qual = "?"
    }
    if ( $pCount -gt 0 ) {
      $toAdd += ", "
    } else {
      $toAdd += " "
    }
    $toAdd += $parms.name + $qual + ": " + $parms.type
    $pCount++
    $pad = " "
  }
  $toAdd += $pad + "): " + $funcReturn + ";"
  return $toAdd
}

foreach ( $function in ($content.globalFuncs | Sort-Object -Property Name) ) {
  # function GetAccountCount(data: string): number;
  if ( $null -ne $function.params.sets ) {
    # Multiple definitions
    foreach( $parmSet in $function.params.sets ) {
      $lines += createFunc $function.Name $parmSet.list $function.returns
    }
  }
  else {
    # Singular definition
    $lines += createFunc $function.Name $function.params.list $function.returns
  }
}

$lines += @(
  "}",
  "",
  "// Ensure the file is treated as a module",
  "export {};"
)

$lines