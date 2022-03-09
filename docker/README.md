# Docker images for refspec testing

These Dockerfiles should be used from the folder one level above.

To check the refspec code, a command similar to the following one should be used
at the time of writing:

```bash
docker run -it --rm -v /home/user/projects/lusee/refspec:/user buddhasystem/lusee-night-refspec-foundation:0.1 bash
```

In the above, the local user directory (which needs to be adjusted
to the actual situation) containing the refspec folder is mapped to
the internal ```/user``` directory in the running container.
Usual ```bash``` commands can be used interactively at this point.
