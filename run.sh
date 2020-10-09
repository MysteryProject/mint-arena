cp -a lua/ "/e/msys2/home/mecwe/projects/q3dev/game/baseq3"

cd code/json
zip -r json.pk3 *.json
mv json.pk3 "/e/msys2/home/mecwe/projects/q3dev/game/baseq3"

cd ../../build/release-mingw64-x86_64/baseq3
mv *.dll "/e/msys2/home/mecwe/projects/q3dev/game/baseq3"

cd ../../../
./../game/Spearmint-Q3.exe
