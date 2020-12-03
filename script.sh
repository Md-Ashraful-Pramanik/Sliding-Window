gcc dll.c
echo '0 0 1011 5 0.2 0.3 500 2 ' | ./a.out | aha --black --title 'DLL-1605001-1' > dll1.html
sed -i 's/color:black;background-color:white;/color:black;background-color:yellow;/g' dll1.html
sed -i 's/background-color:#3333FF;/color:white;background-color:#3333FF;/g' dll1.html
sed -i 's/background-color:red;/color:white;background-color:red;/g' dll1.html
sed -i 's/background-color:lime;/color:white;background-color:green;/g' dll1.html

echo '0 1 1011 5 0.2 0.3 20 2 ' | ./a.out | aha --black --title 'DLL-1605001-2' > dll2.html
sed -i 's/color:black;background-color:white;/color:black;background-color:yellow;/g' dll2.html
sed -i 's/background-color:#3333FF;/color:white;background-color:#3333FF;/g' dll2.html
sed -i 's/background-color:red;/color:white;background-color:red;/g' dll2.html
sed -i 's/background-color:lime;/color:white;background-color:green;/g' dll2.html

echo '1 0 1011 5 0.2 0.3 500 2 ' | ./a.out | aha --black --title 'DLL-1605001-3' > dll3.html
sed -i 's/color:black;background-color:white;/color:black;background-color:yellow;/g' dll3.html
sed -i 's/background-color:#3333FF;/color:white;background-color:#3333FF;/g' dll3.html
sed -i 's/background-color:red;/color:white;background-color:red;/g' dll3.html
sed -i 's/background-color:lime;/color:white;background-color:green;/g' dll3.html

