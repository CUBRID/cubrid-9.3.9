# cubrid-9.3.9
CUBRID Engine 9.3.9. Release

Quick Build Hint:

1. copy cubrid-manager-server as 'cubridmanager'
2. ./autogen.sh --prefix=$CUBRID --enable-64bit --with-cmserver
3. find . -type f | xargs touch -t `date +%Y%m%d%H%M`
4. make -j dist-bin
