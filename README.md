# Graph_lib

A library for directed and undirected networks and related graph algorithms. WIP.

# Compilation and Running 

If you want to use [`micromamba`](https://mamba.readthedocs.io/en/latest/user_guide/micromamba.html) as the package manager, create and activate the environment.

```bash
micromamba create -f environment.yml
micromamba activate seldonenv
```

We use `meson` to compile and build Seldon. 

```bash
meson setup build
meson compile -C build
```

To install `seldon` to your `conda` environment, run the following:

```bash
meson setup build --prefix $CONDA_PREFIX
meson install -C build
```

## Running Tests

To run the tests, go into the build directory and run the following: 

```bash
meson test
```
