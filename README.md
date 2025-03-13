It's a little c program that renders a 3d "π".

Inspired by https://www.a1k0n.net/2006/09/15/obfuscated-c-donut.html

File description:

----not_so_pi.c: a version of the code that has comments in it.
  
----pi.c: the main program. (excuse me for my amateurish obfuscated C attempt)
  
----pi.exe: the compiled program, maximize the command line window to get a better view. (so that the "π" won't be flashing up and down)
  

You can see the comments in the not_so_pi.c codes, and change some parameters to your liking~

Also you can change the "system("cls");" to "printf("\x1b[H");" (as in the donut.c), but its flashing effect makes me choose the former implementation instead.
