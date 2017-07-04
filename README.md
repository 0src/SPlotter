# SPlotter

Windows 64-Bit Download: [HERE](https://drive.google.com/open?id=0B0Q42ssOM6b6d1YwQWFscFJZUjQ) or [HERE](https://github.com/SamuelNZ/SPlotter/releases/download/v1.2/SPlotter.zip)


SPlotter is a light-weight BURST Plotter that creates pre-optimized plots of a given size until the drive is full.

SPlotter is also extremely useful for making larger plots to be copied to SMR/Seagate drives, You can safely copy the plot as soon as it starts working on the next one, It will take care of the start nonce for you so there is no risk of overlapping. I'm considering adding the ability to auto-move a plot to a specified drive.

# Usage

```
@setlocal
@cd /d %~dp0 
SPlotter.exe -id 17559140197979902351 -sn 0 -n 20000 -t 2 -path F:\burst\plots -mem 5G
```

## Usage Breakdown

```
-id: Your BURST address numeric ID.
-sn: Nonce to Start at.
-n: Number of Nonces to Plot.
-t: Number of threads on your CPU you want to use.
-path: Path to your Plots.
-mem: Amount of RAM to use while plotting.
```

## Threads

You can find your number of threads with CPU-Z.

If you have Hyperthreading, You can double your thread count.

![Imgur](http://i.imgur.com/cv5pv7x.png)
;)

## General

A nonce is 256kb.

SPlotter will automatically make plots of the nonce size you specify until you run out of space on the drive.

You need to run as admin/root or the repeat functionality won't work.

Running as admin/root will also increase your plotting speed.

SPlotter is fully compatible with XPlotter.

Don't open issues for things you should already know or could ask google.


![Imgur](http://i.imgur.com/6RNroRy.png)


Part of the dead-line calculation appears to involve the location of the nonce in the plot relative to the center of the plot (not the drive) which means finding a nonce near the start or end of a plot is better and will give you a lower deadline. Which basically means having lots of small plots increases your chance of a lower deadline in some respects, This was the real reason i made this.


![Imgur](http://i.imgur.com/MQcQvCA.png)


## XPlotter

You might prefer the [original software](https://github.com/Blagodarenko/XPlotter) this is based on if you don't specifically need SPlotter for the repeating functionality which XPlotter doesn't have.

