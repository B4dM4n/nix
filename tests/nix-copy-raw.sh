source common.sh

clearStore

path0=$(nix-build nix-build-examples.nix -A input0)
pathBody=$(nix-build nix-build-examples.nix -A body)
drv0=$(nix-instantiate nix-build-examples.nix -A input0)
drv1=$(nix-instantiate nix-build-examples.nix -A input1)
drvBody=$(nix-instantiate nix-build-examples.nix -A body)

path0Base=$(basename $path0)
pathBodyBase=$(basename $pathBody)
drv0Base=$(basename $drv0)
drv1Base=$(basename $drv1)
drvBodyBase=$(basename $drvBody)

nix copy-raw --no-check-sigs --to $cacheDir $path0

# no drv's should be present
([ -e $cacheDir/nix/store/$path0Base ])
(! [ -e $cacheDir/nix/store/$pathBodyBase ])
(! [ -e $cacheDir/nix/store/$drv0Base ])
(! [ -e $cacheDir/nix/store/$drv1Base ])
(! [ -e $cacheDir/nix/store/$drvBodyBase ])

nix copy-raw --no-check-sigs --to $cacheDir $drvBody

# all dependent drv's should be present, but not the drv output
([ -e $cacheDir/nix/store/$path0Base ])
(! [ -e $cacheDir/nix/store/$pathBodyBase ])
([ -e $cacheDir/nix/store/$drv0Base ])
([ -e $cacheDir/nix/store/$drv1Base ])
([ -e $cacheDir/nix/store/$drvBodyBase ])
