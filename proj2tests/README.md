

# Firewall

For this part of the tests, i did not find another way to test the new syscalls other than
testing it using the terminal itself. I will be using shell scrips to be able to use the terminal
and the new syscalls.

For the firewall part of the project, i  will be using netcat which are commands that you can
use in the terminal so that we can connect to both TCP and UDP connection on a desired port.
I will mention that i will be using the drivers we are suppose to implement on proj2drivers.
i did not want to create this files in a separate folder so i wanted to re use this to help me
test the ports that are blocked on my data structure.

The link that helped me with netcat:
https://www.digitalocean.com/community/tutorials/how-to-use-netcat-to-establish-and-test-tcp-and-udp-connections-on-a-vps
https://www.computerhope.com/unix/nc.htm
https://ubidots.com/blog/how-to-simulate-a-tcpudp-client-using-netcat/

# AC Files

For the blocking files in the second part of the project, I have used the same function on the
proj2drivers. Sadly, i did not get to finish this part. My data structure does save the path ,but it does not block it on
the link_path_walk(). I have tried many parts of the inside the function link_path_walk() but since the path is built iteratively i could not find the end of the absolute path. I have put many comments where i tried to put
my function on the link_path_walk() function on the namei.c file.I should mention that my data structure does work saving the file/path and if we want to query it does say it exist., or if we want to block an existing path it also says its blocked already. my locking function in the link_path_walk()  is the only one that it is not locking correctly yet.


# Run Script

In order to run the tests I have created, you can go to the terminal and where the file are in the directory proj2tests/
you must run this script as root and you type "sh firewallTest.sh" for the part part one of the project and "sh acFileTest.sh" for the second part of the project.
Commands:

sh firewallTest.sh
sh acFileTest.sh
