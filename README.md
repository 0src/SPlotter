# SPlotter

Windows 64-Bit Download: [HERE](https://github.com/SamuelNZ/SPlotter/releases/)

**Implemented AVX2 Please Update**

**Please take note of the new -repeat flag**

**Please take note of the new -move flag**


SPlotter is a light-weight BURST Plotter that creates optimized plots of a given size until the drive is full or it makes the number of plots you specify with the ability to auto-move the last plot to a destination drive.

This is particularly useful for creating plots for SMR/Seagate/External drives or creating plots on an NVMe or SSD and moving them automatically.

You can run more than one instance of SPlotter at a time without cannabilizing your CPU, This means if you have more than one drive to plot too that you can reduce the amount of CPU time you waste during write times. Read more below.



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
-RADW: Set to 1 if you are using an accelerated drive to move plots.
```

![Imgur](http://i.imgur.com/60rUvFi.png)

### -move

You can make SPlotter move the last plot by adding the -move flag to your config eg.
This will stop plotting and move the last plot to the destination you specify.

```
@setlocal
@cd /d %~dp0 
SPlotter.exe_avx2 -id 17559140197979902351 -sn 0 -n 20000 -t 2 -path F:\burst\plots -mem 5G -repeat 10 -move E:\burst\plots
```
Please do not delete plots or try move them manually while you are using this feature. 
Its a copy/move and doesn't delete the plot unless its successful.

### -RADW

```
@setlocal
@cd /d %~dp0 
SPlotter.exe_avx2 -id 17559140197979902351 -sn 0 -n 20000 -t 2 -path F:\burst\plots -mem 5G -repeat 10 -move E:\burst\plots -RADW 1
```

This changes the default behavior of -move and makes it so it doesn't stop to move the last plot and starts working on the next plot immediately. This feature is not meant for most drives.

If you are using a SSD or an Accelerated drive then you should add -RADW 1 flag to your config.

You can only see the progress of the last plot that started moving because if your drive isn't fast enough to move the last plot before the next one finishes i don't recommend using this feature because you will eventually stall your drive.

Once more, This feature is not meant for most drives.

### -repeat

Repeat allows you to specify how many plots of a certain size you want, This is very useful if you are moving plots automatically from a much smaller drive or want to make lots of small plots.

![Imgur](http://i.imgur.com/EsiA1wd.png)


### Resuming a Plot

To resume a plot you need to match the config to the existing plot file, eg.

```
17559140197979902351_20001_20000_20000
```
```
SPlotter_avx2.exe -id 17559140197979902351 -sn 20001 -n 20000 -t 6 -path F:\burst\plots -mem 5G
```


### Writing more than 1 set of plots at a time.

Because of the changes to how threading works in SPlotter you can run more than 1 Copy at a time and it won't cannibalize your CPU all the time. If you have more than 1 drive to plot this can be very useful because of the often slow scoop write times where your CPU and is basically doing nothing and for certain kinds of externals that write extremely slow...

3 Turtle Pace drives sharing a cpu without dying.
![Imgur](http://i.imgur.com/Kzw4Hui.png)

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

You can use the -repeat and -move flags while doing this.

For best results start the second instance when you see the GREY writing scoops message.

If you run 2 or more instances at your best possible config you will not have a good time.

## Threads

You can find your number of threads with CPU-Z.

If you have Hyperthreading, You can almost double your thread count.

![Imgur](http://i.imgur.com/7PWoV97.png)


## General

SPlotter will automatically make plots of the nonce size you specify until you run out of space on the drive.

Running as admin/root will also increase your plotting speed.

SPlotter is fully compatible with XPlotter.

Don't open issues for things you should already know or could ask google.

## Nonces

A nonce is 256kb.

1GB is 4096 nonces.

100GB is 409600 nonces.

1TB is 4096000 nonces.

If you have left-over when the plotter is done its because of how your HDD manufacturer determined 1GB, Its not an error.


![Imgur](http://i.imgur.com/SC3rW92.png)


### Yuppers

![Yeap](http://i.imgur.com/lSSuWR1.png)

