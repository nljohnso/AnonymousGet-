AnonymousGet-
=============
Usage:

./ss [port] 

Example:

./ss 5566

This is a stepping stone.  The stepping stone's ip address and port number should both be recorded in the chainFile.txt file. The format of this file is as follows:

<SSnum>
<SSaddr, SSport>
<SSaddr, SSport>
<SSaddr, SSport>
...

Example:

<2>
<129.82.46.205, 5566>
<129.82.46.205, 5567>

After all of the necessary stepping stones are running and the chainFile.txt has been updated you can now run awget.

./awget [URL file] [chain file]

Example:

./awget www.google.com/index.html chainFile.txt
