Remote AVRDUDE
--------------

Written for Linux. Mac might work.

Execute AVRDUDE on another machine.
Uses scp and ssh to run AVRDUDE on another machine, first copying any files, and afterwards copying any files back.

Personally I use it to run AVRDUDE on a Raspberry PI from my desktop to program AVRs on a connected breadboard using SPI.

It is assumed you can ssh without password (i.e. with ssh keys), hence no options to handle this.
Options are stored in a configuration file, in order to avoid conflicts with the options of AVRDUDE, now and in the future.

This file is located at $HOME/.local/remote_avrdude.conf

Options:

hostname:
    The hostname and optionally prefixed with username@
    For example:
        myraspberry
        pi@fruity

quiet:
    Display files being copied and transposed command line on the destination.
    Default is on.

clean:
    Delete remote files afterwards.
    Default is on.
