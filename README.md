# Remote AVRDUDE

Written for Linux. Probably will work on MacOS.

Uses scp and ssh to run AVRDUDE on another machine, first copying any input files, and afterwards copying any output files back.

Personally I use it to run AVRDUDE on a Raspberry PI from my desktop to program AVRs on a connected breadboard using SPI. Partly written for fun ;)

It is assumed you can ssh without password (i.e. with ssh keys), hence no options to handle this.
Options are stored in a configuration file, in order to avoid conflicts with the options of AVRDUDE, now and in the future.

This file is located at ```$HOME/.local/remote_avrdude.conf```

## Options:

### hostname
The hostname and optionally prefixed with ```username@```. This is the only required option.
    
For example:
    
```myraspberry```
        
```pi@fruity```

### quiet
Display files being copied and transposed command line on the destination.

Default is on (false).

### scp
Where to find the scp execuatable, default is ```/usr/bin/scp```

### ssh
Where to find the ssh execuatable, default is ```/usr/bin/ssh```

### avrdude
Where to find the avrdude execuatable on the remote machine, default is ```/usr/local/bin/avrdude```

### remote_directory
Where on the remote to copy any files to or from. Defaults to ```/tmp```

## Example conf file
    # remote avrdude conf
    hostname=myraspberry
    avrdude=/usr/bin/avrdude
