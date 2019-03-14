# Linux-Disk-Info
A work home about linux partitions 

# What does it do?
* List disks and partitions
* Print disk sectors 
* Find a specific string in a disk
* View MBR informations for a disk

# Output examples 
The default directory is /dev

Sector size is by default 512 bytes


* listing disks and partitions

![alt text](https://github.com/AghilesAzzoug/Linux-Disk-Info/blob/master/screens/listing_partitions.PNG)

* Printing the 2nd sector of /dev/sda

![alt text](https://github.com/AghilesAzzoug/Linux-Disk-Info/blob/master/screens/sector2_sda.png)

* Searching for the string 'GRUB' in /dev/sda from sector 0 to sector 1
![alt text](https://github.com/AghilesAzzoug/Linux-Disk-Info/blob/master/screens/GRUB_sector_0_search.PNG)

One occurrence was found in sector 0

* Displaying MBR informations for /dev/sda
  * Disk identifier (in hex) : 0xC4380CB
  * Partitions table (in hex) : starting from offset 0x1BE 
  * And for every partition, its type : primary partition, its file system : EXT and starting address : 2048 (in decimal)

![alt text](https://github.com/AghilesAzzoug/Linux-Disk-Info/blob/master/screens/MBR_dev_sda.png)

The program can also print logical partitions using EBR informations.

The variables, functions and printed strings are also written in french, sorry about that.
