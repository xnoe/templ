# templ The ultra simple template system.

Written in C++ by Xnoe

Depends on https://github.com/xnoe/cpp-utf8/

Available on Gentoo via the Xnoverlay https://github.com/xnoe/Xnoverlay

# Introduction

Templ is a static-site "generator" aimed at aiding the creation of multi-pages sites. It uses a jekyll-esque "Front Matter" for each page. Currently it only performs formatting on `.html` format pages. Markdown support (maybe) soon:tm:.

# Usage

Templ is designed to be fairly simple to use. Place your site's files in the directory called `source`. Files and directories beginning with `_` will be ignored by Templ.

A basic "Front Matter" for a page maybe be:
```
---
layout: default
title: Hello World
url: https://example.com
---
```

This will cause Templ to use the file `source/_layouts/default.html` for the layout (where `{{ content }}` within that file will be replaced with the page's source) and define the variables `page.title` and `page.url` (accessible via `{{ page.title }}` and `{{ page.url }}`). Templ will also read configurations from the (badly named) `source/_config.yml` file. This isn't (yet) able to store YAML. Just values separated by `: ` (space included).

Templ is usable in some circumstances but still may be sub-par for people looking for an actual static site generator. It's also *very, very, **very*** slow!


Templ also has the ability to include files within outputted content. This is achieved with the `{% include example.file %}` "command". This will look in `source/_includes/` for the named file and insert it in to the document in place.

This allows you to have a separate `.css` file for your stylesheets while also including them directly inside your HTML file (for example, if you want the outputted document to work as a single HTML file.)

# Using outputted content

Formatted pages will be in the `output` folder, in the same directory that Templ is called from. 

# Known bugs

Not very bug tested! (Aka. don't use for prod!)

Occasionally gets in to an infinite loop if start / end tags for things are done properly, without any sanity check. (^C to escape and check your source files). This shouldn't be too major of a bug but is just a sign of my awful code :( Sorry All.

Still lacks many necessary features that some people would like to have.
