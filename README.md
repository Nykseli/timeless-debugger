# Timeless Debugger

## Building

```
# First pull the submodule(s)
git submodule update --init --recursive

# Then build it
./build_module.sh

# Build the program
make

# (Optional) Build the examples
make -C examples
```

## Usage

```
./debugger <program>

# For example
./debugger examples/simple
```
