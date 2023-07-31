# Docker images for refspec testing

## General Comments

These Dockerfiles should be used from the folder one level above, e.g.
```bash
docker build . -t lusee/lusee-night-refspec-foundation:1.0 -f docker/Dockerfile-foundation
```

To run with a local copy of the _refspec_ code, a command similar to the following one should be used,
substituting "user" with the desired user name:

```bash
docker run -it --rm -v /home/user/projects/lusee/refspec:/user buddhasystem/lusee-night-refspec-foundation:1.0 bash
```

In the above, the local user directory (which needs to be adjusted
to the actual situation) containing the refspec folder is mapped to
the internal ```/user``` directory in the running container.

## Foundation

The _foundation_ image is based on a recent version of __Debian__, with FFTW software
added. The intent is to keep this relatively static part intact, building images
such as _lusee/lusee-night-refspec-cppyy:1.0_ on top of it.

## Environment

The `Dockerfile-cppyy` contains proper setting for the environment variables `LD_LIBRARY_PATH`
and `CPATH`. The latter is necessary for the `cppyy` interpreter to work.

