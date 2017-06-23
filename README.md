# SPlotter

Windows 64-Bit Download: [HERE](https://drive.google.com/open?id=0B0Q42ssOM6b6d1YwQWFscFJZUjQ)


SPlotter is a light-weight BURST Plotter that creates pre-optimized plots of a given size until the drive is full allowing you to get the best dead line possible for your hard drives, Part of the dead-line calculation appears to involve the location of the nonce in the plot.


This is a light-weight version of [XPlotter](https://github.com/Blagodarenko/XPlotter), Please support them.


# Usage

```
@setlocal
@cd /d %~dp0 
SPlotter.exe -id 17559140197979902351 -sn 0 -n 20000 -t 2 -path F:\burst\plots -mem 5G
```

SPlotter will automatically make plots of the size you specify until you run out of space.

You need to run as admin/root or the repeat functionality won't work.

Running as admin/root will also increase your plotting speed.

SPlotter is fully compatible with XPlotter.

![Imgur](http://i.imgur.com/6RNroRy.png)
