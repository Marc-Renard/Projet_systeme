# ProgSystem-project
First year of master degree project in System coding

(still work in progress)

The goal of this project is to emulate a situation where a main server has computation capabilities that clients have not. Therefore the clients interact with the server using a shared memory object, allowing the server to make computation for them and send them the results afterwards. This project was made in C, with @paberthet. It s aimed at Linux operating systems.

We recommand using gdb to run this project, especially since the computations made by server are fast and therefore the client won t interact with it very long.


First download the files and open two (or more) command terminals in the same folder.

On the first one, type "make" to compile the project. We advise making a "make clean" just before that "make" in case you are rerunning the program. Then type "./serveur mem". You can replace "mem" with whatever name you see fit. It'll be the shm name. You can inspect /dev/shm on your computer to ensure it has been created.

In the second terminal type "./lpc_client mem 1". Ensure to replace mem by the shm name from the server side. For the "1", replace it with whatever test you want to try.


Also check Pierre-Augustin github at https://github.com/paberthet , he's a great guy too
