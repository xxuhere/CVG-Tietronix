
# If called ./generate_assets.sh --clean, instead of doing the conversion,
# remove any files that matche what would have been previously generated.
if [[ "$*" == *"--clean"* ]]
then
    find . -iname "uml_*.png" -delete
	find . -iname "uml_*.svg" -delete
	return
fi

if [[ "$*" == *"--list"* ]]
then
    find . -iname "uml_*.png"
	find . -iname "uml_*.svg"
	return
fi

echo "------------------"
echo "Listing found *.plantuml files:"
echo ""
ls *.plantuml
echo "------------------"

echo "Generating png files from *.plantuml files."
java -jar "$(. ./locate_plantuml.sh)" *.plantuml
echo "    DONE!"

echo "Generating svg files from *.plantuml files."
java -jar "$(. ./locate_plantuml.sh)" -tsvg *.plantuml
echo "    DONE!"
