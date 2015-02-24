rm -rf soundfiles
mkdir soundfiles

outExtension=""
songDir="songs"
outDir="noise"

gcc -w -g -std=c99 FuzzBox.c -o Fuzz -lm

echo "Generating songs"

#gets all songs, * is the wildcard operator
for inputFile in `ls $songDir/*.song`; do
	inputFileBase=`basename $inputFile .song`
	echo "composing $inputFile"

	./Fuzz < $inputFile > $outDir/$inputFileBase

done