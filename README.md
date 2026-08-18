erwisE 
---
This is an old web browser based on X11/Motif.

For more info, see the Wikipedia page: https://en.wikipedia.org/wiki/Erwise

Changes:
* Fixed a few ancient bugs, including one use-after-free.
* Better headers included.
* Convert to ANSI C, removed K&R methods. I hate K&R.
* Removed some legacy cruft.
* Migrated to `<stdbool.h>` as well as marking some strings as `const char*`.

If you want to build and run it, you will need X11 and a flavor of Motif. You may need to change the Makefiles to point to the right location.
It was designed around UNIX and POSIX. If your operating system uses something different, you might need to do more work on it.


What works:
* Simple web pages load.
* Double-clicking on a link opens the link.

What does not work:
* HTTPS has not been tested.
* Any other text encoding other than ASCII and/or ISO Latin 1: Unicode text isn't rendered correctly.
* Custom styles defined by the web page.
* Documentation seems to have disappeared from the internet. If someone could find the requested HTML documents, that would be appreciated. 
