![mishmesher logo](md-imgs/MishMesher-Logo.jpg)

> MishMesher is the result of my Master's Thesis project in collaboration with SAAB AB.
>
> ## Thesis Identifiers
URN: urn:nbn:se:uu:diva-542556
OAI: oai:DiVA.org:uu-542556
DiVA, id: diva2:1912711

**MishMesher is an Effective, Parallel, Multi-Layer, Uniform Cartesian Block Mesh Generator.**
## Basic Functionality
- Imports, parses and repairs .OBJ model files.
- Creates in parallel, a block mesh representation of the imported model.
- Supports multiple layers, and generates a separate block mesh for each.
- Intersects all layers before export.
- Can export block mesh to .OBJ or to .MSH (novel file format).

## Project Parts and Achievements
- Develop a block mesh generator - **MishMesher**
-- Mishmesher is approximatelly **12x** faster than previously used solutions.
-- Able to generate block meshes in resolutions beyond what previous solutions could.
- Develop a new, effective file format for 3D block mesh models - **.MSH**
-- .MSH manages to store multi-layered 3D block meshes in **300x** less storage space when compared to previous solutions.
-- .MSH can be zipped, achieving up to **20600x** smaller files than previous solutions.

## Installation
-- MishMesher is fully programmed in C, so you need **GCC** or some other C compiler to compile the script.
-- Make sure **atomics** are enabled and supported by your system.
-- Make sure OpenMP is installed or included in your compiler.

- Download the repository and navigate to it via the terminal.
- Run `make mishmesh` to compile MishMesher
```sh
cd /path/to/downloaded_repo
make mishmesh
```

MishMesher should now compile into an executable called `mishmesh`.
## .MPROP Files
MPROP = Material properties
- Currently, mprop doesn't do much but can be expanded to specify different mesh or intersection techniques.

To run MishMesher you need to have an associated .mprop file with the .obj model you would like to mesh.
The .mprop file currently represents the properties of each layer in the model, and if it should be hollow or solid.
The layout of the .mprop file:
```sh
l 1
f 0
l 2
f 1
...
..
.
```
Lines starting with `l` represent layers, in the order they appear.OBJ file.
The following lines, currently only `f`, represent the layer properties.
`f` stands for fill, `0 = hollow`, `1 = filled/solid`.
## Running MishMesher
Running MishMesher is relatively simple, the parameters are outlined as follows:
- `"path_to_obj_file"`
- `"path_to_mprop_file"`
- `"path_to_output"` What will the .msh file be called, and what directory will it be exported to?
-  `The desired cell size` How big each cell will be in the final domain, smaller = higher resolution.
-  `Number of threads` How many threads to allocate to generate the block mesh model.

Here is an example of how to run MishMesher:
```sh
./mishmesh "models/square.obj" "models/square.mprop" "msh_out/mish_file_simple" "0.3" 1
```

## License
OSL 3.0

**Free/Open Source Software! Hell Yeah!**

If you have made any changes or improvements that you would like to share with others, message me!
