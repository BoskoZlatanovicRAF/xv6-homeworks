# xv6 OS Modifications – Homework Projects

This repository contains three separate homework assignments completed as part of an Operating Systems course.
All projects are based on the xv6 teaching operating system and implemented in C, with modifications to both the kernel and user space.

⸻

### 1.	Text Color & Background Customization

Goal:
Modify xv6 to support runtime text and background color changes during console output.

Features Implemented:
	•	Toggleable menu for color selection.
	•	Navigation through the color menu and selection of desired option.
	•	Independent choice of text and background colors for all subsequent typed output.

⸻

### 2.	Filesystem Enhancements & Encryption

Goal:
Extend xv6’s filesystem capabilities with support for large files and basic encryption.

Features Implemented:
	•	Support for large file creation, reading, writing, editing, and deletion using double indirect pointers in the inode structure.
	•	File content encryption and decryption on disk.
	•	New system calls:
int setkey(int key);   // Set encryption key
int setecho(int echo); // Enable/disable console echo
int encr(int fd);      // Encrypt file contents
int decr(int fd);      // Decrypt file contents
	•	User-space programs:
	•	setkey – sets the encryption key.
	•	encr – encrypts a file.
	•	decr – decrypts a file.
	•	blockwriter – writes large amounts of data to a file (for testing).

⸻

### 3.	Shared Memory Between Parent and Child Processes

Goal:
Add a kernel mechanism for shared memory between a parent process and its direct children.

Features Implemented:
	•	Parent process can register a shared memory segment with the OS.
	•	All direct children of that parent gain access to the same physical memory region in their own address space.
	•	Memory lifecycle management ensures:
	•	Processes run and terminate normally.
	•	Memory is properly freed when no longer in use.
	•	Only the owning process is responsible for freeing the shared memory.

⸻

Technologies & Skills:
	•	Language: C
	•	OS Internals: xv6 kernel, filesystem, process management
	•	Concepts Applied:
	•	Kernel data structures and memory management
	•	System call implementation
	•	User-space and kernel-space interaction
	•	Filesystem extensions
	•	Inter-process communication (IPC)
	•	Encryption basics
