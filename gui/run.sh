# change theme for build
# eg. "material", "cupertino"...
#       use "-light","-dark" to switch color
cmake -DSLINT_STYLE="fluent-dark" .
# build app
cmake -GNinja .
cmake --build .

# run app
./polonius-gui
