# Docker images for refspec testing

These Dockerfiles should be used from the folder one level above.

To play with the refspec code, and assuming the image name is ```foo```,
a command similar to this one should be used:

```bash
docker run -it --rm -v /home/user/projects/lusee/refspec:/user foo bash
```
