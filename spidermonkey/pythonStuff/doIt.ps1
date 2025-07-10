$files = @(
  "imacro_asm.py",
  "imacros.jsasm",
  "jsopcode.tbl"
)

Push-Location $PSScriptRoot

# Copy files in
foreach( $f in $files ) {
  Copy-Item "..\$f" -Destination . -Force
}

docker build -t imacros .

docker run -d --rm --name imacros imacros sh -c 'while sleep 3600; do :; done'
docker cp imacros:/tmp/imacros.c.out .
docker stop imacros

Move-Item imacros.c.out .. -Force

# Cleanup files
foreach( $f in $files ) {
  Remove-Item $f -Force
}

Pop-Location