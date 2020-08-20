# TTY Snoop #

### Overview ###
The intent of this project is to provide an easy way for new and experienced linux users to browse and view the activity on all ttys on a system in real time. When I was a new unix/linux user in the 90s, after having run a DOS WWIV bbs for a couple of years, this was something I had felt would've been useful at the time. Years later, I have decided to implement this, mostly for fun. 

### Implementation Notes ###
I plan to have multiple implementations for gathering the data. One kernel module implementation, and perhapse one using sys/procfs. The kernel module implementation is just for fun is likely not the best way to do this. The reason for this is to learn more about kernel data structures and facilities available.

For the user interface, see 3 possible iterations:
1. simple ncurses interface to for viewing the tty activity and metadata
2. ncurses interface that can support connecting to a remote host 
3. Web interface, electron, or GTK.


