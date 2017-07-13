# SPlotter

Windows 64-Bit Download: [HERE](https://github.com/SamuelNZ/SPlotter/releases/)

**Implemented AVX2 Please Update**

**Please take note of the new -repeat flag**


SPlotter is a light-weight BURST Plotter that creates pre-optimized plots of a given size until the drive is full or it makes the number of plots you specify.

SPlotter is also extremely useful for making larger plots to be copied to SMR/Seagate drives, You can safely copy the plot as soon as it starts working on the next one, It will take care of the start nonce for you so there is no risk of overlapping. I'm considering adding the ability to auto-move a plot to a specified drive.

# Usage

```
@setlocal
@cd /d %~dp0 
SPlotter.exe -id 17559140197979902351 -sn 0 -n 20000 -t 2 -path F:\burst\plots -mem 5G -repeat 10
```

#### Usage Breakdown

```
-id: Your BURST address numeric ID.
-sn: Nonce to Start at.
-n: Number of Nonces to Plot.
-t: Number of threads on your CPU you want to use.
-path: Path to your Plots.
-mem: Amount of RAM to use while plotting.
-repeat: number of extra plots you want to make of the same size.
-move: Path you want your plots moved too, Path must exist.
```

## Move Last Plot Automatically.


You can make SPlotter move the last plot by adding the -move flag to your config eg.
```
@setlocal
@cd /d %~dp0 
SPlotter.exe_avx2 -id 17559140197979902351 -sn 0 -n 20000 -t 2 -path F:\burst\plots -mem 5G -repeat 10 -move E:\burst\plots
```

This is only recommended for people who are using SSD drives to plot on or NVMe accererlated drives etc.

A separate progress % indicator will show up for each plot that is still moving, Please do not delete plots or try move them manually while you are using this feature.


## Resuming a Plot

To resume a plot you need to match the config to the existing plot file, eg.

```
17559140197979902351_20001_20000_20000
```
```
SPlotter_avx2.exe -id 17559140197979902351 -sn 20001 -n 20000 -t 6 -path F:\burst\plots -mem 5G
```


## Writing more than 1 set of plots at a time.

Because of the changes to how threading works in SPlotter you can run more than 1 Copy at a time and it won't cannibalize your CPU all the time. If you have more than 1 drive to plot this can be very useful because of the often slow scoop write times where your CPU and is basically doing nothing.

eg, If your best possible config is;
```
SPlotter_avx2.exe -id 17559140197979902351 -sn 20001 -n 20000 -t 10 -path F:\burst\plots -mem 16G
```

You could run two instances of:
```
SPlotter_avx2.exe -id 17559140197979902351 -sn 20001 -n 20000 -t 7 -path F:\burst\plots -mem 8G
```

The benefit of doing this is when one instance is writing to disc the other one will be using the resources its not using.

Even with a lower thread count this is faster overall if you need to plot multiple drives.

You can use the repeat and -move flags while doing this.

For best results start the second instance when you see the GREY writing scoops message.

Future versions will automatically start making the next plot as the nonces from the last plot are moved from RAM to DISK, But for now this is the midground.

If you run 2 or more instances at your best possible config you will not have a good time.

## Threads

You can find your number of threads with CPU-Z.

If you have Hyperthreading, You can almost double your thread count.

![Imgur](http://i.imgur.com/7PWoV97.png)


## General

A nonce is 256kb.

SPlotter will automatically make plots of the nonce size you specify until you run out of space on the drive.

Running as admin/root will also increase your plotting speed.

SPlotter is fully compatible with XPlotter.

Don't open issues for things you should already know or could ask google.


![Imgur](http://i.imgur.com/p090cKV.png)


## XPlotter

You might prefer the [original software](https://github.com/Blagodarenko/XPlotter) this is based on if you don't specifically need SPlotter for the repeating functionality which XPlotter doesn't have. Please be aware that XPlotter is only AVX1.

