# UPD_receiver

The application was built in WSL. After the start of the application two options will be offered: enter IP or "q" to exit. The correct IP is from 0.0.0.0 to 255.255.255.255. After entering the IP correctly, you will be prompted to enter PORT. The correct PORT is from 1024 to 65535. The database is created in the user's folder. To check the database, write in console:\
$ sqlite3\
sqlite> .open "test.db"\
sqlite> select * from Packets;
