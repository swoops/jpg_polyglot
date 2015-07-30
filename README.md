# jpg_polyglot
Make Javascript/JPG polyglots to annoy your friends

                                                                                
# Thanks
This is bassed off the work of Saumil Shah's work.

Additionally, thanks to Ange Albertini for his work on polyglots and his 
wonderful posters.

# How to make it go
```
jpg_polyglot ▶ make
gcc -Wall -o jpg_polyglot jpg_polyglot.c
jpg_polyglot ▶ cat /tmp/alert.js

alert("ALERT FUN!!!");
jpg_polyglot ▶ ./jpg_polyglot /tmp/soft.jpg /tmp/soft_poly /tmp/alert.js
[*]DEBUG: hbuff_s(11557) + /x2a/x2f=0; + js(24) + tbuff(472) = 2f1a
jpg_polyglot ▶ file /tmp/soft_poly 
/tmp/soft_poly: JPEG image data, JFIF standard 1.01, aspect ratio, density 1x1, segment length 12074, baseline, precision 8, 90x109, frames 3
jpg_polyglot ▶ strings -a /tmp/soft_poly|grep alert
alert("ALERT FUN!!!");
```
