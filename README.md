# templ The ultra simple template system.

Written in C++ by Xnoe

Depends on https://github.com/xnoe/cpp-utf8/

Available on Gentoo via the Xnoverlay https://github.com/xnoe/Xnoverlay

Hi! This is a super-duper simplistic (and probably awful) system for aiding in developing many-paged websites. Essentially, all you have to do is have the "layout" in a file called `source/_layout.html` and every other `.html` file will be inserted in to where `{{ content }}` resides within said page. It can only replace once!

Output will be in the `output` folder.

Not very bug tested! (Aka. don't use for prod!)
