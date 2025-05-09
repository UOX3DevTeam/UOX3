$content = Get-Content "coreDefinitions.json" | ConvertFrom-Json

$lines = @(
  "declare global {"
)
function createTypeProps( $type ) {
  $lines = @()

  $maxLabelLength = ($type.properties | ForEach-Object { $_.name.Length } | Measure-Object -Maximum).Maximum
  foreach ( $prop in ($type.properties | Sort-Object -Property name) ) {
    if ( $null -eq $prop.unused -or ( $null -ne $prop.unused -and $prop.unused -eq $false ) ) {
      $toAdd = "    "
      if ( $null -ne $prop.readOnly -and $prop.readOnly -eq $true ) {
        $toAdd += "readonly  "
      }
      elseif ( $null -ne $prop.writeOnly -and $prop.writeOnly -eq $true ) {
        $toAdd += "writeonly "
      }
      else {
        $toAdd += "          "
      }
      $toAdd += $prop.name + ": "
      $toAdd = $toAdd.PadRight( $maxLabelLength + 16 ) # 9 for readonly, 4 for indent, 2 for colon
      $toAdd += $prop.type + ";"
      $lines += $toAdd
    }
  }
  return $lines
}
function createTypeEnums( $values ) {
  $lines = @()
  $maxLabelLength = ($values | ForEach-Object { $_.label.Length } | Measure-Object -Maximum).Maximum
  foreach ( $value in $values ) {
    $toAdd = "    "
    if ( $null -ne $value.disabled -and $value.disabled -eq $true ) {
      $toAdd += "// "
    }
    $toAdd += $value.label
    if ( $null -ne $value.value ) {
      $toAdd += " = " + $value.value
    }
    $toAdd += ","
    $toAdd = $toAdd.PadRight( $maxLabelLength + 8 ) # 4 for indent, 3 for possible comment, 1 for comma
    if ( $null -ne $value.comment ) {
      $toAdd += " // " + $value.comment
    }
    $lines += $toAdd
  }
  return $lines
}
function createFunc( $funcName, $funcParms, $funcReturn, $indent, $funcLeader ) {
  $toAdd = $indent + "$funcLeader$funcName("
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
    }
    else {
      $toAdd += " "
    }
    $toAdd += $parms.name + $qual + ": " + $parms.type
    $pCount++
    $pad = " "
  }
  $toAdd += $pad + "): " + $funcReturn + ";"
  return $toAdd
}

function createFuncSet( $funcs, $indent, $funcLeader ) {
  $lines = @()
  foreach ( $function in ($funcs | Sort-Object -Property Name) ) {
    # function GetAccountCount(data: string): number;
    if ( $null -ne $function.params.sets ) {
      # Multiple definitions
      foreach ( $parmSet in $function.params.sets ) {
        $lines += createFunc $function.Name $parmSet.list $function.returns $indent $funcLeader
      }
    }
    else {
      # Singular definition
      $lines += createFunc $function.Name $function.params.list $function.returns $indent $funcLeader
    }
  }
  return $lines
}

$lines += @( "// Object Types", "" )

foreach ( $types in ($content.types | Sort-Object -Property name) ) {
  if ( $null -ne $types.definition ) {
    $lines += "  type $($types.Name) = $($types.definition);"
  }
  elseif ( $null -ne $types.values ) {
    # An enum!
    $decl = "  enum $($types.Name) {"
    $values = createTypeEnums $types.values
    $lines += @(
      $decl,
      $values,
      "  }"
    )
  }
  else {
    $decl = "  interface $($types.Name)"
    if ( $null -ne $types.extends ) {
      $decl += " extends " + $types.extends
    }
    $decl += " {"
    $parms = createTypeProps $types
    $funcs = createFuncSet $types.methods "    " ""
    $lines += @(
      $decl,
      $parms,
      $funcs,
      "  }"
    )
    if ( $null -ne $types.aka ) {
      $lines += "  type $($types.aka) = $($types.Name);"
    }
  }
}

$lines += @( "", "// Global Functions", "" )
$lines += createFuncSet $content.globalFuncs "  " "function "

$lines += @( "", "// Event Functions", "" )
$lines += createFuncSet $content.events "  " "function "

$lines += @(
  "}",
  "",
  "// Ensure the file is treated as a module",
  "export {};"
)

$lines