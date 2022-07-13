rm *.o
gcc -DPLOIDY=1 -DFRONTEND=1 -DTIERRA -c bookeep.c
gcc -DPLOIDY=1 -DFRONTEND=1 -DTIERRA -c decode.c
gcc -DPLOIDY=1 -DFRONTEND=1 -DTIERRA -c diskbank.c
gcc -DPLOIDY=1 -DFRONTEND=1 -DTIERRA -c frontend.c
gcc -DPLOIDY=1 -DFRONTEND=1 -DTIERRA -c tie_inc.c
gcc -DPLOIDY=1 -DFRONTEND=1 -DTIERRA -c instruct.c
gcc -DPLOIDY=1 -DFRONTEND=1 -DTIERRA -c memalloc.c
gcc -DPLOIDY=1 -DFRONTEND=1 -DTIERRA -c memtree.c
gcc -DPLOIDY=1 -DFRONTEND=1 -DTIERRA -c micromon.c
gcc -DPLOIDY=1 -DFRONTEND=1 -DTIERRA -c queues.c
gcc -DPLOIDY=1 -DFRONTEND=1 -DTIERRA -c rambank.c
gcc -DPLOIDY=1 -DFRONTEND=1 -DTIERRA -c slicers.c
gcc -DPLOIDY=1 -DFRONTEND=1 -DTIERRA -c tierra.c
gcc -DPLOIDY=1 -DFRONTEND=1 -DTIERRA -c trand.c
gcc -DPLOIDY=1 -DFRONTEND=1 -DTIERRA -c tsetup.c
gcc -DPLOIDY=1 -DFRONTEND=1 -DTIERRA -c ttools.c
gcc -DPLOIDY=1 -DFRONTEND=1 -DTIERRA -c operator.c
gcc -o tierra *.o -lm -lpc
call coff2exe tierra
if ERRORLEVEL 1 goto ERROR
rm *.o
rm tierra
gcc -DPLOIDY=1 -DARG -c arg.c
gcc -DPLOIDY=1 -DARG -c arg_inc.c
gcc -o arg *.o -lm -lpc
call coff2exe arg
rm *.o
rm arg
if ERRORLEVEL 1 goto ERROR
cd gb0
..\arg c 0080gen.vir 0080aaa.tie
cp 0080gen.vir 0080.gen
cd ..\gb1
..\arg c 0095gen.vir 0095aaa.tie
cp 0095gen.vir 0095.gen
cd ..\gb2
..\arg c 0093gen.vir 0093aaa.tie
cp 0093gen.vir 0093.gen
cd ..\gb3
..\arg c 0082gen.vir 0082aaa.tie
cp 0082gen.vir 0082.gen
cd ..\gb8
..\arg c 0082gen.vir 0082aaa.tie
cp 0082gen.vir 0082.gen
cd ..
goto DONE
:ERROR
echo THERE WAS A PROBLEM COMPILING !!!!
pause
:DONE
