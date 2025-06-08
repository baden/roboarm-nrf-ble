Import("env", "projenv")

# For verification, extract the executable in .bin format from the UF2 file.
# e.g. python2 uf2conv.py --convert --output firmware.bin --base 0x08004000 firmware.uf2
env.AddPostAction(
    "$BUILD_DIR/${PROGNAME}.uf2",
    env.VerboseAction(" ".join([
        "python", "uf2conv.py",
            "-c",
            "-f", "0xADA52840",
            "--output", "$PROJECT_DIR/${PROGNAME}.uf2",
            "$PROJECT_DIR/${PROGNAME}.hex"
    ]), "Building $PROJECT_DIR/${PROGNAME}.uf2")
)
