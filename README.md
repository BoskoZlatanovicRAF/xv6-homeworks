# xv6 OS Modifications – Homework Projects

This repository contains three separate homework assignments completed as part of an Operating Systems course.
All projects are based on the xv6 teaching operating system and implemented in C, with modifications to both the kernel and user space.

⸻

### 1.	Text Color & Background Customization

Goal:
Modify xv6 to support runtime text and background color changes during console output.

Features Implemented: </br>
	•	Toggleable menu for color selection. </br>
	•	Navigation through the color menu and selection of desired option. </br>
	•	Independent choice of text and background colors for all subsequent typed output. </br>

⸻

### 2.	Filesystem Enhancements & Encryption

Goal:
Extend xv6’s filesystem capabilities with support for large files and basic encryption.

Features Implemented: </br>
	•	Support for large file creation, reading, writing, editing, and deletion using double indirect pointers in the inode structure. </br>
	•	File content encryption and decryption on disk. </br>
	•	New system calls: </br>
int setkey(int key);   // Set encryption key </br>
int setecho(int echo); // Enable/disable console echo </br>
int encr(int fd);      // Encrypt file contents </br>
int decr(int fd);      // Decrypt file contents </br>
	•	User-space programs:</br> 
	•	setkey – sets the encryption key.</br>
	•	encr – encrypts a file.</br>
	•	decr – decrypts a file.</br>
	•	blockwriter – writes large amounts of data to a file (for testing).</br>

⸻

### 3.	Shared Memory Between Parent and Child Processes

Goal:
Add a kernel mechanism for shared memory between a parent process and its direct children.

Features Implemented:</br>
	•	Parent process can register a shared memory segment with the OS.</br>
	•	All direct children of that parent gain access to the same physical memory region in their own address space.</br>
	•	Memory lifecycle management ensures:</br>
	•	Processes run and terminate normally.</br>
	•	Memory is properly freed when no longer in use.</br>
	•	Only the owning process is responsible for freeing the shared memory.</br>

⸻

Technologies & Skills:</br>
	•	Language: C</br>
	•	OS Internals: xv6 kernel, filesystem, process management</br>
	•	Concepts Applied:</br>
	•	Kernel data structures and memory management</br>
	•	System call implementation</br>
	•	User-space and kernel-space interaction</br>
	•	Filesystem extensions</br>
	•	Inter-process communication (IPC)</br>
	•	Encryption basics</br>
