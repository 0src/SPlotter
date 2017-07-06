# SPlotter

Windows 64-Bit Download: [HERE](https://github.com/SamuelNZ/SPlotter/releases/)


SPlotter is a light-weight BURST Plotter that creates pre-optimized plots of a given size until the drive is full.

SPlotter is also extremely useful for making larger plots to be copied to SMR/Seagate drives, You can safely copy the plot as soon as it starts working on the next one, It will take care of the start nonce for you so there is no risk of overlapping. I'm considering adding the ability to auto-move a plot to a specified drive.

# Usage

```
@setlocal
@cd /d %~dp0 
SPlotter.exe -id 17559140197979902351 -sn 0 -n 20000 -t 2 -path F:\burst\plots -mem 5G
```

### Usage Breakdown

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

## Resuming a Plot

To resume a plot you need to match the config to the existing plot file, eg.

```
17559140197979902351_20001_20000_20000
```
```
SPlotter_avx.exe -id 17559140197979902351 -sn 20001 -n 20000 -t 6 -path F:\burst\plots -mem 5G
```

## General

A nonce is 256kb.

SPlotter will automatically make plots of the nonce size you specify until you run out of space on the drive.

Running as admin/root will also increase your plotting speed.

SPlotter is fully compatible with XPlotter.

Don't open issues for things you should already know or could ask google.


![Imgur](http://i.imgur.com/ejSocAq.png)


## XPlotter

You might prefer the [original software](https://github.com/Blagodarenko/XPlotter) this is based on if you don't specifically need SPlotter for the repeating functionality which XPlotter doesn't have.

