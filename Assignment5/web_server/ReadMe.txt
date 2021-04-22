There is a folder called "web_server" in lab5 in the home direcory where all the necessary files are kept.

In case of single threaded webserver when we execute the given example, ie. "wget -P test1 -nd -p localhost:7090/index.html" and "wget -P test2 -nd -p localhost:7090/index.html",
we observe that at a time only one of the 2 requests is processed while the other has to wait.

In the modified multi-threaded webserver when we execute the same example, we observe that both requests are processed simultaneouly.
Currently there is an upper limit of max 10 threads, which can be easily changed as per needs.
This means currently at max 10 threads can execute simultaneously.

To compile and build the dynamic webserver use "make webserver_dynamic", then to run it use "./webserver_dynamic 7200". After building and executing the dynamic server, start two new sessions and then execute the examples given, ie., "wget -P test1 -nd -p localhost:7090/index.html" and "wget -P test2 -nd -p localhost:7090/index.html".
It can be seen that both requests are processed simultaneously.
After the completion if we wish to delete the executable file for dynamic webserver then we may use "make clean_dynamic".

Similarly we can use "make weebserver_pool"to compile and build the thread pool server. to run we use "./webserver 7200 <number of threads> <scheduling mode>", available scheduling modes are FCFS and SFF.
We may use the examples as used in the dynamic thread server to test this program, or directly use the shell script given.
After the completion if we wish to delete the executable file for thread pool webserver then we may use "make clean_pool".

The results obtained after running the test shell script file is as follows :
"Total runtime dynamic with 10 parallel requests is 107.197686303
Total runtime pool with 10 parallel requests with FCFS and 5 pool threads is (1st test) 140.746285616
Total runtime pool with 10 parallel req with SFF and 5 pool threads is (2nd test) 136.123134186".