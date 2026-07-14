# CNN Compiler for custom CNN Accelerator

cd ~/TEST
rm -r -f CNN-Compiler


git clone https://github.com/malisaber/CNN-Compiler.git

cd  CNN-Compiler
make


./CNN-Compiler -v \
-n networks/network_5.json \
-m materials \
-d Dump \
-o Out \
-i /mnt/e/WSL/data_gen/IDG/Input_1.bin \
-w /mnt/e/WSL/data_gen/WDG/Weight_1.bin