#!/bin/bash
# scaffold.sh — Create a new project from the basic template
#
# Usage: ./scaffold.sh myproject
#
# Creates myproject/ with Makefile, loader.spec, pico_combined.c, agent.c
# ready to customize. Add a target to the root Makefile to build it.

set -e

if [ -z "$1" ]; then
    echo "Usage: ./scaffold.sh <project-name>"
    echo "Example: ./scaffold.sh myagent"
    exit 1
fi

NAME="$1"

if [ -d "$NAME" ]; then
    echo "Error: $NAME/ already exists"
    exit 1
fi

echo "Creating project: $NAME/"
mkdir -p "$NAME"

# Copy basic template
cp basic/Makefile "$NAME/Makefile"
cp basic/loader.spec "$NAME/loader.spec"
cp basic/pico_combined.c "$NAME/pico_combined.c"
cp basic/agent.c "$NAME/agent.c"

# Update output filenames in Makefile
sed -i "s/basic_pico/${NAME}_pico/g" "$NAME/Makefile"
sed -i "s/basic\.x64/${NAME}.x64/g" "$NAME/Makefile"
sed -i "s/basic\*/${NAME}*/g" "$NAME/Makefile"

# Update loader.spec
sed -i "s/basic_pico/${NAME}_pico/g" "$NAME/loader.spec"

# Update pico_combined.c
sed -i "s/Basic agent/${NAME} agent/g" "$NAME/pico_combined.c"

echo ""
echo "Project created! Next steps:"
echo "  1. Edit $NAME/agent.c with your agent logic"
echo "  2. Add mergelibs to $NAME/loader.spec as needed"
echo "  3. Add to root Makefile:"
echo ""
echo "     $NAME:"
echo "         @\$(MAKE) -C $NAME shellcode runner"
echo ""
echo "     ${NAME}-run:"
echo "         @\$(MAKE) -C $NAME run"
echo ""
echo "  4. Build: make $NAME"
echo "  5. Run:   make ${NAME}-run"
