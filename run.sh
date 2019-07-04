cp -a lua/ "/x/q3/build/baseq3/"

cd code/json
zip -r json.pk3 *.json
mv json.pk3 "/x/q3/build/baseq3"

cd ../../build/release-mingw64-x86_64/baseq3
mv *.dll "/x/q3/build/baseq3"

cd ../../../
./../build/Spearmint-Q3.exe
