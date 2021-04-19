cat /dev/null > www/tester/YoupiBanane/youpi.bla
cat /dev/null > www/tester/post_body/post_file
rm -rf www/tester/YoupiBanane/youpla.bla www/tester/put_test/file_should_exist_after .tempIN .tempOUT
make
echo "Webserv executing..."
./Webserv