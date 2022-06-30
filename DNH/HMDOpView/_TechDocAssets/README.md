This folder contains source files for software architecture diagrams related to the the HMDOp.

# Building with PlantUML

The files ending with the filename pattern uml_*.plantuml are text files representing diagrams in a text language called PlantUml. A renderer can be obtained from [PlantUML's website](https://plantuml.com/download). It is a Java application and therefore also requires Java to be installed on the machine rendering the assets.

The syntax for various diagram types are documented on their webpage.

# Rationale

Previously, diagrams were created as SVG drawings in Inkscape. This requires knowledge of Inkscape and enough knowledge of UML to diagram drawings freehand. Another option was to create screenshots of UML digrams authored in MagicDraw, which requires a MagicDraw license and application training.

While PlantUML requires the use of a some-what abstract text modeling language, it is well documented, concise, quick to iterate, and automates a lot of the superficial drawing work. As well as being free and opensource software.

## Editing

If editing the documents, it is recommended that a GUI editor with a preview to be used. There are some browser applications for this online, examples: [1](https://www.plantuml.com/plantuml/uml/), [2](https://www.planttext.com/), [3](https://plantuml-editor.kkeisuke.com/).


## Rendering

The directory comes with an sh script to render the *.plantuml files, `generate_assets.sh`

To use the script:

1. Open bash and CD to `_TechDocAssets`.
2. Make sure Java is installed. To verify bash can find it, type `where java`. At least 1 path to java.exe should be returned.
3. Download a *.jar of plantuml.
4. Rename `locate_plantuml._sh` to `locate_plantuml.sh`.
5. Modify `locate_plantuml.sh` in a text editor; change the echoed path to where the plantuml jar is on your machine.
6. Run the script, `. ./generate_assets.sh`.
7. All the uml_*.plantuml files should be rendered as *.png and *.svg files.

Note that since these assets are built from the version controlled source files, the generated assets are listed in the .gitignore and not stored in the repo.



# Update Policy

Note that these diagrams may be out of date as they are not expected to stay in perfect sync and reflect the codebase at every given commit. Instead, it is expected that they may become out of date (hopefully not too much) and at certain milestones they will be revisited and effort will be placed on updating them.