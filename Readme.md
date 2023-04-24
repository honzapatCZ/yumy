Yocto firmware for the V3s based smart drink machine Yumy

## Build

### Image
```
build.sh -i
```
Resulting image `core-image-minimal-yummy-{timestamp}.rootfs.wic.gz` will be at `build\tmp\deploy\images\yummy`\
Flash it with your favorite software(Balena Etcher, Rufus)

### SDK
```
build.sh -s
```
resulting env setup will be at `build\tmp\deploy\sdk\yummy`

### Dependency Graph
```
build.sh -g
```


## Credits
https://medium.com/@mitchgaines0/yocto-and-rauc-on-your-sunxi-allwinner-device-for-over-the-air-ota-updates-b423660b6d60
https://github.com/voloviq/meta-licheepizero
https://rauc.readthedocs.io/
https://linux-sunxi.org/
